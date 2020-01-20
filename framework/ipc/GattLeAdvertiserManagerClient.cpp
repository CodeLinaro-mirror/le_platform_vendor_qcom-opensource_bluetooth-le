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

#include "GattLeAdvertiserManagerClient.hpp"
#include "GattDBusFw.hpp"
#include "AdvertisingSetParameters.hpp"
#include "PeriodicAdvertiseParameters.hpp"
#include "AdvertiseData.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeAdvertiserManagerClient "
namespace gatt {
namespace ipc {
GattLeAdvertiserManagerClient::GattLeAdvertiserManagerClient(GattDBusFw *dbus_service)
    :  gatt_dbus_service_(dbus_service) {
  ALOGD(LOGTAG " %s", __func__);
}

bool GattLeAdvertiserManagerClient::startAdvertisingSet(AdvertisingSetParameters *parameters,
        AdvertiseData *advertiseData, AdvertiseData *scanResponse,
        PeriodicAdvertiseParameters *periodicParameters, AdvertiseData *periodicData,
        int32_t duration, int32_t maxExtAdvEvents, gatt::IAdvertisingSetCallback* cb) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  gatt_dbus_service_->SetAdvertisingSetCallback(cb);

  sd_bus_message *m = nullptr;

  auto res = sd_bus_message_new_method_call(
      sdbus,
      &m,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "StartAdvertisingSet");

  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return false;
  }
  res = SdBus::PopulateAdvertisingSet(m, parameters, advertiseData, scanResponse,
                                      periodicParameters, periodicData, duration, maxExtAdvEvents);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with AdvertisingSet params");
    return false;
  }

  res = sd_bus_call_async(sdbus, nullptr, m, nullptr, nullptr, 0);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d: %s\n", -res, strerror(-res));
    return false;
  }

  if (nullptr != m) {
    sd_bus_message_unref(m);
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertiserManagerClient::stopAdvertisingSet(gatt::IAdvertisingSetCallback* cb) {
  // TODO: not using the *cb arg. In what scenario will this be needed at all?
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      sdbus,
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "StopAdvertisingSet",
      nullptr, nullptr, nullptr, nullptr);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

// I/F for AdvertisingSet
bool GattLeAdvertiserManagerClient::getOwnAddress(int32_t advertiserId) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      sdbus,
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "GetOwnAddress",
      nullptr, nullptr,
      "i",
      advertiserId);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}


bool GattLeAdvertiserManagerClient::enableAdvertisingSet(int32_t advertiserId,
                                                         bool enable,
                                                         int32_t duration,
                                                         int32_t maxExtAdvEvents) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      sdbus,
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "EnableAdvertisingSet",
      nullptr, nullptr,
      "ibii",
      advertiserId, enable, duration, maxExtAdvEvents);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertiserManagerClient::setAdvertisingData(int advertiserId,
                                                       AdvertiseData *advData) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  sd_bus_message *message = nullptr;

  auto res = sd_bus_message_new_method_call(
      sdbus,
      &message,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "SetAdvertisingData");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    goto done;
  }
  res = sd_bus_message_append(message, "i", advertiserId);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with advertiserId ");
    goto done;
  }

  // AdvertiseData
  if (advData != nullptr) {
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_append_basic(message, 's', "AdvertiseData");
    if (res < 0) {
      ALOGE(" Failed to append AdvertiseData: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = SdBus::PopulateLeAdvertiseData(message, advData);
    if (res < 0) {
      ALOGE(" Failed to PopulateLeAdvertiseData: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
  }
  res = sd_bus_call_async(sdbus, nullptr, message, nullptr, nullptr, 0);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d: %s\n", -res, strerror(-res));
  }
done:
  if (nullptr != message) {
    sd_bus_message_unref(message);
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return ((res < 0) ? false : true);
}


bool GattLeAdvertiserManagerClient::setScanResponseData(int32_t advertiserId,
                                                        AdvertiseData *scanResp) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  sd_bus_message *message = nullptr;

  auto res = sd_bus_message_new_method_call(
      sdbus,
      &message,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "SetScanResponseData");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    goto done;
  }
  res = sd_bus_message_append(message, "i", advertiserId);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with advertiserId ");
    goto done;
  }

  // ScanResponse
  if (scanResp != nullptr) {
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res)); 
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res)); 
      goto done;
    }
    res = sd_bus_message_append_basic(message, 's', "ScanResponse");
    if (res < 0) {
      ALOGE(" Failed to append ScanResponse: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = SdBus::PopulateLeAdvertiseData(message, scanResp);
    if (res < 0) {
      ALOGE(" Failed to PopulateLeAdvertiseData: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      goto done;
    }
  }
  res = sd_bus_call_async(sdbus, nullptr, message, nullptr, nullptr, 0);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d: %s\n", -res, strerror(-res));
  }

done:
  if (nullptr != message) {
    sd_bus_message_unref(message);
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return ((res < 0) ? false : true);
}

bool GattLeAdvertiserManagerClient::setAdvertisingParameters(int32_t advertiserId,
                                                             AdvertisingSetParameters *advSetParams) {
 ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  sd_bus_message *message = nullptr;

  auto res = sd_bus_message_new_method_call(
      sdbus,
      &message,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_MANAGER_INTERFACE,
      "SetAdvertisingParameters");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    goto done;
  }
  res = sd_bus_message_append(message, "i", advertiserId);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with advertiserId ");
    goto done;
  }

  // AdvertisingSetParameters
  if (advSetParams != nullptr) {
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_append_basic(message, 's', "AdvertisingSetParameters");
    if (res < 0) {
      ALOGE(" Failed to append AdvertisingSetParameters: %d - %s", res,
                    strerror(-res));
      goto done;
    }
    res = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (res < 0) {
      ALOGE(" Failed to open container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = SdBus::PopulateLeAdvertisingSetParameters(message, advSetParams);
    if (res < 0) {
      ALOGE(" Failed to PopulateLeAdvertisingSetParameters container: %d - %s",
                    res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
    res = sd_bus_message_close_container(message);
    if (res < 0) {
      ALOGE(" Failed to close container: %d - %s", res, strerror(-res));
      goto done;
    }
  }
  res = sd_bus_call_async(sdbus, nullptr, message, nullptr, nullptr, 0);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d: %s\n", -res, strerror(-res));
  }

done:
  if (nullptr != message) {
    sd_bus_message_unref(message);
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return ((res < 0) ? false : true);
}

bool GattLeAdvertiserManagerClient::isLeCodedPhySupported() {
  ALOGD(LOGTAG " %s leCodedPhySupported %d", __func__, leCodedPhySupported_);
  return leCodedPhySupported_;
}

bool GattLeAdvertiserManagerClient::isLe2MPhySupported() {
  ALOGD(LOGTAG " %s le2MPhySupported %d", __func__, le2MPhySupported_);
  return le2MPhySupported_;
}

bool GattLeAdvertiserManagerClient::isLePeriodicAdvertisingSupported() {
  ALOGD(LOGTAG " %s LePeriodicAdvertisingSupported %d", __func__, lePeriodicAdvertisingSupported_);
  return lePeriodicAdvertisingSupported_;
}

int GattLeAdvertiserManagerClient::getLeMaximumAdvertisingDataLength() {
  ALOGD(LOGTAG " %s MaxAdvertisingDataLength %d\n",__func__, maxAdvertisingDataLength_);
  return 300;
  // return maxAdvertisingDataLength_;
}

string GattLeAdvertiserManagerClient::getDeviceName() {
  ALOGD(LOGTAG " %s DeviceName %s", __func__, deviceName_);
  return deviceName_;
}

int GattLeAdvertiserManagerClient::getLEParameters() {
  ALOGD(LOGTAG " %s", __func__);
  auto sdbus = gatt_dbus_service_->GetSdbus();
  int r = sd_bus_call_method_async(sdbus, NULL,
                GATT_MANAGER_SERVICE_NAME, GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH,
                "org.freedesktop.DBus.Properties", "GetAll",
                GattLeAdvertiserManagerClient::leParamsProperty_cb, this,
                "s", GATT_SERVER_ADVERTISER_MANAGER_INTERFACE);

  // Register for future update notifications
  std::string advPath(GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH);
  std::string changedMatch =
    "type='signal',path='" + advPath + "',"
    "interface='org.freedesktop.DBus.Properties',"
    "member='PropertiesChanged'";

  int res = sd_bus_add_match(sdbus, nullptr,
           changedMatch.c_str(),
           GattLeAdvertiserManagerClient::leParamsProperty_cb,
           nullptr);

  if (r < 0) {
    ALOGE(LOGTAG "Error calling GetAll");
    return r;
  } else if (res < 0) {
    ALOGE(LOGTAG "Error setting PropertiesChanged match");
    return res;
  }
  return 0;
}

int GattLeAdvertiserManagerClient::leParamsProperty_cb(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGD(LOGTAG " %s", __func__);
  GattLeAdvertiserManagerClient *advMgrClient = static_cast<GattLeAdvertiserManagerClient *>(userdata);

  if (advMgrClient == nullptr)
        return 0;

  const char *contents = NULL;
  const char *interface = NULL;
  char type;

  int r = sd_bus_message_read_basic(m, 's', &interface);

  r = sd_bus_message_peek_type(m, &type, &contents);

  if (SD_BUS_TYPE_ARRAY == type) {
    r = sd_bus_message_enter_container(m, type, contents);
    if (r < 0) {
      ALOGE(LOGTAG "array container parsing err: %s", strerror(-r));
      return 0;
    }
    for (;;) {
      r = sd_bus_message_peek_type(m, &type, &contents);
      if (r < 0) {
        ALOGE(LOGTAG "peek type parsing err: %s", strerror(-r));
        break;
      }
      r = sd_bus_message_enter_container(m, type, contents);
      if (r < 0) {
        ALOGE(LOGTAG "dict container parsing err: %s", strerror(-r));
            break;
      }
      r = sd_bus_message_read_basic(m, 's', &contents);
      if (r < 0) {
        ALOGE(LOGTAG "error parsing property name err: %s", strerror(-r));
        break;
      }
      if (r == 0)
        break;

      r = advMgrClient->parseLeAdvManagerProperties(m, contents);
      if (r < 0) {
        ALOGE(LOGTAG "Error parsing property value: %s", strerror(-r));
        return r;
      }
      r = sd_bus_message_exit_container(m);
    }
    r = sd_bus_message_exit_container(m);
  }
  return r;
}

template <typename T>
int GattLeAdvertiserManagerClient::parseProperties(sd_bus_message *m, const char *type, T *value) {
    int r = 0;
    do {
        r = sd_bus_message_enter_container(m, 'v', type);
        if (r < 0) {
            ALOGE(LOGTAG "Failed to enter container message:%s",
                strerror(-r));
            break;
        }

        r = sd_bus_message_read(m, type, value);
        if (r < 0) {
            ALOGE(LOGTAG "Failed to parse response message:%s",
                strerror(-r));
            break;
        }

        r = sd_bus_message_exit_container(m);
        if (r < 0) {
            ALOGE(LOGTAG "Failed to exit container message:%s",
                strerror(-r));
            break;
        }
    } while (false);
    return r;
}

int GattLeAdvertiserManagerClient::parseLeAdvManagerProperties(sd_bus_message *m, const char *property) {
    ALOGD(LOGTAG "%s Property: %s", __func__, property);

    std::lock_guard<std::mutex> advMgr_prop_lock(advMgr_prop_mtx_);

    std::string name(property);
    int r = 0;
    int b = 0;

    if (name.compare("DeviceName") == 0) {
        r = parseProperties<const char *>(m, "s", &deviceName_);
        if (r < 0) {
            ALOGE(LOGTAG "Error parsing property %s err: %s", name.c_str(), strerror(-r));
        }
        ALOGD(LOGTAG "DeviceName: %s", deviceName_);
    } else if (name.compare("LeCodedPhySupported") == 0) {
        r = parseProperties<int>(m, "b", &b);
        if (r < 0) {
            ALOGE(LOGTAG "Error parsing property %s err: %s", name.c_str(), strerror(-r));
        }
        leCodedPhySupported_ = (b!=0);
        ALOGD(LOGTAG "LeCodedPhySupported: %s", leCodedPhySupported_ ? "true" : "false");
    } else if (name.compare("Le2MPhySupported") == 0) {
        r = parseProperties<int>(m, "b", &b);
        if (r < 0) {
            ALOGE(LOGTAG "Error parsing property %s err: %s", name.c_str(), strerror(-r));
        }
        le2MPhySupported_ = (b!=0);
        ALOGD(LOGTAG "Le2MPhySupported: %s", le2MPhySupported_ ? "true" : "false");
    } else if (name.compare("PeriodicAdvertisingSupported") == 0) {
        r = parseProperties<int>(m, "b", &b);
        if (r < 0) {
            ALOGE(LOGTAG "Error parsing property %s err: %s", name.c_str(), strerror(-r));
        }
        lePeriodicAdvertisingSupported_ = (b!=0);
        ALOGD(LOGTAG "PeriodicAdvertisingSupported: %s", lePeriodicAdvertisingSupported_ ? "true" : "false");
    } else if (name.compare("MaxAdvertisingDataLength") == 0) {
        r = parseProperties<int32_t>(m, "i", &maxAdvertisingDataLength_);
        if (r < 0) {
            ALOGE(LOGTAG "Error parsing property %s err: %s", name.c_str(), strerror(-r));
        }
        ALOGD(LOGTAG "MaxAdvertisingDataLength: %d", maxAdvertisingDataLength_);
    } else {
        r = sd_bus_message_skip(m, "v");
    }
    return r;
}

bool GattLeAdvertiserManagerClient::setPeriodicAdvertisingParameters(int32_t advertiserId,
                                      PeriodicAdvertiseParameters *parameters) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}

bool GattLeAdvertiserManagerClient::setPeriodicAdvertisingData(int32_t advertiserId,
                                      AdvertiseData *data) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
}

bool GattLeAdvertiserManagerClient::setPeriodicAdvertisingEnable(int32_t advertiserId,
                                      bool enable) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}

bool GattLeAdvertiserManagerClient::registerSync(gatt::ScanResult *scanResult,
                                      int32_t skip,
                                      int32_t timeout,
                                      gatt::IPeriodicAdvertisingCallback* cb) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}

bool GattLeAdvertiserManagerClient::unregisterSync(gatt::IPeriodicAdvertisingCallback* cb) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}
} // namespace ipc
} // namespace gatt
