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

#include "GattLeAdvertising.hpp"
#include <systemd/sd-bus.h>
#include "GattDBusService.hpp"

#define LOGTAG "GattLeAdvertising"
namespace gatt {
namespace ipc {
GattLeAdvertising::GattLeAdvertising(GattDBusService *dbus_service)
    :  gatt_dbus_service_(dbus_service) {
  ALOGD(LOGTAG " %s", __func__);
}

bool GattLeAdvertising::AdvertisingSetStarted(const string &server, int32_t advertiserId, int32_t tx_power, int32_t status) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "AdvertisingSetStarted",
      nullptr, nullptr,
      "iii",
      advertiserId, tx_power, status);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::AdvertisingSetStopped(const string &server, int32_t advertiserId) {
  ALOGD(LOGTAG " %s", __func__);

  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method(
      gatt_dbus_service_->GetSdbus(),
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "AdvertisingSetStopped",
      nullptr, nullptr,
      "i",
      advertiserId);
  ALOGD(LOGTAG " %s -> sd_bus_call_method for advertisingSetStopped: %d", __func__, res);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return false;
  }
  return true;
}

bool GattLeAdvertising::OwnAddressRead(const string &server, int32_t advertiserId, int32_t addressType, std::string *devAddress) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }
  char *address = devAddress->c_str();
  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "OwnAddressRead",
      nullptr, nullptr,
      "iis",
      advertiserId, addressType, address);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::AdvertisingEnabled(const string &server, int32_t advertiserId, bool enabled, int32_t status) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "AdvertisingEnabled",
      nullptr, nullptr,
      "ibi",
      advertiserId, enabled, status);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::AdvertisingDataSet(const string &server, int32_t advertiserId, int32_t status) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "AdvertisingDataSet",
      nullptr, nullptr,
      "ii",
      advertiserId, status);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::ScanResponseDataSet(const string &server, int32_t advertiserId, int32_t status) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "ScanResponseDataSet",
      nullptr, nullptr,
      "ii",
      advertiserId, status);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::AdvertisingParametersUpdated(
                              const string &server, int32_t advertiserId, int32_t tx_power, int32_t status) {
  ALOGD(LOGTAG "\n   > RPC invoked for %s", __func__);
  if (gatt_dbus_service_ == nullptr){
    ALOGE(LOGTAG " dbus service unavailable");
    return false;
  }
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " dbus service unaccquired");
    return false;
  }

  auto res = sd_bus_call_method_async(
      gatt_dbus_service_->GetSdbus(),
      nullptr,
      server.c_str(),
      BLE_LISTENER_OBJECT_PATH,
      GATT_SERVER_ADVERTISER_LISTENER_INTERFACE,
      "AdvertisingParametersUpdated",
      nullptr, nullptr,
      "iii",
      advertiserId, tx_power, status);
  if (res < 0) {
    ALOGE(LOGTAG " %s Error calling Method: %d - %s\n", __func__, -res, strerror(-res));
    return false;
  }
  ALOGD(LOGTAG "\n   < RPC completion for %s.\n", __func__);
  return true;
}

bool GattLeAdvertising::PeriodicAdvertisingParametersUpdated(const string &server, int32_t advertiserId, int32_t status) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}

bool GattLeAdvertising::PeriodicAdvertisingDataSet(const string &server, int32_t advertiserId, int32_t status) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}

bool GattLeAdvertising::PeriodicAdvertisingEnabled(const string &server, int32_t advertiserId, bool enable, int32_t status) {
  ALOGD(LOGTAG "\n   > Not yet supported %s", __func__);
  return false;
}
} // namespace ipc
} // namespace gatt
