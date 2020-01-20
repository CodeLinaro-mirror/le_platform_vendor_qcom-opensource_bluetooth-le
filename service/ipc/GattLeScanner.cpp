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

#include "GattLeScanner.hpp"
#include <vector>
#include "GattDBusService.hpp"
#include "ScanResult.hpp"
#include "ScanRecord.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeScanner"

namespace gatt {
namespace ipc {
GattLeScanner::GattLeScanner(GattDBusService *dbus_service)
    : gatt_dbus_service_(dbus_service) {
  ALOGI(LOGTAG " %s", __func__);
}

bool GattLeScanner::onScannerRegistered(const string &server,
                                        int32_t status,
                                        int32_t scannerId) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s, status = %d scannerId = %d\n",
                             __func__, status, scannerId);

  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res = sd_bus_call_method_async(sdbus,
      nullptr,
      server.c_str(),
      BLE_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_LISTENER_INTERFACE,
      "ScannerRegistered",
      nullptr,
      nullptr,
      "ii",
      status,
      scannerId);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScanner::onScanResult(const string &server, ScanResult *scanResult, std::vector<uint8_t> *scanRecord) {
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
      server.c_str(),
      BLE_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_LISTENER_INTERFACE,
      "ScanResult");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return false;
  }

  // res = sd_bus_message_append(m, "i", scannerId);
  // if (res < 0) {
  //   ALOGE(LOGTAG " Error Populating sd_bus message with advertiserId ");
  //   // goto done;
  // }

  res = GattLeScanner::populateScanResult(m, scanResult, scanRecord);
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
  ALOGI(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeScanner::onBatchScanResults(const string &server, std::vector<ScanResult*> batchResults) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return false;
}

bool GattLeScanner::onFoundOrLost(const string &server, bool onFound, ScanResult *scanResult) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return false;
}

bool GattLeScanner::onScanManagerErrorCallback(const string &server, int errorCode) {
  ALOGI(LOGTAG "\n   > RPC invoked for %s", __func__);
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  ALOGI(LOGTAG " %s -> Calling sd_bus_call_method_async", __func__);
  res = sd_bus_call_method_async(sdbus,
      nullptr,
      server.c_str(),
      BLE_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_SCANNER_LISTENER_INTERFACE,
      "ScanManagerErrorCallback",
      nullptr,
      nullptr,
      "i",
      errorCode);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

int GattLeScanner::populateScanResult(sd_bus_message *message,
                                      ScanResult *scanResult,
                                      std::vector<uint8_t> *scanRecord) {

  ALOGI(LOGTAG " %s", __func__);
  if (scanResult == nullptr) {
    ALOGE(LOGTAG " %s - > populateScanResult is not valid", __func__);
    return -1;
  }
  int r= sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  r = SdBus::AppendDictEntry(message, "ScanRecord", scanRecord->data());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "Rssi", (int32_t)scanResult->getRssi());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "DeviceAddress", scanResult->getDevice());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "EventType", scanResult->getEventType());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "PrimaryPhy", (int32_t)scanResult->getPrimaryPhy());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "SecondaryPhy", (int32_t)scanResult->getSecondaryPhy());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "AdvertisingSid", (int32_t)scanResult->getAdvertisingSid());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "TxPower", (int32_t)scanResult->getTxPower());
  if (r < 0) {
    return r;
  }
  r = SdBus::AppendDictEntry(message, "PeriodicAdvertisingInterval", (int32_t)scanResult->getPeriodicAdvertisingInterval());
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }

  return 1;
}
} // namespace ipc
} // namespace gatt
