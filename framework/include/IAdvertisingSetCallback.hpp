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

#ifndef I_ADV_SET_CALLBACK_HPP_
#define I_ADV_SET_CALLBACK_HPP_

#pragma once

#include <iostream>
using namespace std;
using std::string;

namespace gatt{
  /**
  *@hide
  */
class IAdvertisingSetCallback {
  public:
    virtual void onAdvertisingSetStarted(int advertiserId, int tx_power, int status) = 0;
    virtual void onOwnAddressRead(int advertiserId, int addressType, string address) = 0;
    virtual void onAdvertisingSetStopped(int advertiserId) = 0;
    virtual void onAdvertisingEnabled(int advertiserId, bool enable, int status) = 0;
    virtual void onAdvertisingDataSet(int advertiserId, int status) = 0;
    virtual void onScanResponseDataSet(int advertiserId, int status) = 0;
    virtual void onAdvertisingParametersUpdated(int advertiserId, int tx_power, int status) = 0;
    virtual void onPeriodicAdvertisingParametersUpdated(int advertiserId, int status) = 0;
    virtual void onPeriodicAdvertisingDataSet(int advertiserId, int status) = 0;
    virtual void onPeriodicAdvertisingEnabled(int advertiserId, bool enable, int status) = 0;

    bool operator==(const IAdvertisingSetCallback& rhs) const
    {
      return *this == rhs;
    }
};
}
#endif
