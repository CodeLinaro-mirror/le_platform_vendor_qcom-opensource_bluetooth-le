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

#ifndef GATT_CLIENT_MANAGER_H
#define GATT_CLIENT_MANAGER_H

#include <systemd/sd-bus.h>
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
