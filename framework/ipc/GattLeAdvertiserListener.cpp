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

#include "GattLeAdvertiserListener.hpp"
#include "GattDBusFw.hpp"

#define LOGTAG "GattLeAdvertiserListener "
namespace gatt {
namespace ipc {
GattLeAdvertiserListener::GattLeAdvertiserListener(GattDBusFw *dbus_service)
    : gatt_dbus_service_(dbus_service) {
  ALOGD(LOGTAG " %s", __func__);
}

GattLeAdvertiserListener::~GattLeAdvertiserListener() {
  sd_bus_slot_unref(sdbusSlot_);
  sdbusSlot_ = nullptr;
}

bool GattLeAdvertiserListener::InitialiseDBusService() {
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (!sdbus) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  ALOGD(LOGTAG " %s", __func__);
	  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("AdvertisingSetStarted", "iii", nullptr,
          GattLeAdvertiserListener::sd_AdvertisingSetStarted,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("AdvertisingSetStopped", "i", nullptr,
          GattLeAdvertiserListener::sd_AdvertisingSetStopped,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("OwnAddressRead", "iis", nullptr,
          GattLeAdvertiserListener::sd_OwnAddressRead,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("AdvertisingEnabled", "ibi", nullptr,
          GattLeAdvertiserListener::sd_AdvertisingEnabled,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("AdvertisingDataSet", "ii", nullptr,
          GattLeAdvertiserListener::sd_AdvertisingDataSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("ScanResponseDataSet", "ii", nullptr,
          GattLeAdvertiserListener::sd_ScanResponseDataSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("AdvertisingParametersUpdated", "iii", nullptr,
          GattLeAdvertiserListener::sd_AdvertisingParametersUpdated,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("PeriodicAdvertisingParametersUpdated", "iii", nullptr,
          GattLeAdvertiserListener::sd_PeriodicAdvertisingParametersUpdated,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("PeriodicAdvertisingDataSet", "ii", nullptr,
          GattLeAdvertiserListener::sd_PeriodicAdvertisingDataSet,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("PeriodicAdvertisingEnabled", "ibi", nullptr,
          GattLeAdvertiserListener::sd_PeriodicAdvertisingEnabled,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_VTABLE_END};

  auto res = sd_bus_add_object_vtable(sdbus,
                                      &sdbusSlot_,
                                      BLE_LISTENER_OBJECT_PATH,
                                      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
                                      vtable,
                                      this);
  if (res < 0) {
    ALOGI(LOGTAG "interface init failed on path %s: %d - %s\n",
        BLE_LISTENER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  res = sd_bus_emit_object_added(sdbus, BLE_LISTENER_OBJECT_PATH);
  if (res < 0) {
    ALOGI(LOGTAG
                   "object manager failed to signal new path %s: %d - %s\n",
                   BLE_LISTENER_OBJECT_PATH, -res, strerror(-res));
  }
  return true;
}

int GattLeAdvertiserListener::sd_AdvertisingSetStarted(sd_bus_message *m,
                                                        void *userdata,
                                                        sd_bus_error *ret_error) {
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;
  const int32_t tx_power;
  const int32_t status;

  int result = sd_bus_message_read(m, "iii", &advertiserId, &tx_power, &status);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onAdvertisingSetStarted(advertiserId, tx_power, status);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %d\n", __func__, path, advertiserId, tx_power, status );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_AdvertisingSetStopped(sd_bus_message *m,
                                                        void *userdata,
                                                        sd_bus_error *ret_error) {
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;

  int result = sd_bus_message_read(m, "i", &advertiserId);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onAdvertisingSetStopped(advertiserId);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %d\n", __func__, path, advertiserId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_OwnAddressRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;
  const int32_t addressType;
  const std::string devAddress;

  int result = sd_bus_message_read(m, "iis", &advertiserId, &addressType, &devAddress);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onOwnAddressRead(advertiserId, addressType, devAddress);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %s\n", __func__, path, advertiserId, addressType, devAddress.c_str());
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_AdvertisingEnabled(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;
  const bool enable;
  const int32_t status;

  int result = sd_bus_message_read(m, "ibi", &advertiserId, &enable, &status);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onAdvertisingEnabled(advertiserId, enable, status);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %d\n", __func__, path, advertiserId, enable, status);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_AdvertisingDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error){
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;
  const int32_t status;

  int result = sd_bus_message_read(m, "ii", &advertiserId, &status);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onAdvertisingDataSet(advertiserId, status);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %dn", __func__, path, advertiserId, status);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_ScanResponseDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t advertiserId;
  const int32_t status;

  int result = sd_bus_message_read(m, "ii", &advertiserId, &status);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onScanResponseDataSet(advertiserId, status);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d\n", __func__, path, advertiserId, status);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_AdvertisingParametersUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t  advertiserId;
  const int32_t  tx_power;
  const int32_t  status;

  int result = sd_bus_message_read(m, "iii", &advertiserId, &tx_power, &status);
  if (result < 0) {
    ALOGI(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeAdvertiserListener *advListener = static_cast<GattLeAdvertiserListener *>(userdata);
  advListener->gatt_dbus_service_->advertisingSetCallback()->onAdvertisingParametersUpdated(advertiserId, tx_power, status);

  ALOGI(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %d\n", __func__, path, advertiserId, tx_power, status );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_PeriodicAdvertisingParametersUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG " %s > Not yet supported", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_PeriodicAdvertisingDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG " %s > Not yet supported", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeAdvertiserListener::sd_PeriodicAdvertisingEnabled(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG " %s > Not yet supported", __func__);
  return sd_bus_reply_method_return(m, NULL);
}
} // namespace ipc
} // namespace gatt
