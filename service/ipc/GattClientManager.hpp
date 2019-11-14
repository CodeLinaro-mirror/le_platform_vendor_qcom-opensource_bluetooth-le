#ifndef GATT_CLIENT_MANAGER_H
#define GATT_CLIENT_MANAGER_H

#include <systemdq/sd-bus.h>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include "BluetoothLeSdbus.hpp"
#include "BluetoothLeSdbus.hpp"
#include "GattCharacteristic.hpp"
#include "GattClientListenerClient.hpp"
#include "GattDescriptor.hpp"
#include "GattLibService.hpp"
#include "GattService.hpp"

#include "utils/include/uuid.h"

namespace gatt {
namespace ipc {
class GattDBusService;

class GattClientManager {
  public:
  GattClientManager(GattDBusService *dbus_service);
  bool Shutdown();

  bool InitialiseDBusService() { return InitialiseSdbus(); }

  private:
  GattDBusService *gatt_dbus_service_;

  bool InitialiseSdbus();
  static int sd_RegisterClient(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ConnectClient(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_DiscoverServices(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ReadCharacteristic(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_WriteCharacteristic(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ReadDescriptor(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_WriteDescriptor(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_RegisterForNotification(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

  bool onRegisterClient(const std::string& uuid, const std::string& client_listener_sender);
  bool onConnectClient(int32_t clientId, const std::string& deviceAddress, SdBus::ArgumentMap &args);
  bool onDiscoverServices(int32_t clientId, const std::string& deviceAddress);
  bool onReadCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq);
  bool onWriteCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t writeType,
      int32_t authReq, uint8_t *value, int length);
  bool onReadDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq);
  bool onWriteDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq,
      uint8_t *value, int length);
  bool onRegisterForNotification(int32_t clientIf, const std::string& address, int32_t handle,
      bool enable);
};
}  // namespace ipc
}  // namespace gatt
#endif  // GATT_CLIENT_MANAGER_H
