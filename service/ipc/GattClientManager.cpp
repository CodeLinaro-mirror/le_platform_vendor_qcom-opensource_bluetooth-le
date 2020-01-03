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

#include "GattClientManager.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusService.hpp"

#define LOGTAG "GattClientManager"
using bt::Uuid;
namespace gatt {
namespace ipc {

GattClientManager::GattClientManager(GattDBusService *dbus_service)
    : gatt_dbus_service_(dbus_service) {
}
bool GattClientManager::InitialiseSdbus() {
  ALOGI(LOGTAG " %s", __func__);

  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("RegisterClient", "s", nullptr, GattClientManager::sd_RegisterClient,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ConnectClient", "isa{sv}", nullptr, GattClientManager::sd_ConnectClient,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("DiscoverServices", "is", nullptr, GattClientManager::sd_DiscoverServices,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ReadCharacteristic", "isii", nullptr, GattClientManager::sd_ReadCharacteristic,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("WriteCharacteristic", "isiiiay", nullptr, GattClientManager::sd_WriteCharacteristic,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ReadDescriptor", "isii", nullptr, GattClientManager::sd_ReadDescriptor,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("WriteDescriptor", "isiiay", nullptr, GattClientManager::sd_WriteDescriptor,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("RegisterForNotification", "isib", nullptr, GattClientManager::sd_RegisterForNotification,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_VTABLE_END};
  int res = sd_bus_add_object_vtable(gatt_dbus_service_->GetSdbus(), nullptr, GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE, vtable, this);

  if (res < 0) {
    ALOGI(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_CLIENT_MANAGER_OBJECT_PATH, -res, strerror(-res));
    sd_bus_emit_object_removed(gatt_dbus_service_->GetSdbus(), GATT_CLIENT_MANAGER_OBJECT_PATH);
    return false;
  }
  res = sd_bus_emit_object_added(gatt_dbus_service_->GetSdbus(), GATT_CLIENT_MANAGER_OBJECT_PATH);
  if (res < 0) {
    ALOGI(LOGTAG
        "object manager failed to signal new path %s: %d - %s\n",
        GATT_CLIENT_MANAGER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  return true;
}
int GattClientManager::sd_RegisterClient(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  const char *sender = sd_bus_message_get_sender(m);
  const char *uuid;
  int res;

  res = sd_bus_message_read(m, "s", &uuid);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG "uuid:%s, sender:%s", uuid, sender);

  if (!gatt_client_manager_ptr->onRegisterClient(std::string(uuid), std::string(sender))) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_ConnectClient(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  const char *deviceAddress;
  int32_t clientId;
  int res;

  res = sd_bus_message_read(m, "is", &clientId, &deviceAddress);
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

  if (!gatt_client_manager_ptr->onConnectClient(clientId, std::string(deviceAddress), args)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_DiscoverServices(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  const char *address;
  int32_t clientId;
  int res;

  res = sd_bus_message_read(m, "is", &clientId, &address);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, address:%s", clientId, address);

  if (!gatt_client_manager_ptr->onDiscoverServices(clientId, std::string(address))) {
    ALOGE(LOGTAG "Failed to call Discover Services");
    return sd_bus_reply_method_return(m, NULL);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_ReadCharacteristic(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  int32_t clientId;
  const char *deviceAddress;
  int32_t handle;
  int32_t authReq;
  int res;

  res = sd_bus_message_read(m, "i", &clientId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  res = sd_bus_message_read(m, "i", &authReq);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, deviceAddress:%s, handle:%d, authReq:%d", clientId, deviceAddress, handle, authReq);

  if (!gatt_client_manager_ptr->onReadCharacteristic(clientId, std::string(deviceAddress), handle, authReq)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_WriteCharacteristic(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  int32_t clientId;
  const char *deviceAddress;
  int32_t handle;
  int32_t writeType;
  int32_t authReq;
  uint8_t *value_array;
  size_t array_size;

  int res;

  res = sd_bus_message_read(m, "i", &clientId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &writeType);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &authReq);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, deviceAddress:%s, handle:%d, authReq:%d", clientId, deviceAddress, handle, authReq);

  if (!gatt_client_manager_ptr->onWriteCharacteristic(clientId, std::string(deviceAddress), handle, writeType, authReq, value_array, array_size)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_ReadDescriptor(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  int32_t clientId;
  const char *deviceAddress;
  int32_t handle;
  int32_t authReq;
  int res;

  res = sd_bus_message_read(m, "i", &clientId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  res = sd_bus_message_read(m, "i", &authReq);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, deviceAddress:%s, handle:%d, authReq:%d", clientId, deviceAddress, handle, authReq);

  if (!gatt_client_manager_ptr->onReadDescriptor(clientId, std::string(deviceAddress), handle, authReq)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }

  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_WriteDescriptor(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  int32_t clientId;
  const char *deviceAddress;
  int32_t handle;
  int32_t authReq;
  uint8_t *value_array;
  size_t array_size;

  int res;

  res = sd_bus_message_read(m, "i", &clientId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &authReq);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, deviceAddress:%s, handle:%d, authReq:%d", clientId, deviceAddress, handle, authReq);

  if (!gatt_client_manager_ptr->onWriteDescriptor(clientId, std::string(deviceAddress), handle, authReq, value_array, array_size)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientManager::sd_RegisterForNotification(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientManager *gatt_client_manager_ptr = static_cast<GattClientManager *>(userdata);

  int32_t clientId;
  const char *deviceAddress;
  int32_t handle;
  int enable;
  int res;

  res = sd_bus_message_read(m, "i", &clientId);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "b", &enable);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  ALOGI(LOGTAG "clientId:%d, deviceAddress:%s, handle:%d, enable:%d", clientId, deviceAddress, handle, enable);

  if (!gatt_client_manager_ptr->onRegisterForNotification(clientId, std::string(deviceAddress), handle, (bool)enable)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }

  return sd_bus_reply_method_return(m, NULL);
}

bool GattClientManager::onRegisterClient(const std::string& uuid, const std::string& client_listener_sender) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  bool is_valid_uuid;
  auto register_uuid = Uuid::FromString(uuid, &is_valid_uuid);
  if (!is_valid_uuid) {
    ALOGE(LOGTAG " %s -> UUID is not valid", __func__);
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->registerClient(register_uuid, client_listener_sender);
  return true;
}

bool GattClientManager::onConnectClient(int32_t clientId, const std::string& deviceAddress, SdBus::ArgumentMap &args) {
  ALOGI(LOGTAG " %s", __func__);

  auto isDirect_entry = args.find("isDirect");
  auto transport_entry = args.find("transport");
  auto opportunistic_entry = args.find("opportunistic");
  auto initiating_phys_entry = args.find("initiating_phys");

  if (isDirect_entry == args.end() || transport_entry == args.end() || opportunistic_entry == args.end() || initiating_phys_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  bool isDirect = isDirect_entry->second.value_.type_bool;
  int transport = transport_entry->second.value_.type_int32;
  bool opportunistic = opportunistic_entry->second.value_.type_int32;
  int phy = initiating_phys_entry->second.value_.type_int32;

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->clientConnect(clientId, deviceAddress, isDirect, transport, opportunistic, phy);
  return true;
}

bool GattClientManager::onDiscoverServices(int32_t clientId, const std::string& deviceAddress) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->discoverServices(clientId, deviceAddress);
  return true;
}

bool GattClientManager::onReadCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->readCharacteristic(clientIf, address, handle, authReq);
  return true;
}

bool GattClientManager::onWriteCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t writeType, int32_t authReq, uint8_t *value, int length) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->writeCharacteristic(clientIf, address, handle, writeType, authReq, value, length);
  return true;
}

bool GattClientManager::onReadDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->readDescriptor(clientIf, address, handle, authReq);
  return true;
}

bool GattClientManager::onWriteDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq, uint8_t *value, int length) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->writeDescriptor(clientIf, address, handle, authReq, value, length);
  return true;
}

bool GattClientManager::onRegisterForNotification(int32_t clientIf, const std::string& address, int32_t handle, bool enable) {
  ALOGI(LOGTAG " %s", __func__);

  if (gatt_dbus_service_->GetGattLibService() == nullptr) {
    ALOGE(LOGTAG "no GattLibService registered");
    return false;
  }
  gatt_dbus_service_->GetGattLibService()->registerForNotification(clientIf, address, handle, enable);
  return true;
}

}  // namespace ipc
}  // namespace gatt
