#ifndef GATTCLIENTLISTENER_H
#define GATTCLIENTLISTENER_H

#include <systemdq/sd-bus.h>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include "BluetoothLeSdbus.hpp"
#include "GattClient.hpp"
#include "GattService.hpp"
#include "IClientCallback.hpp"
#include "utils/include/uuid.h"

#define LOGTAG "GattClientListener"

namespace gatt {
namespace ipc {
class GattDBusFw;

class GattClientListener {
  public:

  explicit GattClientListener(GattDBusFw *dbus_service);
  ~GattClientListener() = default;

  static std::unique_ptr<GattClientListener> getGattClientListener(GattDBusFw *dbus_service);

  bool InitialiseDBusService();

  private:
  GattDBusFw *dbus_service_ = nullptr;

  static int sd_ClientRegistered(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ConnnectionState(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_SearchComplete(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_CharacteristicRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_CharacteristicWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_DescriptorRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_DescriptorWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_Notify(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

  bool onClientRegistered(int32_t status, int32_t clientId);
  bool onConnnectionState(const std::string& deviceAddress, SdBus::ArgumentMap &args);
  bool onSearchComplete(const std::string& deviceAddress, std::vector<GattService *> services, int32_t status);
  bool onCharacteristicRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length);
  bool onCharacteristicWrite(const std::string& address, int32_t status, int32_t handle);
  bool onDescriptorRead(const std::string& address, int32_t status, int32_t handle, uint8_t *value, int length);
  bool onDescriptorWrite(const std::string& address, int32_t status, int32_t handle);
  bool onNotify(const std::string& address, int32_t handle, uint8_t *value, int length);
};

}  // namespace ipc
}  // namespace gatt

#endif  // GATTCLIENTLISTENER_H
