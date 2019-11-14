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

#ifndef GATTLEADVERTISING_HPP
#define GATTLEADVERTISING_HPP

#include <cstdint>
#include <string>

using std::string;

namespace gatt {
namespace ipc {
class GattDBusService;

class GattLeAdvertising final {
  public:
  explicit GattLeAdvertising(GattDBusService * dbus_service);
    ~GattLeAdvertising() = default;

    bool AdvertisingSetStarted(const string &server, int32_t advertiserId, int32_t tx_power, int32_t status);
    bool AdvertisingSetStopped(const string &server, int32_t advertiserId);
    bool OwnAddressRead(const string &server, int32_t advertiserId, int32_t addressType, string *address);
    bool AdvertisingEnabled(const string &server, int32_t advertiserId, bool enabled, int32_t status);
    bool AdvertisingDataSet(const string &server, int32_t advertiserId, int32_t status);
    bool ScanResponseDataSet(const string &server, int32_t advertiserId, int32_t status);
    bool AdvertisingParametersUpdated(const string &server,int32_t advertiserId, int32_t tx_power, int32_t status);
    bool PeriodicAdvertisingParametersUpdated(const string &server, int32_t advertiserId, int32_t status);
    bool PeriodicAdvertisingDataSet(const string &server, int32_t advertiserId, int32_t status);
    bool PeriodicAdvertisingEnabled(const string &server, int32_t advertiserId, bool enable, int32_t status);

  private:
    GattDBusService * gatt_dbus_service_;
};
} // namespace ipc
} // namespace gatt
#endif  // GATTLEADVERTISING_HPP
