/*
Copyright (c) 2019, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GattLeAdvertiserManager.hpp"
#include "GattDBusService.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeAdvertiserManager"

#define TYPE_TO_STR(...) ((const char[]){__VA_ARGS__, 0})
namespace gatt {
namespace ipc {
GattLeAdvertiserManager::GattLeAdvertiserManager(GattDBusService *dbus_service)
    : gatt_dbus_service_(dbus_service) {
  ALOGD(LOGTAG " %s", __func__);
}

GattLeAdvertiserManager::~GattLeAdvertiserManager() {
  sd_bus_slot_unref(sdbusSlot_);
  sdbusSlot_ = nullptr;
}

bool GattLeAdvertiserManager::InitialiseDBusService() {
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (!sdbus) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  ALOGD(LOGTAG " %s", __func__);
	  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("StartAdvertisingSet", "a{sv}", nullptr,
          GattLeAdvertiserManager::sd_StartAdvertisingSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("StopAdvertisingSet", nullptr, nullptr,
          GattLeAdvertiserManager::sd_StopAdvertisingSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("GetOwnAddress", "i", nullptr,
          GattLeAdvertiserManager::sd_GetOwnAddress,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("EnableAdvertisingSet", "ibii", nullptr, //TODO: Consider: "ia{sv}"
          GattLeAdvertiserManager::sd_EnableAdvertisingSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("SetAdvertisingData", "ia{sv}", nullptr,
          GattLeAdvertiserManager::sd_SetAdvertisingData,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("SetScanResponseData", "ia{sv}", nullptr,
          GattLeAdvertiserManager::sd_SetScanResponseData,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("SetAdvertisingParameters", "ia{sv}", nullptr,
          GattLeAdvertiserManager::sd_SetAdvertisingParameters,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_PROPERTY("LeCodedPhySupported", "b",
          GattLeAdvertiserManager::sd_isLeCodedPhySupported, 0,
          SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
      SD_BUS_PROPERTY("Le2MPhySupported", "b",
          GattLeAdvertiserManager::sd_isLe2MPhySupported, 0,
          SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
      SD_BUS_PROPERTY("PeriodicAdvertisingSupported", "b",
          GattLeAdvertiserManager::sd_isLePeriodicAdvertisingSupported, 0,
          SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
      SD_BUS_PROPERTY("MaxAdvertisingDataLength", "i",
          GattLeAdvertiserManager::sd_getLeMaximumAdvertisingDataLength, 0,
          SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
      SD_BUS_PROPERTY("DeviceName", "s",
          GattLeAdvertiserManager::sd_getDeviceName, 0,
          SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
      SD_BUS_VTABLE_END};

  auto res = sd_bus_add_object_vtable(sdbus,
                                      &sdbusSlot_,
                                      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
                                      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
                                      vtable,
                                      this);
  if (res < 0) {
    ALOGI(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }
  res = sd_bus_emit_object_added(sdbus, GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH);
  if (res < 0) {
    ALOGI(LOGTAG
                   "object manager failed to signal new path %s: %d - %s\n",
                   GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH, -res, strerror(-res));
  }
  return true;
}

int GattLeAdvertiserManager::sd_StartAdvertisingSet(sd_bus_message *m,
                                                     void *userdata,
                                                     sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);

  // Get AdvertisingSet from message
  gatt::AdvertisingSetParameters *advSetParameters = nullptr;
  gatt::AdvertiseData *advertiseData = nullptr;
  gatt::AdvertiseData *scanResponse = nullptr;
  gatt::PeriodicAdvertiseParameters *periodicParameters = nullptr;
  gatt::AdvertiseData *periodicData = nullptr;
  int32_t duration;
  int32_t maxExtAdvEvents;

  int result = SdBus::getAdvertisingSetFromMessage(m, &advSetParameters, &advertiseData, &scanResponse,
                                               &periodicParameters, &periodicData, &duration, &maxExtAdvEvents);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse AdvertisingSet from message: %s\n", strerror(-result));
    return result;
  }

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->startAdvertisingSet(advSetParameters,
               advertiseData, scanResponse, periodicParameters, periodicData, duration, maxExtAdvEvents, sender);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_StopAdvertisingSet(sd_bus_message *m,
                                                    void *userdata,
                                                    sd_bus_error *ret_error) {
 ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->stopAdvertisingSet(sender);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_GetOwnAddress(sd_bus_message *m,
                                              void *userdata,
                                              sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  const int32_t  advertiserId;

  int result = sd_bus_message_read(m, "i", &advertiserId);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->getOwnAddress(advertiserId);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: advertiserId = %d \n", __func__, advertiserId);
  return sd_bus_reply_method_return(m, NULL);
}
/**/
int GattLeAdvertiserManager::sd_EnableAdvertisingSet(sd_bus_message *m,
                                                     void *userdata,
                                                     sd_bus_error *ret_error) {
 ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  const int32_t advertiserId;
  const bool enable;
  const int32_t duration;
  const int32_t maxExtAdvEvents;

  int result = sd_bus_message_read(m, "ibii", &advertiserId, &enable, &duration, &maxExtAdvEvents);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->enableAdvertisingSet(
                                                           advertiserId, enable, duration, maxExtAdvEvents);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_SetAdvertisingData(sd_bus_message *m,
                                                   void *userdata,
                                                   sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  const int32_t  advertiserId;
  gatt::AdvertiseData *advertiseData = nullptr;
  char type;
  const char *contents;

  int res = sd_bus_message_read(m, "i", &advertiserId);
  if (res < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-res));
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Let's check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // And now peek to check it is a dict entry
  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_DICT_ENTRY) {
    ALOGE(LOGTAG "%s -> Type not a Dict, it is:  %c", __func__, type);
    return -EINVAL;
  }
  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // Get the Dict key, it should be "AdvertiseData"
  const char *key;
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
  if (res < 0) {
    return res;
  }
  if (strcasecmp(key, "AdvertiseData") == 0) {
    res = SdBus::getAdvertiseDataFromMessage(m, &advertiseData);
    if (res < 0) {
      ALOGE(LOGTAG "Failed to parse AdvertiseData from message: %s\n", strerror(-res));
      return res;
    }
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->setAdvertisingData(advertiserId, advertiseData);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: advertiserId = %d \n", __func__, advertiserId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_SetScanResponseData(sd_bus_message *m,
                                                    void *userdata,
                                                    sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  const int32_t  advertiserId;
  gatt::AdvertiseData *scanResponse = nullptr;
  char type;
  const char *contents;

  int res = sd_bus_message_read(m, "i", &advertiserId);
  if (res < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-res));
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Let's check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // And now peek to check it is a dict entry
  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_DICT_ENTRY) {
    ALOGE(LOGTAG "%s -> Type not a Dict, it is:  %c", __func__, type);
    return -EINVAL;
  }
  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // Get the Dict key it should be "ScanResponse"
  const char *key;
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
  if (res < 0) {
    return res;
  }
  if (strcasecmp(key, "ScanResponse") == 0) {
    res = SdBus::getAdvertiseDataFromMessage(m, &scanResponse);
    if (res < 0) {
      ALOGE(LOGTAG "Failed to parse ScanResponse from message: %s\n", strerror(-res));
      return res;
    }
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->setScanResponseData(advertiserId, scanResponse);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: advertiserId = %d \n", __func__, advertiserId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_SetAdvertisingParameters(sd_bus_message *m,
                                                         void *userdata,
                                                         sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  const int32_t  advertiserId;
  gatt::AdvertisingSetParameters *advSetParameters = nullptr;
  char type;
  const char *contents;

  int res = sd_bus_message_read(m, "i", &advertiserId);
  if (res < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-res));
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }

  // Let's check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // And now peek to check it is a dict entry
  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_DICT_ENTRY) {
    ALOGE(LOGTAG "%s -> Type not a Dict, it is:  %c", __func__, type);
    return -EINVAL;
  }
  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // Get the Dict key, it should be "AdvertisingSetParameters"
  const char *key;
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
  if (res < 0) {
    return res;
  }
  if (strcasecmp(key, "AdvertisingSetParameters") == 0) {
    res = SdBus::getAdvertisingSetParametersFromMessage(m, &advSetParameters);
    if (res < 0) {
      ALOGE(LOGTAG "Failed to parse AdvertisingSetParameters from message: %s\n", strerror(-res));
      return res;
    }
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  advManager->gatt_dbus_service_->GetGattLibService()->setAdvertisingParameters(advertiserId, advSetParameters);
  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: advertiserId = %d \n", __func__, advertiserId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_SetPeriodicAdvertisingParameters(sd_bus_message *m,
                                                                 void *userdata,
                                                                 sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);

  return sd_bus_reply_method_return(m, NULL);
}
int GattLeAdvertiserManager::sd_SetPeriodicAdvertisingData(sd_bus_message *m,
                                                           void *userdata,
                                                           sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);

  return sd_bus_reply_method_return(m, NULL);
}
int GattLeAdvertiserManager::sd_SetPeriodicAdvertisingEnable(sd_bus_message *m,
                                                             void *userdata,
                                                             sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserManager::sd_isLeCodedPhySupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                        sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  return sd_bus_message_append(reply, TYPE_TO_STR(SD_BUS_TYPE_BOOLEAN),
                               advManager->gatt_dbus_service_->GetGattLibService()->isLeCodedPhySupported());

}

int GattLeAdvertiserManager::sd_isLe2MPhySupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  bool supported = advManager->gatt_dbus_service_->GetGattLibService()->isLe2MPhySupported();
  return sd_bus_message_append(reply, TYPE_TO_STR(SD_BUS_TYPE_BOOLEAN),
                               advManager->gatt_dbus_service_->GetGattLibService()->isLe2MPhySupported());
}

int GattLeAdvertiserManager::sd_isLePeriodicAdvertisingSupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                                   sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  return sd_bus_message_append(reply, TYPE_TO_STR(SD_BUS_TYPE_BOOLEAN),
                               advManager->gatt_dbus_service_->GetGattLibService()->isLePeriodicAdvertisingSupported());
}

int GattLeAdvertiserManager::sd_getLeMaximumAdvertisingDataLength(sd_bus *bus, const char *path, const char *interface, const char *property,
                                                    sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {

  ALOGI(LOGTAG " %s", __func__);
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  return sd_bus_message_append(reply, TYPE_TO_STR(SD_BUS_TYPE_INT32),
                               advManager->gatt_dbus_service_->GetGattLibService()->getLeMaximumAdvertisingDataLength());
}

int GattLeAdvertiserManager::sd_getDeviceName(sd_bus *bus, const char *path, const char *interface, const char *property,
                                sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);
  GattLeAdvertiserManager *advManager = static_cast<GattLeAdvertiserManager *>(userdata);
  return sd_bus_message_append_basic(reply, SD_BUS_TYPE_STRING,
                                     advManager->gatt_dbus_service_->GetGattLibService()->getDeviceName().c_str());
}

int GattLeAdvertiserManager::UpdateLocalLeFeaturesProperties() {
  ALOGI(LOGTAG " %s", __func__);
  return  sd_bus_emit_properties_changed(gatt_dbus_service_->GetSdbus(),
                                         GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
                                         GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
                                         "LeCodedPhySupported", "Le2MPhySupported", "PeriodicAdvertisingSupported",
                                         "MaxAdvertisingDataLength");
}

int GattLeAdvertiserManager::UpdateDeviceNameProperty() {
  ALOGI(LOGTAG " %s", __func__);
  return  sd_bus_emit_properties_changed(gatt_dbus_service_->GetSdbus(),
                                         GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
                                         GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
                                         "DeviceName");
}
} // namespace ipc
} // namespace gatt
