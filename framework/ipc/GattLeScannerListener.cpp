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

#include "GattLeScannerListener.hpp"
#include "GattDBusFw.hpp"
#include "ScanResult.hpp"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattLeScannerListener "

namespace gatt {
namespace ipc {
GattLeScannerListener::GattLeScannerListener(GattDBusFw *dbus_service)
    : gatt_dbus_service_(dbus_service) {
  ALOGI(LOGTAG " %s", __func__);
}

GattLeScannerListener::~GattLeScannerListener() {
  ALOGI(LOGTAG " %s", __func__);
  sd_bus_slot_unref(sdbusSlot_);
  sdbusSlot_ = nullptr;
}

bool GattLeScannerListener::InitialiseDBusService() {
  ALOGI(LOGTAG " %s", __func__);
  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (!sdbus) {
    ALOGE(LOGTAG " Unable to get on D-Bus");
    return false;
  }

	  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("ScannerRegistered", "ii", nullptr,
          GattLeScannerListener::sd_ScannerRegistered,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("ScanResult", "a{sv}", nullptr,
          GattLeScannerListener::sd_ScanResult,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("BatchScanResults", "a{sv}", nullptr,
          GattLeScannerListener::sd_BatchScanResults,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("FoundOrLost", "ba{sv}", nullptr,
          GattLeScannerListener::sd_FoundOrLost,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_METHOD("ScanManagerError", "i", nullptr,
          GattLeScannerListener::sd_ScanManagerError,
          SD_BUS_VTABLE_UNPRIVILEGED),
      SD_BUS_VTABLE_END};

  auto res = sd_bus_add_object_vtable(sdbus,
                                      &sdbusSlot_,
                                      BLE_CLIENT_LISTENER_OBJECT_PATH,
                                      GATT_CLIENT_SCANNER_LISTENER_INTERFACE,
                                      vtable,
                                      this);
  if (res < 0) {
    ALOGE(LOGTAG " interface init failed on path %s: %d - %s\n",
        BLE_CLIENT_LISTENER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  res = sd_bus_emit_object_added(sdbus, BLE_CLIENT_LISTENER_OBJECT_PATH);
  if (res < 0) {
    ALOGE(LOGTAG " object manager failed to signal new path %s: %d - %s\n",
                  BLE_CLIENT_LISTENER_OBJECT_PATH, -res, strerror(-res));
  }
  return true;
}

int GattLeScannerListener::sd_ScannerRegistered(sd_bus_message *m,
                                                void *userdata,
                                                sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const int32_t scannerId;
  const int32_t status;

  int result = sd_bus_message_read(m, "ii", &status, &scannerId);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeScannerListener *scannerListener = static_cast<GattLeScannerListener *>(userdata);
  scannerListener->gatt_dbus_service_->scannerCallback()->onScannerRegistered(status, scannerId);

  const char *path = sd_bus_message_get_path(m);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s status = %d scannerId = %d\n",
                        __func__, path, status, scannerId);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerListener::sd_ScanResult(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n        ==> DBUS CALL ENTRY for %s", __func__);
  const char *sender = sd_bus_message_get_sender(m);

  ScanResult *scanResult = nullptr;
  GattLeScannerListener *scannerListener = static_cast<GattLeScannerListener *>(userdata);

  int result = scannerListener->getScanResultFromMessage(m, &scanResult);
  if (result < 0) {
    ALOGE(LOGTAG " Failed to parse AdvertisingSet from message: %s\n", strerror(-result));
    return result;
  }

  scannerListener->gatt_dbus_service_->scannerCallback()->onScanResult(scanResult);
  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s \n", __func__ );
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerListener::sd_BatchScanResults(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerListener::sd_FoundOrLost(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGW(LOGTAG " %s > Not yet supported", __func__);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerListener::sd_ScanManagerError(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG "\n       ==> DBUS CALL ENTRY for %s", __func__);
  const char *path = sd_bus_message_get_path(m);
  const int32_t errorCode;

  int result = sd_bus_message_read(m, "i", &errorCode);
  if (result < 0) {
    ALOGE(LOGTAG "  --> Failed to parse parameters %s", strerror(-result));
    return result;
  }

  GattLeScannerListener *scannerListener = static_cast<GattLeScannerListener *>(userdata);
  scannerListener->gatt_dbus_service_->scannerCallback()->onScanManagerErrorCallback(errorCode);

  ALOGD(LOGTAG "\n       <== DBUS CALL EXIT for %s: %s %d %d %d\n", __func__, path, errorCode);
  return sd_bus_reply_method_return(m, NULL);
}

int GattLeScannerListener::getScanResultFromMessage(sd_bus_message *m, ScanResult **scanResult) {
  // ScanResult fields
  string deviceAddress = "";
  int32_t rssi;
  int32_t eventType;
  int32_t primaryPhy = GattDevice::PHY_LE_1M;
  int32_t secondaryPhy = ScanResult::PHY_UNUSED;
  int32_t advertisingSid = ScanResult::SID_NOT_PRESENT;
  int32_t txPower;
  int32_t periodicAdvertisingInterval;
  uint8_t *scanRecordData = nullptr;
  size_t scanRecordDataSize;

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

    // Get the  key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "ScanRecord") == 0) {
      // res = GattLeScannerListener::getScanRecordFromMessage(m, &scanRecordData);
      res = SdBus::getByteArrayFromMessage(m, &scanRecordData, &scanRecordDataSize);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Rssi") == 0) {
      res = SdBus::getInt32FromMessage(m, &rssi);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "DeviceAddress") == 0) {
      res = SdBus::getStringFromMessage(m, &deviceAddress);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "EventType") == 0) {
      res = SdBus::getInt32FromMessage(m, &eventType);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "PrimaryPhy") == 0) {
      res = SdBus::getInt32FromMessage(m, &primaryPhy);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "SecondaryPhy") == 0) {
      res = SdBus::getInt32FromMessage(m, &secondaryPhy);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "AdvertisingSid") == 0) {
      res = SdBus::getInt32FromMessage(m, &advertisingSid);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "TxPower") == 0) {
      res = SdBus::getInt32FromMessage(m, &txPower);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "PeriodicAdvertisingInterval") == 0) {
      res = SdBus::getInt32FromMessage(m, &periodicAdvertisingInterval);
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

  std::vector<uint8_t> scanRecordBtyes(scanRecordData, scanRecordData + scanRecordDataSize);

  *scanResult = new ScanResult(deviceAddress, eventType, primaryPhy, secondaryPhy,
        advertisingSid, txPower, rssi, periodicAdvertisingInterval,
        ScanRecord::parseFromBytes(scanRecordBtyes));
  return res;
}
} // namespace ipc
} // namespace gatt
