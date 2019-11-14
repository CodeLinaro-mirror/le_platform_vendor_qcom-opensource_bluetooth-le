
#ifndef GATT_CLIENT_MANAGER_CLIENT_HPP
#define GATT_CLIENT_MANAGER_CLIENT_HPP

#include <systemdq/sd-bus.h>
#include <cstdint>
#include <string>
#include "utils/include/uuid.h"
#include "IClientCallback.hpp"

using gatt::IClientCallback;
namespace gatt {
namespace ipc {
class GattDBusFw;
class GattClientManagerClient {
  public:
  GattClientManagerClient(GattDBusFw *dbus_service);

  ~GattClientManagerClient() = default;


  void registerClient(Uuid uuid);
  void unregisterClient(int32_t clientIf);
  void clientConnect(int32_t clientIf, const std::string& address, bool isDirect, int32_t transport,
                         bool opportunistic, int32_t phy);
  void clientDisconnect(int32_t clientIf, const std::string& address);
  void clientSetPreferredPhy(int32_t clientIf, const std::string& address, int32_t txPhy,
                                    int32_t rxPhy, int32_t phyOptions);
  void clientReadPhy(int32_t clientIf, const std::string& address);
  void refreshDevice(int32_t clientIf, const std::string& address);
  void discoverServices(int32_t clientIf, const std::string& address);
  void discoverServiceByUuid(int32_t clientIf, const std::string& address, Uuid uuid);
  void readCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq);
  void readUsingCharacteristicUuid(int32_t clientIf, const std::string& address, Uuid uuid,
                                            int32_t startHandle, int32_t endHandle, int32_t authReq);
  void writeCharacteristic(int32_t clientIf, const std::string& address, int32_t handle, int32_t writeType,
                                 int32_t authReq, uint8_t *value, int length);
  void readDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq);
  void writeDescriptor(int32_t clientIf, const std::string& address, int32_t handle, int32_t authReq,
                           uint8_t *value, int length);
  void beginReliableWrite(int32_t clientIf, const std::string& address);
  void endReliableWrite(int32_t clientIf, const std::string& address, bool execute);
  void registerForNotification(int32_t clientIf, const std::string& address, int32_t handle,
                                       bool enable);
  void readRemoteRssi(int32_t clientIf, const std::string& address);
  void configureMTU(int32_t clientIf, const std::string& address, int32_t mtu);
  void connectionParameterUpdate(int32_t clientIf, const std::string& address,
                                         int32_t connectionPriority);
  void leConnectionUpdate(int32_t clientIf, const std::string& address,
                                int32_t minConnectionInterval, int32_t maxConnectionInterval,
                                int32_t slaveLatency, int32_t supervisionTimeout,
                                int32_t minConnectionEventLen, int32_t maxConnectionEventLen);

  private:
  GattDBusFw *dbus_service_ = nullptr;

};
} // namespace ipc
} // namespace gatt
#endif  //GATT_CLIENT_MANAGER_CLIENT_HPP
