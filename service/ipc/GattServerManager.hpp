/*
Copyright (c) 2019, The Linux Foundation. All rights reserved.

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

#ifndef GATT_SERVER_MANAGER_H
#define GATT_SERVER_MANAGER_H

#include <systemd/sd-bus.h>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include "BluetoothLeSdbus.hpp"
#include "GattService.hpp"
#include "uuid.h"


using gatt::GattService;
namespace gatt {
namespace ipc {
class GattDBusService;

class GattServerManager {
  public:
  GattServerManager(GattDBusService *dbus_service);

    bool InitialiseDBusService();

  private:
  GattDBusService *gatt_dbus_service_;

    bool StartSdbusWorkerThread();
    static int sd_RegisterServer(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_UnregisterServer(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_AddService(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SendResponse(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SendNotification(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

    bool onRegisterServer(const std::string& uuid, const std::string& gattServerListener_sender);
    bool onUnregisterServer(int32_t serverId);
    bool onAddService(int32_t serverId, GattService *gatt_service);
    bool onSendResponse(int32_t serverId, const std::string& deviceAddress, SdBus::ArgumentMap &args, uint8_t *value, size_t length);
    bool onSendNotification(int32_t serverId, const std::string& deviceAddress, SdBus::ArgumentMap &args, uint8_t *value, size_t length);

};
} // namespace ipc
} // namespace gatt
#endif  // GATT_SERVER_MANAGER_H
