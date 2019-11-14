/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SAMPLE_BLE_SERVER_CB_HPP_
#define SAMPLE_BLE_SERVER_CB_HPP_

#pragma once

#include "SampleBleServer.hpp"

#define LOGTAG "SAMPLE_BLE_SERVER_CB "
namespace gatt {
namespace sample {
class SampleBleServerCallback final: public GattServerCallback {
  public:
    explicit SampleBleServerCallback(SampleBleServer *server) : bleServer_(server) {};

    void onConnectionStateChange(string deviceAddress,
                                 int status,
                                 int newState) {
      ALOGD(LOGTAG"%s status: %d", __FUNCTION__, status);
      bleServer_->StopAdvertisement();
    }

    void onCharacteristicWriteRequest(string deviceAddress,
                                      int requestId,
                                      GattCharacteristic *characteristic,
                                      bool preparedWrite,
                                      bool responseNeeded,
                                      int offset,
                                      uint8_t* value) {
      ALOGD(LOGTAG"%s value = %s",__FUNCTION__, value);
      bleServer_->SendResponse(deviceAddress, requestId, 0, offset, value);
    }

    void onDescriptorWriteRequest(string deviceAddress,
                                  int requestId,
                                  GattDescriptor *descriptor,
                                  bool preparedWrite,
                                  bool responseNeeded,
                                  int offset,
                                  uint8_t* value) {
      ALOGD(LOGTAG"%s value = %s",__FUNCTION__, value);
      bleServer_->SendResponse(deviceAddress, requestId, 0, offset, value);
    }

  private:
    SampleBleServer *bleServer_;
};
}  // namespace sample
}  // namespace gatt
#endif  // SAMPLE_BLE_SERVER_CB_HPP_

