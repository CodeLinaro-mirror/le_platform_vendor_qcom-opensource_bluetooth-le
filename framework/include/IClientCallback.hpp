/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ICLIENTCALLBACK_HPP_
#define ICLIENTCALLBACK_HPP_

#pragma once

#include "GattService.hpp"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
using std::string;

namespace gatt{
  /**
  *@hide
  */
class IClientCallback {
  public:
    virtual void onClientRegistered(int status, int clientIf) = 0;

    virtual void onConnectionState(int status, int clientIf,
                                     bool connected, string address) = 0;

    virtual void onPhyUpdate(string address, int txPhy, int rxPhy, int status) = 0;

    virtual void onPhyRead(string address, int txPhy, int rxPhy, int status) = 0;

    virtual void onSearchComplete(string address, std::vector<GattService*> services, int status) = 0;

    virtual void onCharacteristicRead(string address, int status, int handle, uint8_t *value, int length) = 0;

    virtual void onCharacteristicWrite(string address, int status, int handle) = 0;

    virtual void onExecuteWrite(string address, int status) = 0;

    virtual void onDescriptorRead(string address, int status, int handle, uint8_t *value, int length) = 0;

    virtual void onDescriptorWrite(string address, int status, int handle) = 0;

    virtual void onNotify(string address, int handle, uint8_t *value, int length) = 0;

    virtual void onReadRemoteRssi(string address, int rssi, int status) = 0;

    virtual void onConfigureMTU(string address, int mtu, int status) = 0;

    virtual void onConnectionUpdated(string address, int interval, int latency,
                                 int timeout, int status) = 0;

};
}
#endif
