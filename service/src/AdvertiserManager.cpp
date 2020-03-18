/*
* Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
#include "AdvertiserManager.hpp"
#include "GattLibService.hpp"
#include "IGattNativeV2.hpp"
#include "GattDBusService.hpp"
#include "GattLeAdvertising.hpp"

#define LOGTAG "AdvertiserManager"

#define CHECK_PARAM_VOID(x)                                                      \
   if (!x) {                                                                     \
       ALOGE("'%s' Void Param is NULL - exiting from function ", __FUNCTION__);  \
       return ;                                                                  \
   }

using namespace std;
namespace gatt {

int AdvertiserManager::sTempRegistrationId = -1;

AdvertiserManager::AdvertiserManager(IGattNativeV2 *mGattIf)
{
  if (DBG) {
    ALOGD(LOGTAG " advertise manager created");
  }
  mNative = mGattIf;
}

void AdvertiserManager::cleanup()
{
  if (DBG) {
      ALOGD(LOGTAG " cleanup()");
  }
  /*IAdvertisingSetCallback to be cleaned in application*/
  // mAdvertisers.clear();    // TODO: What was the problem here?
  sTempRegistrationId = -1;
  mNative = NULL;
}

AdvertiserManager::~AdvertiserManager()
{
  this->cleanup();
  ALOGE(LOGTAG "Deinit DOne");
}

std::vector<uint8_t> AdvertiserManager::toVec(uint8_t *data)
{
  size_t len = strlen((char*)data);
  std::vector<uint8_t> vec(data,data+len);

  return vec;
}

static uint32_t INTERVAL_MAX = 0xFFFFFF;
// Always give controller 31.25ms difference between min and max
static uint32_t INTERVAL_DELTA = 50;

advertise_parameters_t AdvertiserManager::parseParams(AdvertisingSetParameters *parameters)
{
  advertise_parameters_t p;

  bool isConnectable = parameters->isConnectable();
  bool isScannable = parameters->isScannable();
  bool isLegacy = parameters->isLegacy();
  bool isAnonymous = parameters->isAnonymous();
  bool includeTxPower = parameters->includeTxPower();
  uint8_t primaryPhy = parameters->getPrimaryPhy();
  uint8_t secondaryPhy = parameters->getSecondaryPhy();
  uint32_t interval = parameters->getInterval();
  int8_t txPowerLevel = parameters->getTxPowerLevel();

  uint16_t props = 0;
  if (isConnectable) props |= 0x01;
  if (isScannable) props |= 0x02;
  if (isLegacy) props |= 0x10;
  if (isAnonymous) props |= 0x20;
  if (includeTxPower) props |= 0x40;

  if (interval > INTERVAL_MAX - INTERVAL_DELTA) {
    interval = INTERVAL_MAX - INTERVAL_DELTA;
  }

  p.advertising_event_properties = props;
  p.min_interval = interval;
  p.max_interval = interval + INTERVAL_DELTA;
  p.channel_map = 0x07; /* all channels */
  p.tx_power = txPowerLevel;
  p.primary_advertising_phy = primaryPhy;
  p.secondary_advertising_phy = secondaryPhy;
  p.scan_request_notification_enable = false;

  return p;
}

periodic_advertising_parameters_t AdvertiserManager::
          parsePeriodicParams(PeriodicAdvertiseParameters *parameter)
{
  periodic_advertising_parameters_t p;

  if (parameter == NULL) {
    p.enable = false;
    return p;
  }

  bool includeTxPower = parameter->getIncludeTxPower();
  uint16_t interval = parameter->getInterval();

  p.enable = true;
  p.min_interval = interval;
  p.max_interval = interval + 16; /* 20ms difference betwen min and max */
  uint16_t props = 0;
  if (includeTxPower) props |= 0x40;
  p.periodic_advertising_properties = props;

  return p;
}

void AdvertiserManager::startAdvertisingSet(AdvertisingSetParameters& parameters,
                                                  AdvertiseData& advertiseData,
                                                  AdvertiseData& scanResponse,
                                                  PeriodicAdvertiseParameters& periodicParameters,
                                                  AdvertiseData& periodicData, int duration,
                                                  int maxExtAdvEvents,
                                                  const string &sender)
{
  if (DBG) {
    ALOGD(LOGTAG " startAdvertisingSet -> sender: %s", sender.c_str());
  }
  GattLibService *mGatt = GattLibService::getGatt();
  if(mGatt == NULL) return;

  string deviceName = mGatt->getDeviceName();
  std::vector<uint8_t> advDataBytes =
            (AdvertiseHelper::advertiseDataToBytes(&advertiseData, deviceName));
  std::vector<uint8_t> scanResponseBytes =
            (AdvertiseHelper::advertiseDataToBytes(&scanResponse, deviceName));
  std::vector<uint8_t> periodicDataBytes =
            (AdvertiseHelper::advertiseDataToBytes(&periodicData, deviceName));

  int cbId = --sTempRegistrationId;
  mAdvertiser_listeners.insert({{cbId, sender}});

  if (DBG) {
    ALOGD(LOGTAG " startAdvertisingSet() - reg_id= %d ", cbId);
  }

  advertise_parameters_t advParameter = parseParams(&parameters);
  periodic_advertising_parameters_t periodicAdvParameter = parsePeriodicParams(&periodicParameters);
  mNative->startAdvertisingSetNative(advParameter,advDataBytes,
                                    scanResponseBytes, periodicAdvParameter,
                                    periodicDataBytes, duration,
                                    maxExtAdvEvents, cbId);
  return;
}

void AdvertiserManager::getOwnAddress(int advertiserId)
{
  mNative->getOwnAddressNative(advertiserId);
}

void AdvertiserManager::stopAdvertisingSet(const string &sender)
{
  if (DBG) {
    ALOGD(LOGTAG " stopAdvertisingSet(): %s ", sender.c_str());
  }

  std::unordered_map<int,string>::iterator it = mAdvertiser_listeners.begin();
  for(; it != mAdvertiser_listeners.end(); ++it) {
    ALOGD(LOGTAG " stopAdvertisingSet(): mAdvertiser_listeners[] %s ", it->second.c_str());
    if ((it->second) == sender) {
      break;
    }
  }

  if (it == mAdvertiser_listeners.end()) {
    ALOGE(LOGTAG " stopAdvertisingSet() - no client found for callback");
    return;
  }

  int advertiserId = it->first;
  if (advertiserId < 0) {
    ALOGI(LOGTAG " stopAdvertisingSet() - advertiser not finished registration yet");
    // Advertiser will be freed once initiated in onAdvertisingSetStarted()
    return;
  }

  mNative->stopAdvertisingSetNative(advertiserId);

  // Delete the entry from the map.
  mAdvertiser_listeners.erase(advertiserId);

  if (mAdvertisingCb_ == NULL) {
      mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  if (!mAdvertisingCb_->AdvertisingSetStopped(sender, advertiserId)) {
    ALOGE(LOGTAG " Error sending AdvertisingSetStopped callback");
  }
}

void AdvertiserManager::enableAdvertisingSet(int advertiserId, bool enable, int duration,
                                                   int maxExtAdvEvents)
{
  mNative->enableAdvertisingSetNative(advertiserId, enable, duration, maxExtAdvEvents);
}

void AdvertiserManager::setAdvertisingData(int advertiserId, AdvertiseData& data) {
  GattLibService *mGatt = GattLibService::getGatt();
  if(mGatt == NULL) return;
  string deviceName = mGatt->getDeviceName();
  mNative->setAdvertisingDataNative(advertiserId,
                (AdvertiseHelper::advertiseDataToBytes(&data, deviceName)));
}

void AdvertiserManager::setScanResponseData(int advertiserId, AdvertiseData& data)
{
  GattLibService *mGatt = GattLibService::getGatt();
  if(mGatt == NULL) return;
  string deviceName = mGatt->getDeviceName();
  mNative->setScanResponseDataNative(advertiserId,
          (AdvertiseHelper::advertiseDataToBytes(&data, deviceName)));
}

void AdvertiserManager::setAdvertisingParameters(int advertiserId,
                                                         AdvertisingSetParameters& parameters)
{
  advertise_parameters_t advParameter = parseParams(&parameters);
  mNative->setAdvertisingParametersNative(advertiserId, advParameter);
}

void AdvertiserManager::setPeriodicAdvertisingParameters(int advertiserId,
                                                       PeriodicAdvertiseParameters& parameters)
{
  periodic_advertising_parameters_t periodicAdvParameter = parsePeriodicParams(&parameters);
  mNative->setPeriodicAdvertisingParametersNative(advertiserId, periodicAdvParameter);
}

void AdvertiserManager::setPeriodicAdvertisingData(int advertiserId, AdvertiseData& data)
{
  GattLibService *mGatt = GattLibService::getGatt();
  if(mGatt == NULL) return;
  string deviceName = mGatt->getDeviceName();
  mNative->setPeriodicAdvertisingDataNative(advertiserId,
          (AdvertiseHelper::advertiseDataToBytes(&data, deviceName)));
}

void AdvertiserManager::setPeriodicAdvertisingEnable(int advertiserId, bool enable)
{
  mNative->setPeriodicAdvertisingEnableNative(advertiserId, enable);
}

void AdvertiserManager::stopAdvertisingSets()
{
  ALOGD(LOGTAG " stopAdvertisingSets()");
  for(std::unordered_map<int, string>::iterator it = mAdvertiser_listeners.begin();
                          it != mAdvertiser_listeners.end(); ++it) {
    auto advertiser_id = it->first;

    if (advertiser_id < 0) {
      ALOGI(LOGTAG " stopAdvertisingSets() - advertiser not finished registration yet");
      continue;
    }

    mNative->stopAdvertisingSetNative(advertiser_id);

    // Delete the entry from the map.
    mAdvertiser_listeners.erase(advertiser_id);

    auto sender  = it->second;
    if (!mAdvertisingCb_->AdvertisingSetStopped(sender, advertiser_id)) {
      ALOGE(LOGTAG " Failed calling AdvertisingSetStopped");
    }
  }
}

void AdvertiserManager::onAdvertisingSetStarted(int regId, int advertiserId, int txPower, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onAdvertisingSetStarted() - regId= %d advertiserId=%d status=%d", regId,
                  advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(regId);

  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingSetStarted() - no callback found for regId %d ", regId);
    // Advertising set was stopped before it was properly registered.
    mNative->stopAdvertisingSetNative(advertiserId);
    return;
  }

  auto sender = entry->second;
  if (status == 0) {
    mAdvertiser_listeners.insert({{advertiserId, sender}});
    mAdvertiser_listeners.erase(regId);
  } else {
    mAdvertiser_listeners.erase(advertiserId);
  }

  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->AdvertisingSetStarted(sender, advertiserId, txPower, status);
  return;
}

void AdvertiserManager::onAdvertisingEnabled(int advertiserId, bool enable, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onAdvertisingSetEnabled() - advertiserId %d enable %d status %d",
                    advertiserId, enable, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingSetEnable() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->AdvertisingEnabled(sender, advertiserId, enable, status);
}


void AdvertiserManager::onOwnAddressRead(int advertiserId, int addressType, string *address)
{
  if (DBG) {
    ALOGD(LOGTAG " onOwnAddressRead() advertiserId %d", advertiserId);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onOwnAddressRead() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->OwnAddressRead(sender, advertiserId, addressType, address);
}

void AdvertiserManager::onAdvertisingDataSet(int advertiserId, int status)
{
  if (DBG) {
     ALOGD(LOGTAG " onAdvertisingDataSet() advertiserId %d status %d",
               advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingDataSet() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->AdvertisingDataSet(sender, advertiserId, status);
}

void AdvertiserManager::onScanResponseDataSet(int advertiserId, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onScanResponseDataSet() advertiserId %d status %d",
             advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onScanResponseDataSet() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->ScanResponseDataSet(sender, advertiserId, status);
}

void AdvertiserManager::onAdvertisingParametersUpdated(int advertiserId, int txPower, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onAdvertisingParametersUpdated() advertiserId %d txPower %d status %d",
                       advertiserId, txPower, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingParametersUpdated() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->AdvertisingParametersUpdated(sender, advertiserId, txPower, status);
}

void AdvertiserManager::onPeriodicAdvertisingParametersUpdated(int advertiserId, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onPeriodicAdvertisingParametersUpdated() advertiserId %d status %d",
             advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingParametersUpdated() - no callback found for advertiserId %d"
           , advertiserId);
    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ = GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->PeriodicAdvertisingParametersUpdated(sender, advertiserId, status);
}

void AdvertiserManager::onPeriodicAdvertisingDataSet(int advertiserId, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onPeriodicAdvertisingDataSet() advertiserId %d status %d",
              advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingParametersUpdated() - no callback found for advertiserId %d"
           , advertiserId);

    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ =  GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->PeriodicAdvertisingDataSet(sender, advertiserId, status);
}

void AdvertiserManager::onPeriodicAdvertisingEnabled(int advertiserId, bool enable, int status)
{
  if (DBG) {
    ALOGD(LOGTAG " onPeriodicAdvertisingEnabled() advertiserId %d status %d",
          advertiserId, status);
  }

  auto entry = mAdvertiser_listeners.find(advertiserId);
  if (entry == mAdvertiser_listeners.end()) {
    ALOGI(LOGTAG " onAdvertisingParametersUpdated() - no callback found for advertiserId %d"
           , advertiserId);

    return;
  }

  auto sender = entry->second;
  if (mAdvertisingCb_ == NULL) {
    mAdvertisingCb_ =  GattDBusService::getGattDBusProxy()->gattLeAdvertising();
    if (mAdvertisingCb_ == NULL) {
      ALOGE(LOGTAG " Failed to get Bluetooth gatt - NULL ");
      return;
    }
  }
  mAdvertisingCb_->PeriodicAdvertisingEnabled(sender, advertiserId, enable, status);
}
}
