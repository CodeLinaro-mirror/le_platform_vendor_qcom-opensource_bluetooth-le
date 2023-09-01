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

#ifndef GATTLEADVERTISERMANAGER_HPP
#define GATTLEADVERTISERMANAGER_HPP

#include <systemd/sd-bus.h>
#include <string>

namespace gatt {
namespace ipc {
class GattDBusService;

class GattLeAdvertiserManager final {
  public:
  explicit GattLeAdvertiserManager(GattDBusService *dbus_service);
  ~GattLeAdvertiserManager();

    bool InitialiseDBusService();
    int UpdateLocalLeFeaturesProperties();
    int UpdateDeviceNameProperty(const std::string& name);

  private:
    static int sd_StartAdvertisingSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_StopAdvertisingSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_GetOwnAddress(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_EnableAdvertisingSet(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetAdvertisingData(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetScanResponseData(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetAdvertisingParameters(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetPeriodicAdvertisingParameters(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetPeriodicAdvertisingData(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_SetPeriodicAdvertisingEnable(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
    static int sd_isLeCodedPhySupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);
    static int sd_isLe2MPhySupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);
    static int sd_isLePeriodicAdvertisingSupported(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);
    static int sd_getLeMaximumAdvertisingDataLength(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);
    static int sd_getDeviceName(sd_bus *bus, const char *path, const char *interface, const char *property,
                                     sd_bus_message *reply, void *userdata, sd_bus_error *ret_error);

  GattDBusService *gatt_dbus_service_;
  sd_bus_slot *sdbusSlot_ = nullptr;

  std::string bdName_;
};
} // namespace ipc
} // namespace gatt

#endif  // GATTLEADVERTISERMANAGER_HPP
