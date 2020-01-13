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

#ifndef GATTSERVERLISTENER_H
#define GATTSERVERLISTENER_H

#include <systemdq/sd-bus.h>
#include <map>
#include <memory>
#include <string>
#include <cstdint>
#include "GattServer.hpp"
#include "GattService.hpp"
#include "IServerCallback.hpp"
#include "uuid.h"
#include "BluetoothLeSdbus.hpp"

#define LOGTAG "GattServerListener"

namespace gatt {
namespace ipc {
class GattDBusFw;

class GattServerListener {
  public:
  GattServerListener(GattDBusFw *dbus_service);

  bool InitialiseDBusService();

  private:
  GattDBusFw *dbus_service_;

  static int sd_ServerRegistered(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ServiceAdded(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ConnnectionState(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_CharacteristicReadRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_DescriptorReadRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_CharacteristicWriteRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_DescriptorWriteRequest(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_NotificationSent(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ExecuteWrite(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_MtuChanged(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  static int sd_ConnectionUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

  bool onServerRegistered(int32_t status, int32_t serverId);
  bool onServiceAdded(int32_t status, GattService* gatt_service);
  bool onConnnectionState(const std::string& deviceAddress, SdBus::ArgumentMap& args);
  bool onCharacteristicReadRequest(const std::string& deviceAddress, SdBus::ArgumentMap& args);
  bool onDescriptorReadRequest(const std::string& deviceAddress, SdBus::ArgumentMap& args);
  bool onCharacteristicWriteRequest(const std::string& deviceAddress, SdBus::ArgumentMap& args, uint8_t *value);
  bool onDescriptorWriteRequest(const std::string& deviceAddress, SdBus::ArgumentMap& args, uint8_t *value);
  bool onNotificationSent(const std::string& deviceAddress, int32_t status);
  bool onExecuteWrite(const std::string& deviceAddress, SdBus::ArgumentMap& args);
  bool onMtuChanged(const std::string& deviceAddress, int32_t mtu);
  bool onConnectionUpdated(const std::string& deviceAddress, SdBus::ArgumentMap& args);

};
} // namespace ipc
} // namespace gatt
#endif  // GATTSERVERLISTENER_H
