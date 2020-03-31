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

#include "GattDBusService.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include <mutex>

#define LOGTAG "GattDBusService"
namespace gatt {
namespace ipc {
std::shared_ptr<GattDBusService> GattDBusService::gattDBusProxy_ = nullptr;
static std::mutex srvProxyMtx_;

GattDBusService::GattDBusService(GattLibService *gatt_lib_service_ptr)
    : gatt_lib_service_(gatt_lib_service_ptr) {
}

GattDBusService::~GattDBusService() {
  ALOGI(LOGTAG "%s", __func__);
  int res = close(callbackFd_);
  ALOGI(LOGTAG " close: %d - %s\n", -res, strerror(-res));
  callbackFd_ = -1;
}

std::shared_ptr<GattDBusService> GattDBusService::getGattDBusProxy(
                                       GattLibService *gatt_lib_service_ptr) {
  ALOGI(LOGTAG " %s", __func__);

  std::lock_guard<std::mutex> srvProxyLock(srvProxyMtx_);
  
  if (GattDBusService::gattDBusProxy_ == nullptr) {
    struct make_shared_singleton : public GattDBusService {
          make_shared_singleton(GattLibService *gatt_lib_service_ptr) :
        GattDBusService(gatt_lib_service_ptr) {};
      };
    GattDBusService::gattDBusProxy_ =
	         std::make_shared<make_shared_singleton>(gatt_lib_service_ptr);

    if (GattDBusService::gattDBusProxy_ != nullptr) {
     GattDBusService::gattDBusProxy_->InitialiseSdbus();
    }
  }
  return GattDBusService::gattDBusProxy_;
}

bool GattDBusService::Shutdown() {
  ALOGI(LOGTAG " %s", __func__);

  if (callbackFd_ >= 0) {
    int res = eventfd_write(callbackFd_, 1ULL);
    ALOGI(LOGTAG " write: %d - %s\n", -res, strerror(-res));
  } else {
    ALOGI(LOGTAG " %s -> callbackFd_ < 0", __func__);
  }

  if (sd_bus_worker_thread_.joinable()) {
    sd_bus_worker_thread_.join();
    std::cout << "sd_bus_worker_thread_ JOINED!" << std::endl;
  }

  sd_bus_flush_close_unref(g_sdbus_);
  g_sdbus_ = nullptr;

  sd_event_unref(g_eventLoop_);
  g_eventLoop_ = nullptr;

  int res = close(callbackFd_);
  ALOGI(LOGTAG " close: %d - %s\n", -res, strerror(-res));
  callbackFd_ = -1;
  GattDBusService::gattDBusProxy_ = nullptr;
  return true;
}

bool GattDBusService::InitialiseSdbus() {
  ALOGI(LOGTAG " %s", __func__);
  // Open a Bus for the BLE Service
  int res = sd_bus_open_system(&g_sdbus_);
  if (res < 0) {
    ALOGE(LOGTAG " Unable open D-Bus: %d - %s\n", -res, strerror(-res));
    return false;
  }

  if (!g_sdbus_) {
    ALOGE(LOGTAG "Unable to get on D-Bus");
    return false;
  }

  res = sd_bus_request_name(g_sdbus_, GATT_MANAGER_SERVICE_NAME, 0);
  if (res < 0) {
    ALOGE(LOGTAG " Failed to get name: %d - %s\n", -res,
        strerror(-res));
    return false;
  }

  gattLeAdvertiserManager_ = make_unique<GattLeAdvertiserManager>(this);
  gattLeAdvertising_ = make_unique<GattLeAdvertising>(this);
  gattServerManager_ = make_unique<GattServerManager>(this);
  gattServerListenerClient_ = make_unique<GattServerListenerClient>(this);
  gattClientManager_ = make_unique<GattClientManager>(this);
  gattClientListenerClient_ = make_unique<GattClientListenerClient>(this);
  gattLeScannerManager_ = make_unique<GattLeScannerManager>(this);
  gattLeScanner_ = make_unique<GattLeScanner>(this);

  // Enable org.freedesktop.DBus.ObjectManager interface on root object path
  res = sd_bus_add_object_manager(g_sdbus_, nullptr, "/");
  if (res < 0) {
    ALOGE(LOGTAG
        "failed to add object manager on root object path: %d - %s\n",
        -res, strerror(-res));
    return false;
  }

  gattServerManager_->InitialiseDBusService();
  gattClientManager_->InitialiseDBusService();
  gattLeAdvertiserManager_->InitialiseDBusService();
  gattLeScannerManager_->InitialiseDBusService();

  res = StartSdbusWorkerThread();
  if (res < 0) {
    return false;
  }

  return true;
}

static int GattDBusService::CallbackEventHandler(sd_event_source *s, int fd,
                                                   uint32_t revents, void *userdata) {
  ALOGI(LOGTAG " %s", __func__);
  GattDBusService *serverDBusFw = static_cast<GattDBusService *>(userdata);
  if (serverDBusFw != nullptr) {
    sd_event_exit(serverDBusFw->GetEventLoop(), 0);
  }
  return 0;
}

bool GattDBusService::StartSdbusWorkerThread() {
  ALOGI(LOGTAG "%s", __func__);

  int res = sd_event_new(&g_eventLoop_);
  if (res < 0) {
    ALOGE(LOGTAG " Failed to get event loop: %d - %s\n", -res,
        strerror(-res));
    return false;
  }
  res = sd_bus_attach_event(g_sdbus_, g_eventLoop_, SD_EVENT_PRIORITY_NORMAL);
  if (res < 0) {
    ALOGE(LOGTAG " Failed to attach event loop: %d - %s\n", -res,
        strerror(-res));
    return false;
  }

  callbackFd_ = eventfd(0, 0);
  if (callbackFd_ == -1) {
    ALOGE(LOGTAG " Failed to create queue fd: %d - %s\n", errno,
        strerror(errno));
    return false;
  }

  res = sd_event_add_io(g_eventLoop_,
      nullptr,
      callbackFd_,
      EPOLLIN,
      GattDBusService::CallbackEventHandler,
      this);
  if (res < 0) {
    ALOGE(LOGTAG " Failed to install callback handler: %d - %s\n", -res,
        strerror(-res));
    return false;
  }

  ALOGI(LOGTAG " Starting sd_bus event loop thread");
  sd_bus_worker_thread_ = std::thread([this]() {
    int result = sd_event_loop(g_eventLoop_);
    if (result < 0) {
      ALOGE(LOGTAG " Failed to run event loop: %d - %s\n", -result,
          strerror(-result));
    }
  });
  return true;
}
} // namespace ipc
} // namespace gatt
