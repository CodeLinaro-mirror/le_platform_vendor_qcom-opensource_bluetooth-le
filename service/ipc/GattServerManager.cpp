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

#include "GattServerManager.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusService.hpp"
#define LOGTAG "GattServerManager"

using bt::Uuid;

namespace gatt {
namespace ipc {
GattServerManager::GattServerManager(GattDBusService *dbus_service)
    : gatt_dbus_service_(dbus_service){};

bool GattServerManager::InitialiseDBusService() {
  ALOGI(LOGTAG " %s", __func__);

  auto sdbus = gatt_dbus_service_->GetSdbus();
  if (!sdbus) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),

      SD_BUS_METHOD("RegisterServer", "s", nullptr, GattServerManager::sd_RegisterServer,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("AddService", "ia{sv}", nullptr, GattServerManager::sd_AddService,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("SendResponse", "isa{sv}ay", nullptr, GattServerManager::sd_SendResponse,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("SendNotification", "isa{sv}ay", nullptr, GattServerManager::sd_SendNotification,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("UnregisterServer", "i", nullptr, GattServerManager::sd_UnregisterServer,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_VTABLE_END};

  int res = sd_bus_add_object_vtable(sdbus, nullptr, GATT_SERVER_MANAGER_OBJECT_PATH,
      GATT_SERVER_MANAGER_INTERFACE, vtable, this);

  if (res < 0) {
    ALOGE(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_SERVER_MANAGER_OBJECT_PATH, -res, strerror(-res));
    sd_bus_emit_object_removed(sdbus, GATT_SERVER_MANAGER_OBJECT_PATH);
    return false;
  }
  res = sd_bus_emit_object_added(sdbus, GATT_SERVER_MANAGER_OBJECT_PATH);
  if (res < 0) {
    ALOGE(LOGTAG
        "object manager failed to signal new path %s: %d - %s\n",
        GATT_SERVER_MANAGER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  return true;
}
int GattServerManager::sd_AddService(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerManager *gatt_server_manager_ptr = static_cast<GattServerManager *>(userdata);

  int32_t serverId = 0;

  // Get the serverId
  int res = sd_bus_message_read(m, "i", &serverId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  // Get the Gatt Service object from the message
  GattService *gatt_service;
  res = SdBus::getGattServiceFromMessage(m, &gatt_service);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse GattService from message: %s\n", strerror(-res));
    return res;
  }

  if (gatt_server_manager_ptr != nullptr) {
    if (!gatt_server_manager_ptr->onAddService(serverId, gatt_service)) {
      ALOGE(LOGTAG "Failed to call AddService");
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattServerManager::sd_RegisterServer(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerManager *gatt_server_manager_ptr = static_cast<GattServerManager *>(userdata);

  const char *sender = sd_bus_message_get_sender(m);
  const char *uuid;
  int res;

  res = sd_bus_message_read(m, "s", &uuid);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG " sender:%s uuid:%s", sender, uuid);

  if (gatt_server_manager_ptr != nullptr) {
    if (!gatt_server_manager_ptr->onRegisterServer(std::string(uuid), std::string(sender))) {
      ALOGE(LOGTAG "Failed to call Register Server");
      return sd_bus_reply_method_return(m, NULL);
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerManager::onRegisterServer(const std::string &uuid, const std::string &gattServerListener_sender) {
  ALOGI(LOGTAG " %s", __func__);
  if (gatt_dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG "Sdbus Handle not available");
    return false;
  }

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  bool is_valid_uuid;
  auto register_uuid = Uuid::FromString(uuid.c_str(), &is_valid_uuid);
  if (!is_valid_uuid) {
    ALOGE(LOGTAG " %s -> UUID is not valid", __func__);
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->registerServer(register_uuid, gattServerListener_sender);
  return true;
}

bool GattServerManager::onAddService(int32_t serverId, GattService *gatt_service) {
  ALOGI(LOGTAG " %s", __func__);
  gatt_dbus_service_->GetGattLibService()->addService(serverId, gatt_service);

  return true;
}
int GattServerManager::sd_SendResponse(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerManager *gatt_server_manager_ptr = static_cast<GattServerManager *>(userdata);

  const char *deviceAddress;
  int32_t serverId;
  int res;

  res = sd_bus_message_read(m, "is", &serverId, &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse Arguments from message: %s\n", strerror(-res));
    return res;
  }

  size_t array_size;
  uint8_t *value_array;
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }

  if (gatt_server_manager_ptr != nullptr) {
    if (!gatt_server_manager_ptr->onSendResponse(serverId, std::string(deviceAddress), args, value_array)) {
      return sd_bus_reply_method_return(m, NULL);
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattServerManager::sd_SendNotification(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerManager *gatt_server_manager_ptr = static_cast<GattServerManager *>(userdata);

  const char *deviceAddress;
  int32_t serverId;
  int res;

  res = sd_bus_message_read(m, "is", &serverId, &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  SdBus::ArgumentMap args;
  res = SdBus::getArgumentsFromMessage(m, args);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse Arguments from message: %s\n", strerror(-res));
    return res;
  }

  size_t array_size;
  uint8_t *value_array;
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }

  if (gatt_server_manager_ptr != nullptr) {
    if (!gatt_server_manager_ptr->onSendNotification(serverId, std::string(deviceAddress), args, value_array)) {
      return sd_bus_reply_method_return(m, NULL);
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}
bool GattServerManager::onSendResponse(int32_t serverId, const std::string &deviceAddress, SdBus::ArgumentMap &args, uint8_t *value) {
  ALOGI(LOGTAG " %s", __func__);

  auto requestId_entry = args.find("requestId");
  auto offset_entry = args.find("offset");
  auto status_entry = args.find("status");

  if (requestId_entry == args.end() || offset_entry == args.end() || status_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int requestId = requestId_entry->second.value_.type_int32;
  int offset = offset_entry->second.value_.type_int32;
  int status = status_entry->second.value_.type_int32;

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->sendResponse(serverId, deviceAddress, requestId, status, offset, value);

  return true;
}

bool GattServerManager::onSendNotification(int32_t serverId, const std::string &deviceAddress, SdBus::ArgumentMap &args, uint8_t *value) {
  ALOGI(LOGTAG " %s", __func__);

  auto attrId_entry = args.find("attrId");
  auto confirm_entry = args.find("confirm");

  if (attrId_entry == args.end() || confirm_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int attrId = attrId_entry->second.value_.type_int32;
  int confirm = confirm_entry->second.value_.type_bool;

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->sendNotification(serverId, deviceAddress, attrId, confirm, value);
  return true;
}

int GattServerManager::sd_UnregisterServer(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattServerManager *gatt_server_manager_ptr = static_cast<GattServerManager *>(userdata);

  int32_t serverId;

  int res = sd_bus_message_read(m, "i", &serverId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  if (gatt_server_manager_ptr != nullptr) {
    if (!gatt_server_manager_ptr->onUnregisterServer((int)serverId)) {
      return sd_bus_reply_method_return(m, NULL);
    }
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattServerManager::onUnregisterServer(int32_t serverId) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->unregisterServer(serverId);
  return true;
}
}  // namespace ipc
}  // namespace gatt
