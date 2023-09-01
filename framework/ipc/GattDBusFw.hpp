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

#ifndef GATTDBUSFW_H
#define GATTDBUSFW_H

#include <systemd/sd-bus.h>
#include <thread>
#include <memory>

#include "GattServer.hpp"
#include "GattClient.hpp"
#include "GattLeAdvertiserListener.hpp"
#include "GattLeAdvertiserManagerClient.hpp"
#include "GattServerManagerClient.hpp"
#include "GattServerListener.hpp"
#include "GattLeScannerManagerClient.hpp"
#include "GattLeScannerListener.hpp"
#include "GattClientListener.hpp"
#include "GattClientManagerClient.hpp"
#include "GattDBusDefines.hpp"

class gatt::IAdvertisingSetCallback;
class gatt::IScannerCallback;

namespace gatt {
namespace ipc {
class GattDBusFw {
  public:

    static std::shared_ptr<GattDBusFw> GattDBusFw::getGattDBusProxy();

    void InitialiseGattServer(GattServer *gatt_server_ptr);
    void InitialiseGattClient(GattClient *gatt_client_ptr);
    void InitialiseLeAdvertiser();
    void InitialiseLeScanner();

    sd_event * GetEventLoop() { return g_eventLoop_; }
    sd_bus * GetSdbus() { return g_sdbus_; }

    GattServer * GetGattServer() { return gatt_server_; } // This should be sth like GetGattServer(string uuid);
    GattClient * getGattClient() { return gatt_client_; } //                 ,,

    GattLeAdvertiserManagerClient* gattLeAdvertiserManagerClient() {
      return gattLeAdvertiserManagerClient_.get();
    }
    GattServerListener* gattServerListener() {
      return gattServerListener_.get();
    }
    GattServerManagerClient* gattServerManagerClient() {
      return gattServerManagerClient_.get();
    }
    GattLeScannerManagerClient* gattLeScannerManagerClient() {
      return gattLeScannerManagerClient_.get();
    }
    GattClientManagerClient* gattClientManagerClient() {
      return gattClientManagerClient_.get();
    }
    GattClientListener* gattClientListener() {
      return gattGattClientListener_.get();
    }
    void SetAdvertisingSetCallback(gatt::IAdvertisingSetCallback *cb) { advertisingSetCb_ = cb; }
    gatt::IAdvertisingSetCallback* advertisingSetCallback() { return advertisingSetCb_; }
    void SetScannerCallback(gatt::IScannerCallback *cb) { scannerCb_ = cb; }
    gatt::IScannerCallback* scannerCallback() { return scannerCb_; }

    bool Shutdown();
    ~GattDBusFw();

  private:
    GattDBusFw::GattDBusFw() = default;
    bool InitialiseSdbus();

    sd_bus *g_sdbus_ = nullptr;
    sd_event *g_eventLoop_ = nullptr;
    int callbackFd_ = -1;
    std::thread sd_bus_worker_thread_;
    static int CallbackEventHandler(sd_event_source *s, int fd, uint32_t revents, void *userdata);
    bool StartSdbusWorkerThread();

    GattServer *gatt_server_;
    GattClient *gatt_client_;

    gatt::IAdvertisingSetCallback *advertisingSetCb_ = nullptr;
    gatt::IScannerCallback *scannerCb_ = nullptr;

    std::unique_ptr<GattLeAdvertiserManagerClient> gattLeAdvertiserManagerClient_;
    std::unique_ptr<GattLeAdvertiserListener> gattLeAdvertiserListener_;
    std::unique_ptr<GattServerListener> gattServerListener_;
    std::unique_ptr<GattServerManagerClient> gattServerManagerClient_;
    std::unique_ptr<GattLeScannerManagerClient> gattLeScannerManagerClient_;
    std::unique_ptr<GattLeScannerListener> gattLeScannerListener_;
    std::unique_ptr<GattClientListener> gattGattClientListener_;
    std::unique_ptr<GattClientManagerClient> gattClientManagerClient_;

    static std::shared_ptr<GattDBusFw> gattDBusProxy_;
};
} // namespace ipc
} // namespace gatt
#endif  // GATTDBUSFW_H
