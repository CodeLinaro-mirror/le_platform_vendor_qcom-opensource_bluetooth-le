/*
Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.

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

#include "GattClientListenerClient.hpp"
#include <cstdint>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusService.hpp"
#include "GattService.hpp"

#define LOGTAG "GattClientListenerClient"

namespace gatt {
namespace ipc {

GattClientListenerClient::GattClientListenerClient(GattDBusService *dbus_service)
    : dbus_service_(dbus_service) {
}

void GattClientListenerClient::onClientRegistered(int32_t status, int32_t serverIf, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  auto sdbus = dbus_service_->GetSdbus();
  if (sdbus == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  ALOGD(LOGTAG " %s -> Calling sd_bus_call_method_async", __func__);
  res = sd_bus_call_method_async(sdbus,
      nullptr,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "ClientRegistered",
      nullptr,
      nullptr,
      "ii",
      status,
      serverIf);
  ALOGD(LOGTAG " %s -> sd_bus_call_method_async Called RES: %d", __func__, res);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onConnectionState(int32_t status, int32_t clientIf, bool connected, const std::string& address, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }

  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "ConnnectionState");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the address argument
  res = sd_bus_message_append(m, "s", address.c_str());

  // Create arguments map/
  SdBus::ArgumentMap args;
  SdBus::SdbusArgument status_arg;
  status_arg.type_ = SdBus::SdbusArgument::INT32;
  status_arg.value_.type_int32 = status;
  args["status"] = status_arg;

  SdBus::SdbusArgument clientId_arg;
  clientId_arg.type_ = SdBus::SdbusArgument::INT32;
  clientId_arg.value_.type_int32 = clientIf;
  args["clientId"] = clientId_arg;

  SdBus::SdbusArgument connected_arg;
  connected_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  connected_arg.value_.type_bool = connected;
  args["connected"] = connected_arg;

  res = SdBus::PopulateArgumentMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with Arguments");
    return;
  }
  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onPhyUpdate(const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}

void GattClientListenerClient::onPhyRead(const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}

void GattClientListenerClient::onSearchComplete(const std::string& address, std::vector<GattService *> services, int32_t status, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }

  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "SearchComplete");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the address argument
  res = sd_bus_message_append(m, "s", address.c_str());
  if (res < 0) {
    ALOGE(LOGTAG " %s -> Could not append to message", __func__);
    return;
  }

  res = SdBus::PopulateGattServices(m, services);
  if (res < 0) {
    ALOGE(LOGTAG " %s -> Could not append to message", __func__);
    return;
  }

  res = sd_bus_message_append(m, "i", (int32_t)status);
  if (res < 0) {
    ALOGE(LOGTAG " %s -> Could not append to message", __func__);
    return;
  }
  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onCharacteristicRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "CharacteristicRead");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the clientIf argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Add the address argument
  res = sd_bus_message_append(m, "i", status);
  // Add the handle argument
  res = sd_bus_message_append(m, "i", handle);

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onCharacteristicWrite(const std::string& address, int32_t status, int32_t handle, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  ALOGD(LOGTAG " %s -> Calling sd_bus_call_method_async", __func__);

  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "CharacteristicWrite",
      nullptr,
      nullptr,
      "sii",
      address.c_str(),
      status,
      handle);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattClientListenerClient::onExecuteWrite(const std::string& address, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}

void GattClientListenerClient::onDescriptorRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "DescriptorRead");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the clientIf argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Add the address argument
  res = sd_bus_message_append(m, "i", status);
  // Add the handle argument
  res = sd_bus_message_append(m, "i", handle);

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onDescriptorWrite(const std::string& address, int32_t status, int32_t handle, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "DescriptorWrite",
      nullptr,
      nullptr,
      "sii",
      address.c_str(),
      status,
      handle);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattClientListenerClient::onNotify(const std::string& address, int32_t handle, uint8_t *value, int length, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      service_name.c_str(),
      GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE,
      "Notify");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the clientIf argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Add the address argument
  res = sd_bus_message_append(m, "i", handle);
  // Add the handle argument

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattClientListenerClient::onReadRemoteRssi(const std::string&  address, int32_t rssi, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}

void GattClientListenerClient::onConfigureMTU(const std::string&  address, int32_t mtu, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}

void GattClientListenerClient::onConnectionUpdated(const std::string& address, int32_t interval, int32_t latency, int32_t timeout, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not currently implemented", __func__);
}
}  // namespace ipc
}  // namespace gatt
