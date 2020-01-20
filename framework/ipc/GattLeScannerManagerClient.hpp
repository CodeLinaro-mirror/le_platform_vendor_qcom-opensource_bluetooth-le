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

#ifndef GATTLESCANNERMANAGERCLIENT_HPP
#define GATTLESCANNERMANAGERCLIENT_HPP

#include "IScannerCallback.hpp"
#include "ScanSettings.hpp"
#include "ScanFilter.hpp"
#include "ResultStorageDescriptor.hpp"
#include <systemdq/sd-bus.h>

namespace gatt {
namespace ipc {
class GattDBusFw;

using gatt::ipc::GattDBusFw;

class GattLeScannerManagerClient final {
  public:
    explicit GattLeScannerManagerClient(GattDBusFw * dbus_service);
    ~GattLeScannerManagerClient() = default;

    bool registerScanner(IScannerCallback *callback);
    bool unregisterScanner(int scannerId);
    bool startScan(int scannerId, ScanSettings *settings, std::vector<ScanFilter*> filters,
                   std::vector<std::vector<ResultStorageDescriptor*>> storages);
    bool stopScan(int scannerId);
    bool flushPendingBatchResults(int scannerId);
    bool disconnectAll();
    bool isScanClient(int clientIf);
    bool isOffloadedFilteringSupported();
    int numHwTrackFiltersAvailable();

  private:
    int populateScanSettings(sd_bus_message *m, ScanSettings *scanSettings);
    int populateScanFilters(sd_bus_message *m, std::vector<ScanFilter*> *filters);
    int populateScanResultStorageDesc(sd_bus_message *m, std::vector<std::vector<ResultStorageDescriptor*>> *storages);
    int populateStartScan(sd_bus_message *m, ScanSettings *scanSettings, std::vector<ScanFilter*> *filters,
                             std::vector<std::vector<ResultStorageDescriptor*>> *storages);

    GattDBusFw * gatt_dbus_service_;
};
} // namespace ipc
} // namespace gatt
#endif  //GATTLESCANNERMANAGERCLIENT_HPP
