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

#include "GattLeScannerManagerClient.hpp"
#include "GattDBusFw.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeScannerManagerClient "

namespace gatt {
namespace ipc {
GattLeScannerManagerClient::GattLeScannerManagerClient(GattDBusFw *dbus_service)
    :  gatt_dbus_service_(dbus_service) {
  ALOGI(LOGTAG " %s", __func__);
}

bool GattLeScannerManagerClient::registerScanner(IScannerCallback *callback) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  gatt_dbus_service_->SetScannerCallback(callback);

  auto res = sd_bus_call_method_async(
      sdbus,
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "RegisterScanner",
      nullptr, nullptr, nullptr, nullptr);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScannerManagerClient::unregisterScanner(int scannerId) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s with scannerId = %d", __func__, scannerId);
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
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "UnregisterScanner",
      nullptr,
      nullptr,
      "i",
      scannerId);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScannerManagerClient::startScan(int scannerId,
                                           ScanSettings *settings,
                                           std::vector<ScanFilter*> filters,
                                           std::vector<std::vector<ResultStorageDescriptor*>> storages) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr) {
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  sd_bus_message *m = nullptr;
  auto res = sd_bus_message_new_method_call(
      sdbus,
      &m,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "StartScan");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return false;
  }

  res = sd_bus_message_append(m, "i", scannerId);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with advertiserId ");
    return false;
  }

  res = GattLeScannerManagerClient::populateStartScan(m, settings, &filters, &storages);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with StartScan params");
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

bool GattLeScannerManagerClient::stopScan(int scannerId) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
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
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "StopScanByID",
      nullptr, nullptr,
      "i",
      scannerId);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScannerManagerClient::flushPendingBatchResults(int scannerId) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
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
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "FlushPendingBatchResults",
      nullptr, nullptr,
      "i",
      scannerId);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScannerManagerClient::disconnectAll() {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
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
      GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_MANAGER_INTERFACE,
      "DisconnectAll",
      nullptr, nullptr, nullptr, nullptr);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScannerManagerClient::isScanClient(int clientIf) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return false;
}

bool GattLeScannerManagerClient::isOffloadedFilteringSupported() {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return false;
}

int GattLeScannerManagerClient::numHwTrackFiltersAvailable() {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return 1;
}

int GattLeScannerManagerClient::populateScanSettings(sd_bus_message *message,
                                                   ScanSettings *scanSettings) {
  ALOGI(LOGTAG " %s", __func__);
  if (scanSettings == nullptr) {
    ALOGE(LOGTAG " %s - > populateScanSettings is not valid", __func__);
    return -1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "ScanMode", (int32_t)(scanSettings->getScanMode()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "CallbackType", (int32_t)(scanSettings->getCallbackType()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "ScanResultType", (int32_t)(scanSettings->getScanResultType()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "ReportDelayMillis", (int32_t)(scanSettings->getReportDelayMillis()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "MatchMode", (int32_t)(scanSettings->getMatchMode()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "NumOfMatchesPerFilter", (int32_t)(scanSettings->getNumOfMatches()));
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "Legacy", scanSettings->getLegacy());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "Phy", (int32_t)(scanSettings->getPhy()));
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }

  return r;
}


int GattLeScannerManagerClient::populateScanFilters(sd_bus_message *message,
                                                    std::vector<ScanFilter*> *filters) {
  ALOGI(LOGTAG " %s", __func__);
  if (filters == nullptr) {
    ALOGE(LOGTAG " %s - > populateScanFilters is not valid", __func__);
    return -1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "a{sv}");
  if (r < 0) {
    return r;
  }

  for (auto filter : *filters) {
    if (filter == nullptr) {
      ALOGE(LOGTAG " %s - > Scan filter is not valid", __func__);
      return -1;
    }
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
    if (r < 0) {
      return r;
    }
    r = SdBus::AppendDictEntry(message, "DeviceName", filter->getDeviceName());
    if (r < 0) {
      return r;
    }
    r = SdBus::AppendDictEntry(message, "DeviceAddress", filter->getDeviceAddress());
    if (r < 0) {
      return r;
    }

    auto tmpUuid = filter->getServiceUuid();
    if (tmpUuid != Uuid::kEmpty) {
      r = SdBus::AppendDictEntry(message, "ServiceUuid", tmpUuid.ToString());
      if (r < 0) {
        return r;
      }
      tmpUuid = filter->getServiceUuidMask();
      if (tmpUuid != Uuid::kEmpty) {
        r = SdBus::AppendDictEntry(message, "ServiceUuidMask", tmpUuid.ToString());
        if (r < 0) {
          return r;
        }
      }
    }
    tmpUuid = filter->getServiceSolicitationUuid();
    if (tmpUuid != Uuid::kEmpty) {
      r = SdBus::AppendDictEntry(message, "ServiceSolicitationUuid", tmpUuid.ToString());
      if (r < 0) {
        return r;
      }
      tmpUuid = filter->getServiceSolicitationUuidMask();
      if (tmpUuid != Uuid::kEmpty) {
        r = SdBus::AppendDictEntry(message, "ServiceSolicitationUuidMask", tmpUuid.ToString());
        if (r < 0) {
          return r;
        }
      }
    }
    tmpUuid = filter->getServiceDataUuid();
    if (tmpUuid != Uuid::kEmpty) {
      r = SdBus::AppendDictEntry(message, "ServiceDataUuid", tmpUuid.ToString());
      if (r < 0) {
        return r;
      }
      auto data = filter->getServiceData();
      if (!data.empty()) {
        r = SdBus::AppendDictEntry(message, "ServiceData", data.data());
        if (r < 0) {
          return r;
        }
      }
      data = filter->getServiceDataMask();
      if (!data.empty()) {
        r = SdBus::AppendDictEntry(message, "ServiceDataMask", data.data());
        if (r < 0) {
          return r;
        }
      }
    }
    auto id = filter->getManufacturerId();
    if (id >0) {
      r = SdBus::AppendDictEntry(message, "ManufacturerId", (int32_t)id);
      if (r < 0) {
        return r;
      }
      auto data = filter->getManufacturerData();
      if (!data.empty()) {
        r = SdBus::AppendDictEntry(message, "ManufacturerData", data.data());
        if (r < 0) {
          return r;
        }
        data = filter->getManufacturerDataMask();
        if (!data.empty()) {
          r = SdBus::AppendDictEntry(message, "ManufacturerDataMask", data.data());
          if (r < 0) {
            return r;
          }
        }
      }
    }

    r = sd_bus_message_close_container(message);
    if (r < 0) {
      return r;
    }
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return r;
}

int GattLeScannerManagerClient::populateScanResultStorageDesc(sd_bus_message *message,
                                                  std::vector<std::vector<ResultStorageDescriptor*>> *storageDescs) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__); // TODO: Not Yet supported so Untested

  if (storageDescs == nullptr) {
    ALOGE(LOGTAG " %s - > populateScanResultStorageDesc is not valid", __func__);
    return -1;
  }

  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', "ScanResultStorageDesc");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "av");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "v");
  if (r < 0) {
    return r;
  }

  for (auto storeDesc : *storageDescs) {

    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;

    int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
    if (r < 0) {
      return r;
    }

    for (auto resultDesc : storeDesc) {
      if (resultDesc == nullptr) {
        ALOGE(LOGTAG " %s - > store scan result descriptor is not valid", __func__);
        return -1;
      }
      r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
      if (r < 0)
        return r;

      int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
      if (r < 0) {
        return r;
      }
      r = SdBus::AppendDictEntry(message, "Type", (int32_t)(resultDesc->getType()));
      if (r < 0) {
        return r;
      }
      r = SdBus::AppendDictEntry(message, "Offset", (int32_t)(resultDesc->getOffset()));
      if (r < 0) {
        return r;
      }
      r = SdBus::AppendDictEntry(message, "Length", (int32_t)resultDesc->getLength());
      if (r < 0) {
        return r;
      }
      r = sd_bus_message_close_container(message);
      if (r < 0) {
        return r;
      }
      r = sd_bus_message_close_container(message);
      if (r < 0) {
        return r;
      }
    }
    r = sd_bus_message_close_container(message);
    if (r < 0) {
      return r;
    }
    r = sd_bus_message_close_container(message);
    if (r < 0) {
      return r;
    }
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return r;
}

int GattLeScannerManagerClient::populateStartScan(sd_bus_message *message,
                                                  ScanSettings *scanSettings,
                                                  std::vector<ScanFilter*> *filters,
                                                  std::vector<std::vector<ResultStorageDescriptor*>> *storages) {
  ALOGI(LOGTAG " %s", __func__);
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  // StartScanSettings Dict Entry
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  // Key
  r = sd_bus_message_append_basic(message, 's', "StartScanSettings");
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  // ScanSettings Entry
  if (scanSettings != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "ScanSettings");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = populateScanSettings(message, scanSettings);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  // ScanFilters Entry
  if (filters != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "ScanFilters");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "aa{sv}");
    if (r < 0)
      return r;
    r = populateScanFilters(message, filters);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }
/*
  // ScanResultsStorageDesc Entry
  if (storages != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "ScanResultsStorageDesc");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = populateScanResultStorageDesc(message, storages);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }*/
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }
  return r;
}
} // namespace ipc
} // namespace gatt
