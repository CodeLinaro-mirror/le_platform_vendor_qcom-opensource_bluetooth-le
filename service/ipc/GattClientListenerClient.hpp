
#ifndef GATT_CLIENT_LISTENER_CLIENT_HPP
#define GATT_CLIENT_LISTENER_CLIENT_HPP

#include <systemdq/sd-bus.h>
#include <string>
#include "GattService.hpp"
#include "IClientCallback.hpp"

namespace gatt {
namespace ipc {
class GattDBusService;

class GattClientListenerClient {
  public:
  GattClientListenerClient(GattDBusService *dbus_service);

  void onClientRegistered(int status, int clientIf, const std::string& service_name);

  // Methods not implemented
  // TODO: Implement
  void onConnectionState(int32_t status, int32_t clientIf, bool connected, const std::string&  address, const std::string& service_name);
  void onPhyUpdate(const std::string&  address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name);
  void onPhyRead(const std::string&  address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name);
  // TODO: Implement
  void onSearchComplete(const std::string&  address, std::vector<GattService *> services, int32_t status, const std::string& service_name);
  // TODO: Implement
  void onCharacteristicRead(const std::string&  address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name);
  // TODO: Implement
  void onCharacteristicWrite(const std::string&  address, int32_t status, int32_t handle, const std::string& service_name);
  // TODO: Implement
  void onExecuteWrite(const std::string&  address, int32_t status, const std::string& service_name);
  // TODO: Implement
  void onDescriptorRead(const std::string&  address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name);
  // TODO: Implement
  void onDescriptorWrite(const std::string&  address, int32_t status, int32_t handle, const std::string& service_name);
  // TODO: Implement
  void onNotify(const std::string&  address, int32_t handle, uint8_t *value, int length, const std::string& service_name);
  void onReadRemoteRssi(const std::string&  address, int32_t rssi, int32_t status, const std::string& service_name);
  void onConfigureMTU(const std::string&  address, int32_t mtu, int32_t status, const std::string& service_name);
  void onConnectionUpdated(const std::string&  address, int32_t interval, int32_t latency,
      int32_t timeout, int32_t status, const std::string& service_name);

  private:
  GattDBusService *dbus_service_;
};
}  // namespace ipc
}  // namespace gatt
#endif  //GATT_SERVER_LISTENER_CLIENT_HPP
