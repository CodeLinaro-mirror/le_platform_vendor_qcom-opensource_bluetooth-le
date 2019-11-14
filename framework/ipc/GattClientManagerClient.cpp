#include "GattClientManagerClient.hpp"
#include <cstdint>
#include "BluetoothLeSdbus.hpp"
#include "GattDBusFw.hpp"

#define LOGTAG "GattClientManagerClient"

namespace gatt {
namespace ipc {
GattClientManagerClient::GattClientManagerClient(GattDBusFw *dbus_service)
    : dbus_service_(dbus_service) {
}

void GattClientManagerClient::registerClient(Uuid uuid) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattClientListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "RegisterClient",
      nullptr,
      nullptr,
      "s",
      uuid.ToString().c_str());
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattClientManagerClient::unregisterClient(int32_t clientIf) {
}

// TODO: Implement
void GattClientManagerClient::clientConnect(int32_t clientIf, const std::string& address, bool isDirect, int32_t transport, bool opportunistic, int32_t phy) {
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
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "ConnectClient");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the serverId argument
  res = sd_bus_message_append(m, "i", clientIf);
  // Add the address argument
  res = sd_bus_message_append(m, "s", address.c_str());

  // Create arguments map/
  SdBus::ArgumentMap args;
  SdBus::SdbusArgument isDirect_arg;
  isDirect_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  isDirect_arg.value_.type_bool = isDirect;
  args["isDirect"] = isDirect_arg;

  SdBus::SdbusArgument transport_arg;
  transport_arg.type_ = SdBus::SdbusArgument::INT32;
  transport_arg.value_.type_int32 = transport;
  args["transport"] = transport_arg;

  SdBus::SdbusArgument initiating_phys_arg;
  initiating_phys_arg.type_ = SdBus::SdbusArgument::INT32;
  initiating_phys_arg.value_.type_int32 = phy;
  args["initiating_phys"] = initiating_phys_arg;

  SdBus::SdbusArgument opportunistic_arg;
  opportunistic_arg.type_ = SdBus::SdbusArgument::BOOLEAN;
  opportunistic_arg.value_.type_bool = opportunistic;
  args["opportunistic"] = opportunistic_arg;

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
// TODO: Implement
void GattClientManagerClient::clientDisconnect(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::clientSetPreferredPhy(int32_t clientIf, const std::string& address, int32_t txPhy, int32_t rxPhy, int32_t phyOptions) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::clientReadPhy(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::refreshDevice(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}
// TODO: Implement
void GattClientManagerClient::discoverServices(int32_t clientIf, const std::string& address) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattClientListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "DiscoverServices",
      nullptr,
      nullptr,
      "is",
      clientIf,
      address.c_str());
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}
// TODO: Implement
void GattClientManagerClient::discoverServiceByUuid(int32_t clientIf, const std::string& address, Uuid uuid) {
  ALOGW(LOGTAG " %s", __func__);
}
// TODO: Implement
void GattClientManagerClient::readCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattClientListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "ReadCharacteristic",
      nullptr,
      nullptr,
      "isii",
      clientIf,
      address.c_str(),
      handle,
      authReq);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}
// TODO: Implement
void GattClientManagerClient::readUsingCharacteristicUuid(int32_t clientIf, const std::string& address, Uuid uuid, int32_t startHandle, int32_t endHandle, int32_t authReq) {
  ALOGW(LOGTAG " %s", __func__);
}
// TODO: Implement
void GattClientManagerClient::writeCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t writeType, int32_t authReq, uint8_t *value, int length) {
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
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "WriteCharacteristic");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the clientIf argument
  res = sd_bus_message_append(m, "i", clientIf);
  // Add the address argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Add the handle argument
  res = sd_bus_message_append(m, "i", handle);
  // Add the writeType argument
  res = sd_bus_message_append(m, "i", writeType);
  // Add the authReq argument
  res = sd_bus_message_append(m, "i", authReq);

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}
// TODO: Implement
void GattClientManagerClient::readDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattClientListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "ReadDescriptor",
      nullptr,
      nullptr,
      "isii",
      clientIf,
      address.c_str(),
      handle,
      authReq);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}
// TODO: Implement
void GattClientManagerClient::writeDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq, uint8_t *value, int length) {
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
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "WriteDescriptor");
  if (res < 0) {
    ALOGE(LOGTAG " Error creating new method call: %d - %s\n", -res, strerror(-res));
    return;
  }
  // Add the clientIf argument
  res = sd_bus_message_append(m, "i", clientIf);
  // Add the address argument
  res = sd_bus_message_append(m, "s", address.c_str());
  // Add the handle argument
  res = sd_bus_message_append(m, "i", handle);
  // Add the authReq argument
  res = sd_bus_message_append(m, "i", authReq);

  res = sd_bus_message_append_array(m, 'y', value, length);

  res = sd_bus_call_async(dbus_service_->GetSdbus(), nullptr, m, nullptr, nullptr, 0);

  if (res < 0) {
    ALOGE(LOGTAG " Error calling Method: %d - %s\n", -res, strerror(-res));
    return;
  }
}
// TODO: Implement
void GattClientManagerClient::beginReliableWrite(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}
// TODO: Implement
void GattClientManagerClient::endReliableWrite(int32_t clientIf, const std::string& address, bool execute) {
  ALOGW(LOGTAG " %s", __func__);
}

// TODO int32_t change for all methods.
void GattClientManagerClient::registerForNotification(int32_t clientIf, const std::string& address, int32_t handle, bool enable) {
  ALOGI(LOGTAG " %s", __func__);
  if (dbus_service_->GetSdbus() == nullptr) {
    ALOGE(LOGTAG " sd_bus object not valid");
    return;
  }
  int res;

  if (dbus_service_->gattClientListener() == nullptr) {
    ALOGE(LOGTAG " listener object not valid");
    return;
  }
  res = sd_bus_call_method_async(dbus_service_->GetSdbus(),
      nullptr,
      GATT_MANAGER_SERVICE_NAME,
      GATT_CLIENT_MANAGER_OBJECT_PATH,
      GATT_CLIENT_MANAGER_INTERFACE,
      "RegisterForNotification",
      nullptr,
      nullptr,
      "isib",
      clientIf,
      address.c_str(),
      handle,
      enable ? 1 : 0);
  ALOGD(LOGTAG " %s -> sd_bus_call_method Called RES: %d", __func__, res);
}

void GattClientManagerClient::readRemoteRssi(int32_t clientIf, const std::string& address) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::configureMTU(int32_t clientIf, const std::string& address, int32_t mtu) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::connectionParameterUpdate(int32_t clientIf, const std::string& address, int32_t connectionPriority) {
  ALOGW(LOGTAG " %s", __func__);
}

void GattClientManagerClient::leConnectionUpdate(int32_t clientIf, const std::string& address, int32_t minConnectionInterval, int32_t maxConnectionInterval, int32_t slaveLatency, int32_t supervisionTimeout, int32_t minConnectionEventLen, int32_t maxConnectionEventLen) {
  ALOGW(LOGTAG " %s", __func__);
}

}  // namespace ipc
}  // namespace gatt
