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

#include "GattServerManagerClient.hpp"
#include <systemd/sd-bus.h>
#include <cstdint>
#include <memory>
#include "BluetoothLeSdbus.hpp"
#include "GattCharacteristic.hpp"
#include "GattDBusFw.hpp"
#include "GattServerListener.hpp"
#include "GattService.hpp"

#define LOGTAG "GattServerManagerClient"
namespace gatt {
namespace ipc {
GattServerManagerClient::GattServerManagerClient(GattDBusFw *dbus_service)
    : dbus_service_(dbus_service) {
  ALOGD(LOGTAG " %s", __func__);
}

void GattServerManagerClient::registerServer(Uuid uuid) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattServerListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE,
      "RegisterServer",
      nullptr,
      nullptr,
      "s",
      uuid.ToString().c_str());
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattServerManagerClient::unregisterServer(int32_t serverIf) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattServerListener() == nullptr) {
    ALOGE(LOGTAG

        " listener object not valid");
    return;
  }

  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE,
      "UnregisterServer",
      nullptr,
      nullptr,
      "i",
      serverIf);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattServerManagerClient::serverConnect(int32_t serverIf, const std::string& address, bool isDirect, int32_t transport) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattServerManagerClient::serverDisconnect(int32_t serverIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattServerManagerClient::serverSetPreferredPhy(int32_t serverIf, const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t phyOptions) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattServerManagerClient::serverReadPhy(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattServerManagerClient::sendResponse(int32_t serverIf, const std::string& address, int32_t requestId, int32_t status, int32_t offset, uint8_t *value, size_t length) {
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
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE,
      "SendResponse");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "i", serverIf);
  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = requestId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument offset_arg;
  offset_arg.type_ = SdBus::SdbusArgument::INT32;
  offset_arg.value_.type_int32 = offset;
  args["offset"] = offset_arg;

  SdBus::SdbusArgument status_arg;
  status_arg.type_ = SdBus::SdbusArgument::INT32;
  status_arg.value_.type_int32 = status;
  args["status"] = status_arg;

  // isLong is not used by server class so is not transmitted;

  res = SdBus::PopulateArgumentMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with Arguments");
    return;
  }

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattServerManagerClient::sendNotification(int32_t serverIf, const std::string& address, int32_t handle, bool confirm, uint8_t *value, size_t length) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  ALOGD(LOGTAG " %s -> address: %s", __func__, address.c_str());


  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE,
      "SendNotification");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "i", serverIf);
  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map
  SdBus::ArgumentMap args;
  SdBus::SdbusArgument attrId_arg;
  attrId_arg.type_ = SdBus::SdbusArgument::INT32;
  attrId_arg.value_.type_int32 = handle;
  args["attrId"] = attrId_arg;

  SdBus::SdbusArgument confirm_arg;
  confirm_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  confirm_arg.value_.type_bool = confirm;
  args["confirm"] = confirm_arg;

  res = SdBus::PopulateArgumentMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with Arguments");
    return;
  }

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattServerManagerClient::addService(int32_t serverIf, GattService *svc) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  if (svc == nullptr) {
    ALOGE(LOGTAG " service object not valid");
    return;
  }
  int res;
  sd_bus_message *m = NULL;
  res = sd_bus_message_new_method_call(
      dbus_service_->GetSdbus(),
      &m,
      GATT_MANAGER_SERVICE_NAME,
      GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE,
      "AddService");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "i", (unsigned)serverIf);

  res = SdBus::PopulateGattServiceMessage(m, svc);
  if (res < 0) {
    ALOGE(LOGTAG " Error Populating sd_bus message with Gatt Service structure");
    return;
  }
  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattServerManagerClient::removeService(int32_t serverIf, int32_t handle) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattServerManagerClient::clearServices(int32_t serverIf) {
  ALOGW(LOGTAG " %s", __func__);
}
}  // namespace ipc
}  // namespace gatt
