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
