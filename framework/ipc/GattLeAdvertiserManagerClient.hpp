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

#ifndef GATTLEADVERTISERMANAGERCLIENT_HPP
#define GATTLEADVERTISERMANAGERCLIENT_HPP

#include "AdvertiseData.hpp"
#include "AdvertisingSetParameters.hpp"
#include "PeriodicAdvertiseParameters.hpp"
#include "IAdvertisingSetCallback.hpp"
#include "IPeriodicAdvertisingCallback.hpp"
#include "ScanResult.hpp"
#include <systemd/sd-bus.h>
#include <string>
#include <mutex>

using std::string;
using namespace gatt;

namespace gatt {
namespace ipc {
class GattDBusFw;

class GattLeAdvertiserManagerClient final {
  public:
    explicit GattLeAdvertiserManagerClient(GattDBusFw * dbus_service);
    ~GattLeAdvertiserManagerClient() = default;

    // I/F for LE properties ...
    bool isLeCodedPhySupported();
    bool isLe2MPhySupported();
    bool isLePeriodicAdvertisingSupported();
    int getLeMaximumAdvertisingDataLength();
    string getDeviceName();
    int getLEParameters();

    static int leParamsProperty_cb(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

    // I/F for GattLeAdvertiser:
    bool startAdvertisingSet(gatt::AdvertisingSetParameters *parameters,
                             gatt::AdvertiseData *advertiseData,
                             gatt::AdvertiseData *scanResponse,
                             gatt::PeriodicAdvertiseParameters *periodicParameters,
                             gatt::AdvertiseData *periodicData,
                             int32_t duration,
                             int32_t maxExtAdvEvents,
                             IAdvertisingSetCallback* cb);
    bool stopAdvertisingSet(IAdvertisingSetCallback* cb);

    // I/F for AdvertisingSet
    bool getOwnAddress(int advertiserId);
    bool enableAdvertisingSet(int advertiserId,
                              bool enable,
                              int32_t duration,
                              int32_t maxExtAdvEvents);
    bool setAdvertisingData(int32_t advertiserId,
                            AdvertiseData *data);
    bool setScanResponseData(int32_t advertiserId,
                             AdvertiseData *data);
    bool setAdvertisingParameters(int32_t advertiserId,
                                  AdvertisingSetParameters *parameters);
    bool setPeriodicAdvertisingParameters(int32_t advertiserId,
                                          PeriodicAdvertiseParameters *parameters);
    bool setPeriodicAdvertisingData(int32_t advertiserId,
                                    AdvertiseData *data);
    bool setPeriodicAdvertisingEnable(int32_t advertiserId,
                                      bool enable);
    bool registerSync(gatt::ScanResult *scanResult,
                      int32_t skip,
                      int32_t timeout,
                      gatt::IPeriodicAdvertisingCallback* cb);
    bool unregisterSync(gatt::IPeriodicAdvertisingCallback* cb);

  private:
    template <typename T> int parseProperties(sd_bus_message *m, const char *type, T *value);
    int parseLeAdvManagerProperties(sd_bus_message *m, const char *property);

    std::mutex advMgr_prop_mtx_;
    GattDBusFw * gatt_dbus_service_;
    const char* deviceName_ = "";
    int32_t maxAdvertisingDataLength_;
    bool lePeriodicAdvertisingSupported_;
    bool le2MPhySupported_;
    bool leCodedPhySupported_;
};
} // namespace ipc
} // namespace gatt
#endif  //GATTLEADVERTISERMANAGERCLIENT_HPP
