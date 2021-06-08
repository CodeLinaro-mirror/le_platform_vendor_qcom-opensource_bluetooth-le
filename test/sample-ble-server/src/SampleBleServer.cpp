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

#include "SampleBleServer.hpp"
#include "SampleBleServerCb.hpp"
#include "SampleBleAdvertiserCb.hpp"

#include "GattServerCallback.hpp"
#include "AdvertisingSetParameters.hpp"
#include "AdvertiseData.hpp"
#include "AdvertiseSettings.hpp"
#include "AdvertisingSetCallback.hpp"
#include "uuid.h"


#define LOGTAG "SAMPLE_BLE_SERVER "

namespace gatt {
namespace sample {

#define CONNECTABLE 1
#define SCANNABLE 0
#define LEGACYMODE 0
#define ANONYMOUS 0
#define INCLUDETXPOWER 0
#define TRANSPORT 0
#define GATT_PROP_WRITE      (0x08)
#define GATT_PERM_WRITE      (0x10)

const Uuid SERVICE_UUID = Uuid::FromString("0000AA01-0000-1000-8000-00805f9b34fb");
const Uuid CHAR_UUID = Uuid::FromString("0000BB02-0000-1000-8000-00805f9b34fb");
const Uuid DESC_UUID = Uuid::FromString("0000CC03-0000-1000-8000-00805f9b34fb");

SampleBleServer::SampleBleServer() : service_state (SERVICE_INACTIVE), 
                                     mGattServer(NULL),
                                     mGattService(NULL),
                                     mGattCharacteristic(NULL),
                                     mGattDescriptor(NULL),
                                     mGattAdvertiser(NULL),
                                     mAdvParameters(NULL),
                                     mAdvData(NULL),
                                     mBleServerAdvCb(NULL),
                                     mGattServerCb(NULL) {
  ALOGD(LOGTAG"(%s)", __FUNCTION__);
  mGattServer = new GattServer(TRANSPORT);
}

SampleBleServer::~SampleBleServer() {
  ALOGD(LOGTAG"%s", __FUNCTION__);
  service_state  = SERVICE_INACTIVE;

  delete(mGattDescriptor);
  delete(mGattCharacteristic);
  delete(mGattService);
  delete(mBleServerAdvCb);
  delete(mGattServerCb);
  delete(mGattServer);
}

bool SampleBleServer::EnableBleServer() {
  ALOGD(LOGTAG"%s", __FUNCTION__);
  mGattServerCb = new SampleBleServerCallback(this);
  mGattServer->registerCallback(*mGattServerCb);

  mGattService = new GattService(SERVICE_UUID,GattService::SERVICE_TYPE_PRIMARY);
  mGattCharacteristic = new GattCharacteristic(CHAR_UUID, GATT_PROP_WRITE, GATT_PERM_WRITE);
  mGattDescriptor = new GattDescriptor(DESC_UUID, GATT_PERM_WRITE);

  mGattDescriptor->setValue("SAMPLE_BLE_DESC", sizeof("SAMPLE_BLE_SERVER")+1);

  mGattCharacteristic->addDescriptor(mGattDescriptor);
  mGattCharacteristic->setValue(std::string("SAMPLE_BLE_SERVER"));

  mGattService->addCharacteristic(mGattCharacteristic);  
  mGattServer->addService(*mGattService);

  mBleServerAdvCb = new SampleBleAdvertiserCallback();
  // Get hold of the system GattLeAdvertiser
  mGattAdvertiser = GattLeAdvertiser::getGattLeAdvertiser();

  // @TODO: Error handling
  // -  e.i: if system GattLeAdvertiser is not available, that is mGattAdvertiser is NULL
  return true;
}

bool SampleBleServer::StartAdvertisement() {
  ALOGD(LOGTAG"%s", __FUNCTION__);
  service_state  = SERVICE_INACTIVE;
  mAdvParameters = AdvertisingSetParameters::Builder()
                            .setConnectable(CONNECTABLE)
                            .setScannable(SCANNABLE)
                            .setLegacyMode(LEGACYMODE)
                            .setAnonymous(ANONYMOUS)
                            .setIncludeTxPower(INCLUDETXPOWER)
                            .setInterval(AdvertisingSetParameters::INTERVAL_MEDIUM)
                            .setTxPowerLevel(AdvertisingSetParameters::TX_POWER_MEDIUM)
                            .build();
  AdvertiseData::Builder builder = AdvertiseData::Builder().setIncludeDeviceName(true)
                                  .setIncludeTxPowerLevel(false);

  std::vector<uint8_t> vec(adv_data().begin(), adv_data().end());
  builder.addServiceUuid(SERVICE_UUID);
  builder.addServiceData(SERVICE_UUID, vec);
  mAdvData = builder.build();
  try {
      mGattAdvertiser->startAdvertisingSet(mAdvParameters,
                         mAdvData, NULL, NULL, NULL, mBleServerAdvCb);
  } catch(const std::exception &ex) {
    ALOGE(LOGTAG"%s start Advertising exception  %s", __FUNCTION__, ex.what());
    return false;
  }
  return true;
}

void SampleBleServer::StopAdvertisement(){
  ALOGD(LOGTAG"%s", __FUNCTION__);
  mGattAdvertiser->stopAdvertising(mBleServerAdvCb);
}

void SampleBleServer::SendResponse(string deviceAddress,
                       int requestId,
                       int status,
                       int offset,
                       uint8_t * value, size_t length) {
  ALOGD(LOGTAG"%s", __FUNCTION__);
  if(value != NULL && !strncasecmp((const char *)(value), "on", 2)) {
    if (service_state == SERVICE_INACTIVE) {
      service_state  = SERVICE_TRANSACTION_PENDING;

      // Go and perfom the specific SERVICES
      DoInternalServerStuff();
    }
    status = 0;
  } else {
    status = -1;
  }
  service_state  = SERVICE_ACTIVE;
  mGattServer->sendResponse(deviceAddress, requestId, status, offset, value, length);
}

void SampleBleServer::DoInternalServerStuff() {
  ALOGD(LOGTAG"%s", __FUNCTION__);

  // Do internal processing, in this example trigger StopAdvertising.
  StopAdvertisement();
}
}  // namespace sample
}  // namespace gatt

int main() {
    gatt::sample::SampleBleServer bleServer;
    bleServer.EnableBleServer();
    bleServer.StartAdvertisement();
    // TODO: Handle system calls for signal handling.
    while (1) {};
}
