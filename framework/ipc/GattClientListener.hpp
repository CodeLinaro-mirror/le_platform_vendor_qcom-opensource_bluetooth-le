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

#ifndef GATTCLIENTLISTENER_H
#define GATTCLIENTLISTENER_H

#include <systemd/sd-bus.h>
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
