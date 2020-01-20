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

#ifndef GATTLESCANNER_HPP
#define GATTLESCANNER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <systemdq/sd-bus.h>

using std::string;

namespace gatt {
class ScanResult;
class ScanRecord;
namespace ipc {
class GattDBusService;

class GattLeScanner final {
  public:
    explicit GattLeScanner(GattDBusService * dbus_service);
    ~GattLeScanner() = default;

    bool onScannerRegistered(const string &server, int32_t status, int32_t scannerId);
    bool onScanResult(const string &server, ScanResult *scanResult, std::vector<uint8_t> *scanRecord);
    bool onBatchScanResults(const string &server, std::vector<ScanResult*> batchResults);
    bool onFoundOrLost(const string &server, bool onFound, ScanResult *scanResult);
    bool onScanManagerErrorCallback(const string &server, int errorCode);

  private:
    int populateScanResult(sd_bus_message *message, ScanResult *scanResult, std::vector<uint8_t>* scanRecord);

    GattDBusService * gatt_dbus_service_;
};
} // namespace ipc
} // namespace gatt
#endif  // GATTLESCANNER_HPP
