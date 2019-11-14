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

#include "AdvertisingSet.hpp"
#include <exception>
#include "GattDBusFw.hpp"

#define LOGTAG "AdvertisingSet"

using namespace std;
namespace gatt {
using ipc::GattDBusFw;

AdvertisingSet::~AdvertisingSet()
{
  mGatt = NULL;
}

AdvertisingSet::AdvertisingSet(int advertiserId)
{
  mAdvertiserId = advertiserId;
  mGatt = GattDBusFw::getGattDBusProxy()->gattLeAdvertiserManagerClient();
  if ( mGatt == NULL) {
    ALOGE(LOGTAG " Failed to get Bluetooth gatt");
  }
}

void AdvertisingSet::setAdvertiserId(int advertiserId)
{
  mAdvertiserId = advertiserId;
}

void AdvertisingSet::enableAdvertising(bool enable, int duration,
                                            int maxExtendedAdvertisingEvents)
{
  if (!mGatt->enableAdvertisingSet(mAdvertiserId, enable, duration,
            maxExtendedAdvertisingEvents)) {
    ALOGE(LOGTAG " fail to enableAdvertisingSet");
  }
}

void AdvertisingSet::setAdvertisingData(AdvertiseData& advertiseData)
{
  if (!mGatt->setAdvertisingData(mAdvertiserId, &advertiseData)) {
    ALOGE(LOGTAG " fail to setAdvertisingData");
  }
}

void AdvertisingSet::setScanResponseData(AdvertiseData& scanResponse)
{
  if (!mGatt->setScanResponseData(mAdvertiserId, &scanResponse)) {
    ALOGE(LOGTAG " fail to setScanResponseData");
  }
}

void AdvertisingSet::setAdvertisingParameters(AdvertisingSetParameters& parameters)
{
  if (!mGatt->setAdvertisingParameters(mAdvertiserId, &parameters)) {
    ALOGE(LOGTAG " fail to setAdvertisingParameters");
  }
}

void AdvertisingSet::setPeriodicAdvertisingParameters(PeriodicAdvertiseParameters& parameters)
{
  if (!mGatt->setPeriodicAdvertisingParameters(mAdvertiserId, &parameters)) {
    ALOGE(LOGTAG " fail to setPeriodicAdvertisingParameters");
  }
}

void AdvertisingSet::setPeriodicAdvertisingData(AdvertiseData& periodicData)
{
  if (!mGatt->setPeriodicAdvertisingData(mAdvertiserId, &periodicData)) {
    ALOGE(LOGTAG " fail to setPeriodicAdvertisingData");
  }
}

void AdvertisingSet::setPeriodicAdvertisingEnabled(bool enable)
{
  if (!mGatt->setPeriodicAdvertisingEnable(mAdvertiserId, &enable)) {
    ALOGE(LOGTAG " fail to setPeriodicAdvertisingEnable");
  }
}

void AdvertisingSet::getOwnAddress()
{
  if (!mGatt->getOwnAddress(mAdvertiserId)) {
    ALOGE(LOGTAG " fail to getOwnAddress");
  }
}

int AdvertisingSet::getAdvertiserId()
{
  return mAdvertiserId;
}
}//namespace gatt
