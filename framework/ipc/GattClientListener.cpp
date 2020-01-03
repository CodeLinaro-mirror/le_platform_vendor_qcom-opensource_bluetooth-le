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

#include "GattClientListener.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusFw.hpp"
#include "GattServer.hpp"
#include "GattServer.hpp"

#define LOGTAG "GattClientListener"
namespace gatt {
namespace ipc {
GattClientListener::GattClientListener(GattDBusFw *dbus_service)
    : dbus_service_(dbus_service) {
}

bool GattClientListener::InitialiseDBusService() {
  ALOGI(LOGTAG " %s", __func__);

  if (!dbus_service_->GetSdbus()) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  static const sd_bus_vtable vtable[] = {
      SD_BUS_VTABLE_START(0),
      SD_BUS_METHOD("ClientRegistered", "ii", nullptr, GattClientListener::sd_ClientRegistered,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("ConnnectionState", "sa{sv}", nullptr, GattClientListener::sd_ConnnectionState,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("SearchComplete", "savi", nullptr, GattClientListener::sd_SearchComplete,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("CharacteristicRead", "siiay", nullptr, GattClientListener::sd_CharacteristicRead,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("CharacteristicWrite", "sii", nullptr, GattClientListener::sd_CharacteristicWrite,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("DescriptorRead", "siiay", nullptr, GattClientListener::sd_DescriptorRead,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("DescriptorWrite", "sii", nullptr, GattClientListener::sd_DescriptorWrite,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_METHOD("Notify", "siay", nullptr, GattClientListener::sd_Notify,
          SD_BUS_VTABLE_METHOD_NO_REPLY),
      SD_BUS_VTABLE_END};

  int res = sd_bus_add_object_vtable(dbus_service_->GetSdbus(), nullptr, GATT_CLIENT_LISTENER_OBJECT_PATH,
      GATT_CLIENT_LISTENER_INTERFACE, vtable, this);
  if (res < 0) {
    ALOGE(LOGTAG "interface init failed on path %s: %d - %s\n",
        GATT_CLIENT_LISTENER_OBJECT_PATH, -res, strerror(-res));

    sd_bus_emit_object_removed(dbus_service_->GetSdbus(), GATT_CLIENT_LISTENER_OBJECT_PATH);
    return false;
  }

  res = sd_bus_emit_object_added(dbus_service_->GetSdbus(), GATT_CLIENT_LISTENER_OBJECT_PATH);
  if (res < 0) {
    ALOGE(LOGTAG
        "object manager failed to signal new path %s: %d - %s\n",
        GATT_CLIENT_LISTENER_OBJECT_PATH, -res, strerror(-res));
    return false;
  }

  return true;
}

int GattClientListener::sd_ClientRegistered(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);
  // Send null reply response

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  int result;
  int32_t status = 0;
  int32_t clientId = 0;
  result = sd_bus_message_read(m, "ii", &status, &clientId);

  if (result < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-result));
    return result;
  }
  ALOGI(LOGTAG " %s -> status:%d, clientId:%d", __func__, status, clientId);

  gatt_client_listener_ptr->onClientRegistered(status, clientId);

  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_ConnnectionState(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

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
    ALOGE(LOGTAG "Failed to parse Arguments from message: %s\n", strerror(-res));
    return res;
  }

  if (!gatt_client_listener_ptr->onConnnectionState(std::string(deviceAddress), args)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_SearchComplete(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  std::vector<GattService *> services;
  int32_t status;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  res = SdBus::getServiceListFromMessage(m, &services);
  if (res < 0) {
    ALOGE(LOGTAG "Failed to parse ServiceList from message: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &status);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  if (!gatt_client_listener_ptr->onSearchComplete(std::string(deviceAddress), services, (int)status)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_CharacteristicRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  int32_t status;
  int32_t handle;
  uint8_t *value_array;
  size_t array_size;

  int res;
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &status);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }

  if (!gatt_client_listener_ptr->onCharacteristicRead(std::string(deviceAddress), (int)status, (int)handle, value_array, array_size)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_CharacteristicWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  int32_t status;
  int32_t handle;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &status);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  if (!gatt_client_listener_ptr->onCharacteristicWrite(std::string(deviceAddress), (int)status, (int)handle)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_DescriptorRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  int32_t status;
  int32_t handle;
  uint8_t *value_array;
  size_t array_size;

  int res;
  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &status);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }

  if (!gatt_client_listener_ptr->onDescriptorRead(std::string(deviceAddress), (int)status, (int)handle, value_array, array_size)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_DescriptorWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  int32_t status;
  int32_t handle;
  int res;

  res = sd_bus_message_read(m, "s", &deviceAddress);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &status);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }
  res = sd_bus_message_read(m, "i", &handle);
  if (res < 0) {
    fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-res));
    return res;
  }

  if (!gatt_client_listener_ptr->onDescriptorWrite(std::string(deviceAddress), (int)status, (int)handle)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

int GattClientListener::sd_Notify(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  ALOGI(LOGTAG " %s", __func__);

  if (userdata == nullptr) {
    ALOGE(LOGTAG " %s -> userdata pointer is null", __func__);
    return -1;
  }
  // Get the server listener object from the userdata
  GattClientListener *gatt_client_listener_ptr = static_cast<GattClientListener *>(userdata);

  const char *deviceAddress;
  int32_t handle;
  uint8_t *value_array;
  size_t array_size;

  int res;
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
  res = sd_bus_message_read_array(m, 'y', &value_array, &array_size);
  if (res < 0) {
    return res;
  }

  if (!gatt_client_listener_ptr->onNotify(std::string(deviceAddress), (int)handle, value_array, array_size)) {
    ALOGE(LOGTAG "Failed to call Register Client");
    return sd_bus_reply_method_return(m, NULL);
  }
  return sd_bus_reply_method_return(m, NULL);
}

bool GattClientListener::onClientRegistered(int32_t status, int32_t clientId) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onClientRegistered(status, clientId);
  return true;
}

bool GattClientListener::onConnnectionState(const std::string& deviceAddress, SdBus::ArgumentMap &args) {
  ALOGI(LOGTAG " %s", __func__);

  auto status_entry = args.find("status");
  auto clientId_entry = args.find("clientId");
  auto connected_entry = args.find("connected");

  if (status_entry == args.end() || clientId_entry == args.end() || connected_entry == args.end()) {
    ALOGE(LOGTAG "%s -> Args not passed correctly", __func__);
    return false;
  }
  int status = status_entry->second.value_.type_int32;
  int clientId = clientId_entry->second.value_.type_int32;
  bool connected = connected_entry->second.value_.type_bool;

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onConnectionState(status, clientId, connected, deviceAddress);
  return true;
}

bool GattClientListener::onSearchComplete(const std::string& deviceAddress, std::vector<gatt::GattService *> services, int32_t status) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onSearchComplete(deviceAddress, services, status);
  return true;
}

bool GattClientListener::onCharacteristicRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onCharacteristicRead(address, status, handle, value, length);
  return true;
}

bool GattClientListener::onCharacteristicWrite(const std::string& address, int32_t status, int32_t handle) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onCharacteristicWrite(address, status, handle);
  return true;
}

bool GattClientListener::onDescriptorRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onDescriptorRead(address, status, handle, value, length);
  return true;
}

bool GattClientListener::onDescriptorWrite(const std::string& address, int32_t status, int32_t handle) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onDescriptorWrite(address, status, handle);
  return true;
}

bool GattClientListener::onNotify(const std::string& address, int32_t handle, uint8_t *value, int length) {
  ALOGI(LOGTAG " %s", __func__);

  if (dbus_service_->getGattClient() == nullptr) {
    ALOGE(LOGTAG " %s -> No valid registered gatt client", __func__);
    return false;
  }
  dbus_service_->getGattClient()->onNotify(address, handle, value, length);
  return true;
}
}  // namespace ipc
}  // namespace gatt
