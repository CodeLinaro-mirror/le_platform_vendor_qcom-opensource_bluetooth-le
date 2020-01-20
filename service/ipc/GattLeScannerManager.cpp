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

#include "GattLeScannerManager.hpp"
#include "GattDBusService.hpp"
#include "BluetoothLeSdbus.hpp"
#include "ScanSettings.hpp"
#include "ScanFilter.hpp"
#include "ResultStorageDescriptor.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeScannerManager"

namespace gatt {
namespace ipc {
GattLeScannerManager::GattLeScannerManager(GattDBusService *dbus_service)
    : gatt_dbus_service_(dbus_service) {
  ALOGI(LOGTAG " %s", __func__);
}

GattLeScannerManager::~GattLeScannerManager() {
  ALOGI(LOGTAG " %s", __func__);
  sd_bus_slot_unref(sdbusSlot_);
  sdbusSlot_ = nullptr;
}

bool GattLeScannerManager::InitialiseDBusService() {
  ALOGI(LOGTAG " %s", __func__);
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (!sdbus) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("RegisterScanner", nullptr, nullptr,
          GattLeScannerManager::sd_RegisterScanner,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("UnregisterScanner", "i", nullptr,
          GattLeScannerManager::sd_UnregisterScanner,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("StartScan", "ia{sv}", nullptr,
          GattLeScannerManager::sd_StartScan,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("StopScanByID", "i", nullptr,
          GattLeScannerManager::sd_StopScanByID,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("StopScanByClient", "a{sv}", nullptr,
          GattLeScannerManager::sd_StopScanByClient,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("FlushPendingBatchResults", "i", nullptr,
          GattLeScannerManager::sd_FlushPendingBatchResults,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("DisconnectAll", nullptr, nullptr,
          GattLeScannerManager::sd_DisconnectAll,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_VTABLE_END};

  auto res = sd_bus_add_object_vtable(sdbus,
                                      &sdbusSlot_,
                                      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
                                      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
                                      vtable,
                                      this);
  if (res < 0) {
    ALOGE(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  res = sd_bus_emit_object_added(sdbus, GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH);
  if (res < 0) {
    ALOGI(LOGTAG "object manager failed to signal new path %s: %d - %s\n",
                   GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH, -res, strerror(-res));
  }
  return true;
}

int GattLeScannerManager::sd_RegisterScanner(sd_bus_message *m,
                                             void *userdata,
                                             sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);
  GattLeScannerManager *scannerManager = static_cast<GattLeScannerManager *>(userdata);
  scannerManager->gatt_dbus_service_->GetGattLibService()->registerScanner(sender);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s\n", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_UnregisterScanner(sd_bus_message *m,
                                               void *userdata,
                                               sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const int32_t scannerId;

  int result = sd_bus_message_read(m, "i", &scannerId);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeScannerManager *scannerManager = static_cast<GattLeScannerManager *>(userdata);
  scannerManager->gatt_dbus_service_->GetGattLibService()->unregisterScanner(scannerId);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_StartScan(sd_bus_message *m,
                                       void *userdata,
                                       sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const int32_t scannerId;
  int result = sd_bus_message_read(m, "i", &scannerId);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  ScanSettings *settings = nullptr;
  std::vector<ScanFilter*> filters;
  std::vector<std::vector<ResultStorageDescriptor*>> storages;

  GattLeScannerManager *scannerManager = static_cast<GattLeScannerManager *>(userdata);
  result = scannerManager->getStartScanParamsFromMessage(m, &settings, &filters, &storages);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse StartScan params from message: %s\n", strerror(-result));
    return result;
  }

  // NOTE: ResultStorageDescriptor is not yet supported, so we'll be creating an empty instance for now
  scannerManager->gatt_dbus_service_->GetGattLibService()->startScan(scannerId, settings, filters, std::vector<std::vector<ResultStorageDescriptor*>>());
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s: scannerId = %d \n", __func__, scannerId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_StopScanByID(sd_bus_message *m,
                                          void *userdata,
                                          sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const int32_t scannerId;

  int result = sd_bus_message_read(m, "i", &scannerId);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }
  GattLeScannerManager *scannerManager = static_cast<GattLeScannerManager *>(userdata);
  scannerManager->gatt_dbus_service_->GetGattLibService()->stopScan(scannerId);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s: scannerId = %d \n", __func__, scannerId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_StopScanByClient(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n   > Not yet supported %s", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_FlushPendingBatchResults(sd_bus_message *m,
                                                      void *userdata,
                                                      sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const int32_t scannerId;

  int result = sd_bus_message_read(m, "i", &scannerId);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }
  GattLeScannerManager *scannerManager = static_cast<GattLeScannerManager *>(userdata);
  scannerManager->gatt_dbus_service_->GetGattLibService()->flushPendingBatchResults(scannerId);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s: scannerId = %d \n", __func__, scannerId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerManager::sd_DisconnectAll(sd_bus_message *m,
                                           void *userdata,
                                           sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);

  GattLeScannerManager *scanManager = static_cast<GattLeScannerManager *>(userdata);
  scanManager->gatt_dbus_service_->GetGattLibService()->disconnectAll();
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);;
}

int GattLeScannerManager::sd_isScanClient(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error) {
  ALOGW(LOGTAG "\n   > Not yet supported %s", __func__);
  return -1;
}

int GattLeScannerManager::getScanSettingsFromMessage(sd_bus_message *m, ScanSettings **scanSettings) {
  ALOGI(LOGTAG " %s", __func__);
  int32_t scanMode; // TODO: initiaize params to default values ?
  int32_t callbackType;
  int32_t scanResultType;
  int32_t reportDelayMillis;
  int32_t matchMode;
  int32_t numOfMatchesPerFilter;
  bool legacy;
  int32_t phy;

  char type;
  const char *contents;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }

    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not dict. it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }
    if (strcasecmp(key, "ScanMode") == 0) {
      res = SdBus::getInt32FromMessage(m, &scanMode);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "CallbackType") == 0) {
      res = SdBus::getInt32FromMessage(m, &callbackType);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ScanResultType") == 0) {
      res = SdBus::getInt32FromMessage(m, &scanResultType);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ReportDelayMillis") == 0) {
      res = SdBus::getInt32FromMessage(m, &reportDelayMillis);
     if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "MatchMode") == 0) {
      res = SdBus::getInt32FromMessage(m, &matchMode);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "NumOfMatchesPerFilter") == 0) {
      res = SdBus::getInt32FromMessage(m, &numOfMatchesPerFilter);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Legacy") == 0) {
      res = SdBus::getBoolFromMessage(m, &legacy);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Phy") == 0) {
      res = SdBus::getInt32FromMessage(m, &phy);
          if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
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

  *scanSettings = gatt::ScanSettings::Builder()
                          .setScanMode(scanMode)
                          .setCallbackType(callbackType)
                          .setScanResultType(scanResultType)
                          .setReportDelay(reportDelayMillis)
                          .setNumOfMatches(matchMode)
                          .setNumOfMatches(numOfMatchesPerFilter)
                          .setLegacy(legacy)
                          .setPhy(phy)
                          .build();
  return res;
}

int GattLeScannerManager::getScanFiltersFromMessage(sd_bus_message *m,
                                                    std::vector<ScanFilter*> *filters) {
  ALOGI(LOGTAG " %s", __func__);
  char type;
  const char *contents;

  string deviceName = "";
  string deviceAddress = "";
  Uuid serviceUuid = Uuid::kEmpty;
  Uuid serviceUuidMask = Uuid::kEmpty;
  Uuid serviceSolicitationUuid = Uuid::kEmpty;
  Uuid serviceSolicitationUuidMask = Uuid::kEmpty;
  Uuid serviceDataUuid = Uuid::kEmpty;
  std::vector<uint8_t> serviceData;
  std::vector<uint8_t> serviceDataMask;
  int manufacturerId = -1;
  std::vector<uint8_t> manufacturerData;
  std::vector<uint8_t> manufacturerDataMask;
  uint8_t *data = nullptr;
  size_t array_size;


  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG " %s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "aa{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG " %s -> Type not array", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each array entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check that we have an array of dicts "aa{sv}"
    if (type != SD_BUS_TYPE_ARRAY) {
      ALOGE(LOGTAG "%s -> Type not array", __func__);

      return -EINVAL;
    }
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    auto fltrBldr = gatt::ScanFilter::Builder();
    // Each dict entry is a a filter
    for (;;) {
      res = sd_bus_message_peek_type(m, &type, &contents);
      if (res < 0) {
        return res;
      }
      // Break out of loop if end of array
      if (res == 0) {
        break;
      }

      // Check if it is a dict entry
      if (type != SD_BUS_TYPE_DICT_ENTRY) {
        ALOGE(LOGTAG "%s -> Type not dict. it is:  %c", __func__, type);
        return -EINVAL;
      }
      // Open Dict Entry
      res = sd_bus_message_enter_container(m, type, contents);
      if (res < 0) {
        return res;
      }

      // Get the Dict key
      const char *key;
      bool isValid;
      string uuidString;
      res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
      if (res < 0) {
        return res;
      }
      ALOGE(LOGTAG " %s", key);
      if (strcasecmp(key, "DeviceName") == 0) {
        res = SdBus::getStringFromMessage(m, &deviceName);
        if (res < 0) {
          return res;
        }

        fltrBldr.setDeviceName(deviceName);
      } else if (strcasecmp(key, "DeviceAddress") == 0) {
        res = SdBus::getStringFromMessage(m, &deviceAddress);
        if (res < 0) {
          return res;
        }
        fltrBldr.setDeviceAddress(deviceAddress);
      } else if (strcasecmp(key, "ServiceUuid") == 0) {
        res = SdBus::getStringFromMessage(m, &uuidString);
        if (res < 0) {
          return res;
        }
        serviceUuid = Uuid::FromString(uuidString, &isValid);
        if (!isValid) {
          return -EINVAL;
        }
      } else if (strcasecmp(key, "ServiceUuidMask") == 0) {
        res = SdBus::getStringFromMessage(m, &uuidString);
        if (res < 0) {
          return res;
        }
        serviceUuidMask = Uuid::FromString(uuidString, &isValid);
        if (!isValid) {
          return -EINVAL;
        }
      } else if (strcasecmp(key, "ServiceSolicitationUuid") == 0) {
        res = SdBus::getStringFromMessage(m, &uuidString);
        if (res < 0) {
          return res;
        }
        serviceSolicitationUuid = Uuid::FromString(uuidString, &isValid);
        if (!isValid) {
          return -EINVAL;
        }
      } else if (strcasecmp(key, "ServiceSolicitationUuidMask") == 0) {
        res = SdBus::getStringFromMessage(m, &uuidString);
        if (res < 0) {
          return res;
        }
        serviceSolicitationUuidMask = Uuid::FromString(uuidString, &isValid);
        if (!isValid) {
          return -EINVAL;
        }
      } else if (strcasecmp(key, "ServiceDataUuid") == 0) {
        res = SdBus::getStringFromMessage(m, &uuidString);
        if (res < 0) {
          return res;
        }
        serviceDataUuid = Uuid::FromString(uuidString, &isValid);
        if (!isValid) {
          return -EINVAL;
        }
      } else if (strcasecmp(key, "ServiceData") == 0) {
        res = SdBus::getByteArrayFromMessage(m, &data, &array_size);
        if (res < 0) {
          return res;
        }
        serviceData.insert(serviceData.begin(), data, data + array_size);
      } else if (strcasecmp(key, "ServiceDataMask") == 0) {
        res = SdBus::getByteArrayFromMessage(m, &data, &array_size);
        if (res < 0) {
          return res;
        }
        serviceDataMask.insert(serviceDataMask.begin(), data, data + array_size);
      } else if (strcasecmp(key, "ManufacturerId") == 0) {
        res = SdBus::getInt32FromMessage(m, &manufacturerId);
        if (res < 0) {
          return res;
        }
      } else if (strcasecmp(key, "ManufacturerData") == 0) {
        res = SdBus::getByteArrayFromMessage(m, &data, &array_size);
        if (res < 0) {
          return res;
        }
        manufacturerData.insert(manufacturerData.begin(), data, data + array_size);
      } else if (strcasecmp(key, "ManufacturerDataMask") == 0) {
        res = SdBus::getByteArrayFromMessage(m, &data, &array_size);
        if (res < 0) {
          return res;
        }
        manufacturerDataMask.insert(manufacturerDataMask.begin(), data, data + array_size);
      } else {
        res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
        if (res < 0) {
          return res;
        }
      }
      res = sd_bus_message_exit_container(m);
      if (res < 0) {
        return res;
      }
    }
    if (serviceUuidMask != Uuid::kEmpty) {
      if (serviceUuid != Uuid::kEmpty ) {
        fltrBldr.setServiceUuid(serviceUuid, serviceUuidMask);
      }
      else {
        fltrBldr.setServiceUuid(serviceUuid);
      }
    }
    if (serviceSolicitationUuid != Uuid::kEmpty) {
      if (serviceSolicitationUuidMask != Uuid::kEmpty ) {
        fltrBldr.setServiceSolicitationUuid(serviceSolicitationUuid, serviceSolicitationUuidMask);
      }
      else {
        fltrBldr.setServiceSolicitationUuid(serviceSolicitationUuid);
      }
    }
    if (serviceDataUuid != Uuid::kEmpty) {
      if (!serviceDataMask.empty()) {
        fltrBldr.setServiceData(serviceDataUuid, serviceData);
      }
      else {
        fltrBldr.setServiceData(serviceDataUuid, serviceData, serviceDataMask);
      }
    }
    if (manufacturerId != -1) {
      if (!manufacturerDataMask.empty()) {
        fltrBldr.setManufacturerData(manufacturerId, manufacturerData);
      }
      else {
        fltrBldr.setManufacturerData(manufacturerId, manufacturerData, manufacturerDataMask);
      }
    }

    (*filters).push_back(fltrBldr.build());

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
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
  return res;
}


int GattLeScannerManager::getStartScanParamsFromMessage(sd_bus_message *m, ScanSettings **scanSettings,
                                      std::vector<ScanFilter*> *scanFilters,
                                      std::vector<std::vector<ResultStorageDescriptor*>> *storagesDesc) {
  ALOGI(LOGTAG " %s", __func__);
  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }
    if (strcasecmp(key, "StartScanSettings") == 0) {
      int res = sd_bus_message_peek_type(m, &type, &contents);
      if (res < 0) {
        return res;
      }
      if (type != SD_BUS_TYPE_VARIANT) {
        ALOGE(LOGTAG "%s -> Type not variant", __func__);
        return -EINVAL;
      }

      res = sd_bus_message_enter_container(m, type, contents);
      if (res < 0) {
        return res;
      }

      res = sd_bus_message_peek_type(m, &type, &contents);
      if (res < 0) {
        return res;
      }
      // Check that we have an array of dicts "a{sv}"
      if (type != SD_BUS_TYPE_ARRAY) {
        ALOGE(LOGTAG "%s -> Type not array", __func__);

        return -EINVAL;
      }

      res = sd_bus_message_enter_container(m, type, contents);
      if (res < 0) {
        return res;
      }
      // For each dict entry
      for (;;) {
        res = sd_bus_message_peek_type(m, &type, &contents);
        if (res < 0) {
          return res;
        }
        // Break out of loop if end of array
        if (res == 0) {
          break;
        }
        // Check if it is a dict entry
        if (type != SD_BUS_TYPE_DICT_ENTRY) {
          ALOGE(LOGTAG "%s -> Type not array. it is:  %c", __func__, type);
          return -EINVAL;
        }

        // Open Dict Entry
        res = sd_bus_message_enter_container(m, type, contents);
        if (res < 0) {
          return res;
        }

        // Get the Dict key
        const char *key;
        res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
        if (res < 0) {
          return res;
        }

        if (strcasecmp(key, "ScanSettings") == 0) {
          res = getScanSettingsFromMessage(m, scanSettings);
          if (res < 0) {
            return res;
          }
        } else if (strcasecmp(key, "ScanFilters") == 0) {
          res = getScanFiltersFromMessage(m, scanFilters);
          if (res < 0) {
            return res;
          }
        }  else if (strcasecmp(key, "ScanResultsStorageDesc") == 0) {
          ALOGW(LOGTAG " ScanResultsStorageDesc Not yet supported !");
        //   res = getScanResultsStorageDescFromMessage(m, storagesDesc);
        //   if (res < 0) {
        //     return res;
        //   }
        } else {
          res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
          if (res < 0) {
            return res;
          }
        }

        res = sd_bus_message_exit_container(m);
        if (res < 0) {
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
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return res;
}
} // namespace ipc
} // namespace gatt
