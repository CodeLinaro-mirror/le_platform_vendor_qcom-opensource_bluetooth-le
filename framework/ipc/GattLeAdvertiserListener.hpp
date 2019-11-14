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

#ifndef GATTLEADVERTISERLISTENER_HPP
#define GATTLEADVERTISERLISTENER_HPP

#include <cstdint>
#include <systemdq/sd-bus.h>

namespace gatt {
namespace ipc {
class GattDBusFw;

class GattLeAdvertiserListener final {
  public:
    explicit GattLeAdvertiserListener(GattDBusFw * dbus_service);
    ~GattLeAdvertiserListener();

    bool InitialiseDBusService();

  private:
    static int sd_AdvertisingSetStarted(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_AdvertisingSetStopped(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_OwnAddressRead(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_AdvertisingEnabled(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_AdvertisingDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_ScanResponseDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_AdvertisingParametersUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_PeriodicAdvertisingParametersUpdated(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_PeriodicAdvertisingDataSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_PeriodicAdvertisingEnabled(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

    GattDBusFw * gatt_dbus_service_;
    sd_bus_slot *sdbusSlot_ = nullptr;
};
} // namespace ipc
} // namespace gatt
#endif  // GATTLEADVERTISERLISTENER_HPP
