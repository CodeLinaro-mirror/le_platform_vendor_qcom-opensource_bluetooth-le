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

#include "GattServerListener.hpp"
#include <systemdq/sd-bus.h>
#include <unistd.h>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusFw.hpp"
#include "GattServer.hpp"

#define LOGTAG "GattServerListener"

namespace gatt {
namespace ipc {
GattServerListener::GattServerListener(GattDBusFw *dbus_service)
    : dbus_service_(dbus_service) {
}

bool GattServerListener::InitialiseDBusService() {
  ALOGI(LOGTAG " %s", __func__);

  auto sdbus = dbus_service_->GetSdbus();
  if (!dbus_service_->GetSdbus()) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("ServerRegistered", "ii", nullptr, GattServerListener::sd_ServerRegistered,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ServiceAdded", "ia{sv}", nullptr, GattServerListener::sd_ServiceAdded,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ConnnectionState", "sa{sv}", nullptr, GattServerListener::sd_ConnnectionState,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("CharacteristicReadRequest", "sa{sv}", nullptr, GattServerListener::sd_CharacteristicReadRequest,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("DescriptorReadRequest", "sa{sv}", nullptr, GattServerListener::sd_DescriptorReadRequest,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("CharacteristicWriteRequest", "sa{sv}ay", nullptr, GattServerListener::sd_CharacteristicWriteRequest,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("DescriptorWriteRequest", "sa{sv}ay", nullptr, GattServerListener::sd_DescriptorWriteRequest,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("NotificationSent", "si", nullptr, GattServerListener::sd_NotificationSent,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ExecuteWrite", "sa{sv}", nullptr, GattServerListener::sd_ExecuteWrite,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("MtuChanged", "si", nullptr, GattServerListener::sd_MtuChanged,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ConnectionUpdated", "sa{sv}", nullptr, GattServerListener::sd_ConnectionUpdated,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_VTABLE_END};

  auto res = sd_bus_add_object_vtable(sdbus, nullptr, GATT_SERVER_LISTENER_OBJECT_PATH,
      GATT_SERVER_LISTENER_INTERFACE, vtable, this);
  if (res < 0) {
    ALOGE(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_SERVER_LISTENER_OBJECT_PATH, -res, strerror(-res));

    sd_bus_emit_object_removed(sdbus, GATT_SERVER_LISTENER_OBJECT_PATH);

    return false;
  }

  res = sd_bus_emit_object_added(sdbus, GATT_SERVER_LISTENER_OBJECT_PATH);
  if (res < 0) {
    ALOGE(LOGTAG
        "object manager failed to signal new path %s: %d - %s\n",
        GATT_SERVER_LISTENER_OBJECT_PATH, -res, strerror(-res));
  }
  return true;
}

int GattServerListener::sd_ServerRegistered(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  int result;
  int32_t status = 0;
  int32_t serverId = 0;
  result = sd_bus_message_read(m, "ii", &status, &serverId);

  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }
  ALOGD(LOGTAG " %s -> status:%d, serverId:%d", __func__, status, serverId);

  if (gatt_server_listener_ptr != nullptr) {
    gatt_server_listener_ptr->onServerRegistered(status, serverId);
  }

  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onServerRegistered(int32_t status, int32_t serverId) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }
  dbus_service_->GetGattServer()->onServerRegistered(status, serverId);
  return true;
}

int GattServerListener::sd_ServiceAdded(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  int32_t status = 0;

  // Get the serverId
  int result = sd_bus_message_read(m, "i", &status);
  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }

  // Get the Gatt Service object from the message
  GattService *gatt_service;
  result = SdBus::getGattServiceFromMessage(m, &gatt_service);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-result));
    return result;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onServiceAdded(status, gatt_service)) {
      ALOGE(LOGTAG "Failed to call AddService");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onServiceAdded(int32_t status, gatt::GattService *gatt_service) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }
  dbus_service_->GetGattServer()->onServiceAdded(status, gatt_service);
  return true;
}

int GattServerListener::sd_ConnnectionState(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  SdBus::ArgumentMap args;
  const char *deviceAddress;

  // Get the serverId
  int result = sd_bus_message_read(m, "s", &deviceAddress);
  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }

  // Get the arguement map from the message
  result = SdBus::getArgumentsFromMessage(m, args);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse args from message: %s\n", strerror(-result));
    return result;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onConnnectionState(std::string(deviceAddress), args)) {
      ALOGE(LOGTAG "Failed to call onConnnectionState");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onConnnectionState(const std::string &deviceAddress, SdBus::ArgumentMap &args) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto status_entry = args.find("status");
  auto serverId_entry = args.find("serverId");
  auto connected_entry = args.find("connected");

  if (status_entry == args.end() || serverId_entry == args.end() || connected_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int status = status_entry->second.value_.type_int32;
  int serverId = serverId_entry->second.value_.type_int32;
  bool connected = connected_entry->second.value_.type_bool;

  dbus_service_->GetGattServer()->onConnectionState(status, serverId, connected, deviceAddress);
  return true;
}

int GattServerListener::sd_CharacteristicReadRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  SdBus::ArgumentMap args;
  const char *deviceAddress;

  // Get the serverId
  int result = sd_bus_message_read(m, "s", &deviceAddress);
  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }

  // Get the arguement map from the message
  result = SdBus::getArgumentsFromMessage(m, args);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse args from message: %s\n", strerror(-result));
    return result;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onCharacteristicReadRequest(std::string(deviceAddress), args)) {
      ALOGE(LOGTAG "Failed to call onCharacteristicReadRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onCharacteristicReadRequest(const std::string &deviceAddress, SdBus::ArgumentMap &args) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto requestId_entry = args.find("requestId");
  auto offset_entry = args.find("offset");
  auto characteristic_entry = args.find("characteristic");

  if (requestId_entry == args.end() || offset_entry == args.end() || characteristic_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  int offset = offset_entry->second.value_.type_int32;
  int characteristic = characteristic_entry->second.value_.type_int32;

  dbus_service_->GetGattServer()->onCharacteristicReadRequest(deviceAddress, requestId, offset, true, characteristic);

  return true;
}

int GattServerListener::sd_DescriptorReadRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  SdBus::ArgumentMap args;
  const char *deviceAddress;

  // Get the serverId
  int result = sd_bus_message_read(m, "s", &deviceAddress);
  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }

  // Get the arguement map from the message
  result = SdBus::getArgumentsFromMessage(m, args);
  if (result < 0) {
    ALOGE(LOGTAG "Failed to parse args from message: %s\n", strerror(-result));
    return result;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onDescriptorReadRequest(std::string(deviceAddress), args)) {
      ALOGE(LOGTAG "Failed to call onDescriptorReadRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onDescriptorReadRequest(const std::string &deviceAddress, SdBus::ArgumentMap &args) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto requestId_entry = args.find("requestId");
  auto offset_entry = args.find("offset");
  auto descriptor_entry = args.find("descriptor");

  if (requestId_entry == args.end() || offset_entry == args.end() || descriptor_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  int offset = offset_entry->second.value_.type_int32;
  int descriptor = descriptor_entry->second.value_.type_int32;

  dbus_service_->GetGattServer()->onDescriptorReadRequest(deviceAddress, requestId, offset, true, descriptor);

  return true;
}

int GattServerListener::sd_CharacteristicWriteRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  const char *deviceAddress;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-res));
    return res;
  }

  size_t array_size;
  uint8_t *value_array;
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }
  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onCharacteristicWriteRequest(std::string(deviceAddress), args, value_array)) {
      ALOGE(LOGTAG "Failed to call onCharacteristicWriteRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onCharacteristicWriteRequest(const std::string &deviceAddress, SdBus::ArgumentMap &args, uint8_t *value) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto requestId_entry = args.find("requestId");
  auto characteristic_entry = args.find("characteristic");
  auto preparedWrite_entry = args.find("preparedWrite");
  auto responseNeeded_entry = args.find("responseNeeded");
  auto offset_entry = args.find("offset");

  if (requestId_entry == args.end() || characteristic_entry == args.end() || preparedWrite_entry == args.end() || responseNeeded_entry == args.end() || offset_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  int characteristic = characteristic_entry->second.value_.type_int32;
  bool preparedWrite = preparedWrite_entry->second.value_.type_bool;
  bool responseNeeded = responseNeeded_entry->second.value_.type_bool;
  int offset = offset_entry->second.value_.type_int32;
  // length is 0 as it is not used.
  int length = 0;

  dbus_service_->GetGattServer()->onCharacteristicWriteRequest(deviceAddress, requestId, offset,
      length, preparedWrite, responseNeeded,
      characteristic, value);

  return true;
}

int GattServerListener::sd_DescriptorWriteRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  const char *deviceAddress;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-res));
    return res;
  }

  size_t array_size;
  uint8_t *value_array;
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }
  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onDescriptorWriteRequest(std::string(deviceAddress), args, value_array)) {
      ALOGE(LOGTAG "Failed to call onDescriptorWriteRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattServerListener::sd_NotificationSent(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  int result;
  const char *deviceAddress;
  int32_t status = 0;
  result = sd_bus_message_read(m, "si", &deviceAddress, &status);

  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }
  ALOGD(LOGTAG " %s -> deviceAddress:%s, status:%d", __func__, deviceAddress, status);

  if (gatt_server_listener_ptr != nullptr) {
    gatt_server_listener_ptr->onNotificationSent(std::string(deviceAddress), status);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattServerListener::sd_ExecuteWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  const char *deviceAddress;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-res));
    return res;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onExecuteWrite(std::string(deviceAddress), args)) {
      ALOGE(LOGTAG "Failed to call onDescriptorWriteRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattServerListener::sd_MtuChanged(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  int result;
  const char *deviceAddress;
  int32_t mtu = 0;
  result = sd_bus_message_read(m, "si", &deviceAddress, &mtu);

  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }
  ALOGD(LOGTAG " %s -> deviceAddress:%s, mtu:%d", __func__, deviceAddress, mtu);

  if (gatt_server_listener_ptr != nullptr) {
    gatt_server_listener_ptr->onMtuChanged(std::string(deviceAddress), mtu);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattServerListener::sd_ConnectionUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerListener *gatt_server_listener_ptr = static_cast<GattServerListener *>(userdata);

  const char *deviceAddress;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-res));
    return res;
  }

  if (gatt_server_listener_ptr != nullptr) {
    if (!gatt_server_listener_ptr->onConnectionUpdated(std::string(deviceAddress), args)) {
      ALOGE(LOGTAG "Failed to call onDescriptorWriteRequest");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerListener::onDescriptorWriteRequest(const std::string &deviceAddress, SdBus::ArgumentMap &args, uint8_t *value) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto requestId_entry = args.find("requestId");
  auto descriptor_entry = args.find("descriptor");
  auto preparedWrite_entry = args.find("preparedWrite");
  auto responseNeeded_entry = args.find("responseNeeded");
  auto offset_entry = args.find("offset");

  if (requestId_entry == args.end() || descriptor_entry == args.end() || preparedWrite_entry == args.end() || responseNeeded_entry == args.end() || offset_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  int descriptor = descriptor_entry->second.value_.type_int32;
  bool preparedWrite = preparedWrite_entry->second.value_.type_bool;
  bool responseNeeded = responseNeeded_entry->second.value_.type_bool;
  int offset = offset_entry->second.value_.type_int32;
  // length is 0 as it is not used.
  int length = 0;

  dbus_service_->GetGattServer()->onDescriptorWriteRequest(deviceAddress, requestId, offset,
      length, preparedWrite, responseNeeded,
      descriptor, value);

  return true;
}

bool GattServerListener::onNotificationSent(const std::string &deviceAddress, int32_t status) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }
  dbus_service_->GetGattServer()->onNotificationSent(deviceAddress, status);
  return true;
}

bool GattServerListener::onExecuteWrite(const std::string &deviceAddress, SdBus::ArgumentMap &args)
{
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto requestId_entry = args.find("requestId");
  auto execute_entry = args.find("execute");

  if (requestId_entry == args.end() || execute_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  bool execute = execute_entry->second.value_.type_bool;

  dbus_service_->GetGattServer()->onExecuteWrite(deviceAddress, requestId, execute);
  return true;
}

bool GattServerListener::onMtuChanged(const std::string &deviceAddress, int32_t mtu)
{
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }
  dbus_service_->GetGattServer()->onMtuChanged(deviceAddress, mtu);
  return true;
}

bool GattServerListener::onConnectionUpdated(const std::string &deviceAddress, SdBus::ArgumentMap &args)
{
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->GetGattServer() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt server", __func__);
    return false;
  }

  auto status_entry = args.find("status");
  auto interval_entry = args.find("interval");
  auto latency_entry = args.find("latency");
  auto timeout_entry = args.find("timeout");

  if (status_entry == args.end() || interval_entry == args.end() || latency_entry == args.end() || timeout_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int status = status_entry->second.value_.type_int32;
  int interval = interval_entry->second.value_.type_int32;
  int latency = latency_entry->second.value_.type_int32;
  int timeout = timeout_entry->second.value_.type_int32;

  dbus_service_->GetGattServer()->onConnectionUpdated(deviceAddress, interval, latency, timeout, status);
  return true;
}
}  // namespace ipc
}  // namespace gatt
