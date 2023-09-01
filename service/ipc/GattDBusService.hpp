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

#ifndef GATTSERVERDBUSSRV_H
#define GATTSERVERDBUSSRV_H

#include <systemd/sd-bus.h>
#include <thread>
#include <memory>
#include "GattDBusDefines.hpp"
#include "GattServerManager.hpp"
#include "GattClientManager.hpp"
#include "GattLeAdvertiserManager.hpp"
#include "GattLeScannerManager.hpp"
#include "GattServerListenerClient.hpp"
#include "GattClientListenerClient.hpp"
#include "GattLeAdvertising.hpp"
#include "GattLeScanner.hpp"
#include "GattLibService.hpp"

using gatt::GattLibService;
namespace gatt {
namespace ipc {
class GattDBusService {
  public:
    ~GattDBusService();

  static std::shared_ptr<GattDBusService> getGattDBusProxy(GattLibService *gatt_lib_service_ptr);

  static std::shared_ptr<GattDBusService> getGattDBusProxy() {
    return GattDBusService::gattDBusProxy_;
  }

    sd_event * GetEventLoop() { return g_eventLoop_; }
    sd_bus * GetSdbus() { return g_sdbus_; }
    GattLibService * GetGattLibService() { return gatt_lib_service_; }

    GattServerManager* gattServerManager() {
      return gattServerManager_.get();
    }
    GattServerListenerClient* gattServerListenerClient() {
      return gattServerListenerClient_.get();
    }
    GattClientManager* gattClientManager() {
      return gattClientManager_.get();
    }
    GattClientListenerClient* gattClientListenerClient() {
      return gattClientListenerClient_.get();
    }
    GattLeAdvertiserManager* gattLeAdvertiserManager() {
      return gattLeAdvertiserManager_.get();
    }
    GattLeAdvertising* gattLeAdvertising() {
      return gattLeAdvertising_.get();
    }
    GattLeScannerManager* gattLeScannerManager() {
      return gattLeScannerManager_.get();
    }
    GattLeScanner* gattLeScanner() {
      return gattLeScanner_.get();
    }

    bool Shutdown();

  private:
    explicit GattDBusService(GattLibService *gatt_lib_service_ptr);
    bool InitialiseSdbus();
    bool StartSdbusWorkerThread();
    static int CallbackEventHandler(sd_event_source *s, int fd, uint32_t revents, void *userdata);

    sd_bus *g_sdbus_ = nullptr;
    sd_event *g_eventLoop_ = nullptr;
    int callbackFd_ = -1;
    std::thread sd_bus_worker_thread_;

    GattLibService *gatt_lib_service_;

    std::unique_ptr<GattLeAdvertiserManager> gattLeAdvertiserManager_;
    std::unique_ptr<GattLeAdvertising> gattLeAdvertising_;
    std::unique_ptr<GattServerManager> gattServerManager_;
    std::unique_ptr<GattServerListenerClient> gattServerListenerClient_;
    std::unique_ptr<GattClientManager> gattClientManager_;
    std::unique_ptr<GattClientListenerClient> gattClientListenerClient_;
    std::unique_ptr<GattLeScannerManager> gattLeScannerManager_;
    std::unique_ptr<GattLeScanner> gattLeScanner_;

    static std::shared_ptr<GattDBusService> gattDBusProxy_;

};
} // namespace ipc
} // namespace gatt
#endif  // GATTSERVERDBUSSRV_H
