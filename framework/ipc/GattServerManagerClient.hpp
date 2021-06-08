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

#ifndef GATTSERVERMANAGERCLIENT_HPP
#define GATTSERVERMANAGERCLIENT_HPP

#include <systemdq/sd-bus.h>
#include <cstdint>
#include "GattService.hpp"
#include "IServerCallback.hpp"
#include "uuid.h"
#include <string>
using gatt::IServerCallback;
using gatt::GattService;
namespace gatt {
namespace ipc {
class GattDBusFw;

class GattServerManagerClient {
  public:
  GattServerManagerClient(GattDBusFw *dbus_service);

  void registerServer(Uuid uuid);
  void unregisterServer(int32_t serverIf);
  void serverConnect(int32_t serverIf, const std::string& address, bool isDirect, int32_t transport);
  void serverDisconnect(int32_t serverIf, const std::string& address);
  void serverSetPreferredPhy(int32_t serverIf, const std::string& address, int32_t txPhy, int32_t rxPhy,
      int32_t phyOptions);
  void serverReadPhy(int32_t clientIf, const std::string& address);
  void sendResponse(int32_t serverIf, const std::string& address, int32_t requestId, int32_t status,
      int32_t offset, uint8_t *value, size_t length);
  void sendNotification(int32_t serverIf, const std::string& address, int32_t handle, bool confirm,
      uint8_t *value, size_t length);
  void addService(int32_t serverIf, GattService *svc);
  void removeService(int32_t serverIf, int32_t handle);
  void clearServices(int32_t serverIf);

  private:

  GattDBusFw *dbus_service_;
};
} // namespace ipc
} // namespace gatt
#endif  // GATTSERVERMANAGERCLIENT_HPP
