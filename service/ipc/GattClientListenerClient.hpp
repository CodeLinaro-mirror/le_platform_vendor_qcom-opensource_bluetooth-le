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
  void onConnectionState(int32_t status, int32_t clientIf, bool connected, const std::string&  address, const std::string& service_name);
  void onSearchComplete(const std::string&  address, std::vector<GattService *> services, int32_t status, const std::string& service_name);
  void onCharacteristicRead(const std::string&  address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name);
  void onCharacteristicWrite(const std::string&  address, int32_t status, int32_t handle, const std::string& service_name);
  void onDescriptorRead(const std::string&  address, int32_t status, int32_t handle, uint8_t *value, int length, const std::string& service_name);
  void onDescriptorWrite(const std::string&  address, int32_t status, int32_t handle, const std::string& service_name);
  void onNotify(const std::string&  address, int32_t handle, uint8_t *value, int length, const std::string& service_name);

  // Methods not implemented:
  void onPhyUpdate(const std::string&  address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name);
  void onPhyRead(const std::string&  address, int32_t txPhy, int32_t rxPhy, int32_t status, const std::string& service_name);
  void onExecuteWrite(const std::string&  address, int32_t status, const std::string& service_name);
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
