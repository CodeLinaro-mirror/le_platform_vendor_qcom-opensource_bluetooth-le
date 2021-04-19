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

#include "GattServerListenerClient.hpp"
#include <cstdint>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusService.hpp"
#include "GattService.hpp"

#define LOGTAG "GattServerListenerClient"
namespace gatt {
namespace ipc {
void GattServerListenerClient::onServerRegistered(int32_t status, int32_t serverIf, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "ServerRegistered",
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
  ALOGD(LOGTAG " %s -> Leaving ", __func__);
}

GattServerListenerClient::GattServerListenerClient(GattDBusService *dbus_service)
    : dbus_service_(dbus_service) {
}

void GattServerListenerClient::onServiceAdded(int32_t status, GattService *svc, const std::string& service_name) {
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
      service_name.c_str(),
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "ServiceAdded");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "i", (unsigned)status);

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

void GattServerListenerClient::onConnectionState(int32_t status, int32_t serverIf, bool connected, const std::string& address, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "ConnnectionState");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument status_arg;
  status_arg.type_ = SdBus::SdbusArgument::INT32;
  status_arg.value_.type_int32 = serverIf;
  args["status"] = status_arg;

  SdBus::SdbusArgument serverId_arg;
  serverId_arg.type_ = SdBus::SdbusArgument::INT32;
  serverId_arg.value_.type_int32 = status;
  args["serverId"] = serverId_arg;

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

void GattServerListenerClient::onCharacteristicReadRequest(const std::string& address, int32_t transId, int32_t offset, bool isLong, int32_t handle, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "CharacteristicReadRequest");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = transId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument offset_arg;
  offset_arg.type_ = SdBus::SdbusArgument::INT32;
  offset_arg.value_.type_int32 = offset;
  args["offset"] = offset_arg;

  SdBus::SdbusArgument characteristic_arg;
  characteristic_arg.type_ = SdBus::SdbusArgument::INT32;
  characteristic_arg.value_.type_int32 = handle;
  args["characteristic"] = characteristic_arg;

  // isLong is not used by server class so is not transmitted;

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

void GattServerListenerClient::onDescriptorReadRequest(const std::string& address, int32_t transId, int32_t offset, bool isLong, int32_t handle, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "DescriptorReadRequest");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = transId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument offset_arg;
  offset_arg.type_ = SdBus::SdbusArgument::INT32;
  offset_arg.value_.type_int32 = offset;
  args["offset"] = offset_arg;

  SdBus::SdbusArgument descriptor_arg;
  descriptor_arg.type_ = SdBus::SdbusArgument::INT32;
  descriptor_arg.value_.type_int32 = handle;
  args["descriptor"] = descriptor_arg;

  // isLong is not used by server class so is not transmitted;

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

void GattServerListenerClient::onCharacteristicWriteRequest(const std::string& address, int32_t transId, int32_t offset, int32_t length, bool isPrep, bool needRsp, int32_t handle, uint8_t *value, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "CharacteristicWriteRequest");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }

  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = transId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument characteristic_arg;
  characteristic_arg.type_ = SdBus::SdbusArgument::INT32;
  characteristic_arg.value_.type_int32 = handle;
  args["characteristic"] = characteristic_arg;

  SdBus::SdbusArgument preparedWrite_arg;
  preparedWrite_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  preparedWrite_arg.value_.type_bool = isPrep;
  args["preparedWrite"] = preparedWrite_arg;

  SdBus::SdbusArgument responseNeeded_arg;
  responseNeeded_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  responseNeeded_arg.value_.type_bool = needRsp;
  args["responseNeeded"] = responseNeeded_arg;

  SdBus::SdbusArgument offset_arg;
  offset_arg.type_ = SdBus::SdbusArgument::INT32;
  offset_arg.value_.type_int32 = offset;
  args["offset"] = offset_arg;

  // length is not used by server class so is not transmitted;

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

void GattServerListenerClient::onDescriptorWriteRequest(const std::string& address, int32_t transId, int32_t offset, int32_t length, bool isPrep, bool needRsp, int32_t handle, uint8_t *value, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "DescriptorWriteRequest");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }

  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = transId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument descriptor_arg;
  descriptor_arg.type_ = SdBus::SdbusArgument::INT32;
  descriptor_arg.value_.type_int32 = handle;
  args["descriptor"] = descriptor_arg;

  SdBus::SdbusArgument preparedWrite_arg;
  preparedWrite_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  preparedWrite_arg.value_.type_bool = isPrep;
  args["preparedWrite"] = preparedWrite_arg;

  SdBus::SdbusArgument responseNeeded_arg;
  responseNeeded_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  responseNeeded_arg.value_.type_bool = needRsp;
  args["responseNeeded"] = responseNeeded_arg;

  SdBus::SdbusArgument offset_arg;
  offset_arg.type_ = SdBus::SdbusArgument::INT32;
  offset_arg.value_.type_int32 = offset;
  args["offset"] = offset_arg;

  // length is not used by server class so is not transmitted;

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

void GattServerListenerClient::onExecuteWrite(const std::string& address, int32_t transId, bool execWrite, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "ExecuteWrite");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }

  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument requestId_arg;
  requestId_arg.type_ = SdBus::SdbusArgument::INT32;
  requestId_arg.value_.type_int32 = transId;
  args["requestId"] = requestId_arg;

  SdBus::SdbusArgument execute_arg;
  execute_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  execute_arg.value_.type_bool = execWrite;
  args["execute"] = execute_arg;

  // length is not used by server class so is not transmitted;

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

void GattServerListenerClient::onNotificationSent(const std::string& address, int32_t status, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      service_name.c_str(),
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "NotificationSent",
      nullptr,
      nullptr,
      "si",
      address.c_str(),
      status);
  ALOGD(LOGTAG " %s -> sd_bus_call_method_async Called RES: %d", __func__, res);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattServerListenerClient::onMtuChanged(const std::string& address, int32_t mtu, const std::string& service_name) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      service_name.c_str(),
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "MtuChanged",
      nullptr,
      nullptr,
      "si",
      address.c_str(),
      mtu);
  ALOGD(LOGTAG " %s -> sd_bus_call_method_async Called RES: %d", __func__, res);
  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}

void GattServerListenerClient::onPhyUpdate(const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not implemented", __func__);
}

void GattServerListenerClient::onPhyRead(const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name) {
  ALOGW(LOGTAG " %s -> Not implemented", __func__);
}

void GattServerListenerClient::onConnectionUpdated(const std::string& address, int interval, int latency, int timeout, int status, const std::string& service_name) {
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
      GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE,
      "ConnectionUpdated");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }

  res = sd_bus_message_append(m, "s", address.c_str());
  // Create arguments map/
  SdBus::ArgumentMap args;

  SdBus::SdbusArgument status_arg;
  status_arg.type_ = SdBus::SdbusArgument::INT32;
  status_arg.value_.type_int32 = status;
  args["status"] = status_arg;

  SdBus::SdbusArgument interval_arg;
  interval_arg.type_ = SdBus::SdbusArgument::INT32;
  interval_arg.value_.type_int32 = interval;
  args["interval"] = interval_arg;

  SdBus::SdbusArgument latency_arg;
  latency_arg.type_ = SdBus::SdbusArgument::INT32;
  latency_arg.value_.type_int32 = latency;
  args["latency"] = latency_arg;

  SdBus::SdbusArgument timeout_arg;
  timeout_arg.type_ = SdBus::SdbusArgument::INT32;
  timeout_arg.value_.type_int32 = timeout;
  args["timeout"] = timeout_arg;

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
}  // namespace ipc
}  // namespace gatt
