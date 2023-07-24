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

#include "BluetoothLeSdbus.hpp"
#include <cstdint>
#include <iostream>
#include <vector>
#include "uuid.h"

#define LOGTAG "SdBus "

namespace SdBus {
int PopulateGattServiceMessage(sd_bus_message *message, gatt::GattService *svc) {
  if (svc == nullptr) {
    ALOGE(LOGTAG " %s - > Gatt Service object is not valid", __func__);
    return -1;
  }
  std::string uuid = svc->getUuid().ToString();
  int instance = svc->getInstanceId();
  bool advertisePreferred = svc->isAdvertisePreferred();
  std::string deviceaddress = svc->getDevice();
  int serviceType = svc->getType();
  std::vector<gatt::GattCharacteristic *> characteristics = svc->getCharacteristics();
  std::vector<gatt::GattService *> includedServices = svc->getIncludedServices();
  //sd_bus_message_append(m, "v", "g", "sdbusisgood");
  // Open the array of entries
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not create dictionary", __func__);
    return r;
  }
  r = AppendDictEntry(message, "Uuid", uuid);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate Uuid", __func__);
    return r;
  }
  r = AppendDictEntry(message, "instance", static_cast<int32_t>(instance));
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate instance", __func__);
    return r;
  }
  r = AppendDictEntry(message, "advertisePreferred", advertisePreferred);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate advertisePreferred", __func__);
    return r;
  }
  r = AppendDictEntry(message, "deviceaddress", deviceaddress);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate deviceaddress", __func__);
    return r;
  }
  r = AppendDictEntry(message, "serviceType", serviceType);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate serviceType", __func__);
    return r;
  }
  r = AppendDictEntry(message, "characteristics", characteristics);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate characteristics", __func__);
    return r;
  }
  r = AppendDictEntry(message, "includedServices", includedServices);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate includedServices", __func__);
    return r;
  }
  // Close the Array
  r = sd_bus_message_close_container(message);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Could not close dictionary", __func__);
    return r;
  }
  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, bool value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "b");
  if (r < 0)
    return r;
  int int_value = value ? 1 : 0;
  r = sd_bus_message_append_basic(message, 'b', &int_value);

  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::string value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "s");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', value.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, const std::vector<uint8_t> &value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "ay");
  if (r < 0)
    return r;
  r = sd_bus_message_append_array(message, 'y', value.data(), value.size());
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, uint32_t value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;

  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "u");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 'u', &value);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return r;
}

int AppendDictEntry(sd_bus_message *message, std::string key, int32_t value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;

  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "i");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 'i', &value);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return r;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<bt::Uuid> value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "as");
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "s");
  if (r < 0) {
    return r;
  }
  for (auto entry : value) {
    r = sd_bus_message_append_basic(message, 's', entry.ToString().c_str());
    if (r < 0)
      return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::map<bt::Uuid, std::vector<uint8_t>> value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{say}");
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{say}");
  if (r < 0) {
    return r;
  }
  int len;
  for (std::map<bt::Uuid, std::vector<uint8_t>>::iterator it = value.begin();
          it!= value.end(); ++it) {
    sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "say");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', it->first.ToString().c_str());
    if (r < 0)
      return r;
    r = sd_bus_message_append_array(message, 'y', it->second.data(), it->second.size());
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattService *> value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "av");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "v");
  if (r < 0) {
    return r;
  }

  for (auto service : value) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;

    r = PopulateGattServiceMessage(message, service);
    if (r < 0)
      return r;

    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattCharacteristic *> value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "av");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "v");
  if (r < 0) {
    return r;
  }
  for (auto characteristic : value) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;

    r = PopulateGattCharacteristic(message, characteristic);
    if (r < 0)
      return r;

    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int PopulateGattCharacteristic(sd_bus_message *message, gatt::GattCharacteristic *gatt_characteristic) {
  if (gatt_characteristic == nullptr) {
    ALOGE(LOGTAG " %s - > Gatt Service object is not valid", __func__);
    return -1;
  }
  std::string uuid = gatt_characteristic->getUuid().ToString();
  int instance = gatt_characteristic->getInstanceId();
  int permissions = gatt_characteristic->getPermissions();
  int properties = gatt_characteristic->getProperties();
  int keySize = gatt_characteristic->getKeySize();
  int writeType = gatt_characteristic->getWriteType();
  uint8_t *value = gatt_characteristic->getValue();
  int valueLength = gatt_characteristic->getValueLength();
  std::vector<gatt::GattDescriptor *> descriptors = gatt_characteristic->getDescriptors();

  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not create dict", __func__);
    return r;
  }
  r = AppendDictEntry(message, "Uuid", uuid);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate Uuid", __func__);
    return r;
  }
  r = AppendDictEntry(message, "instance", instance);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate instance", __func__);
    return r;
  }
  r = AppendDictEntry(message, "permissions", permissions);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate permissions", __func__);
    return r;
  }
  r = AppendDictEntry(message, "properties", properties);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate properties", __func__);
    return r;
  }
  r = AppendDictEntry(message, "keySize", keySize);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate keySize", __func__);
    return r;
  }
  r = AppendDictEntry(message, "writeType", writeType);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate writeType", __func__);
    return r;
  }
  r = AppendDictEntry(message, "value", value, valueLength);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate value", __func__);
    return r;
  }

  r = AppendDictEntry(message, "descriptors", descriptors);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate descriptors", __func__);
    return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not close dict", __func__);
    return r;
  }
  return 1;
}

int PopulateGattDescriptor(sd_bus_message *message, gatt::GattDescriptor *gatt_descriptor) {
  if (gatt_descriptor == nullptr) {
    ALOGE(LOGTAG " %s - > Gatt Service object is not valid", __func__);
    return -1;
  }
  std::string uuid = gatt_descriptor->getUuid().ToString();
  int instance = gatt_descriptor->getInstanceId();
  int permissions = gatt_descriptor->getPermissions();
  // This is a byte array with a null terminated string. The length of the array
  // can be achieved with functions such as strlen
  uint8_t *value = gatt_descriptor->getValue();
  int valueLength = gatt_descriptor->getValueLength();

  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");

  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Open Dict", __func__);

    return r;
  }

  r = AppendDictEntry(message, "Uuid", uuid);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate Uuid", __func__);
    return r;
  }
  r = AppendDictEntry(message, "instance", instance);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate instance", __func__);
    return r;
  }

  r = AppendDictEntry(message, "permissions", permissions);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate permissions", __func__);
    return r;
  }

  r = AppendDictEntry(message, "value", value, valueLength);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: Could not Populate value", __func__);
    return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0) {
    ALOGE(LOGTAG " %s -> ERROR: not close Dict", __func__);
    return r;
  }
  return 1;
}

int PopulateLeAdvertisingSetParameters(sd_bus_message *message, gatt::AdvertisingSetParameters *parameters) {
  if (parameters == nullptr) {
    ALOGE(LOGTAG " %s - > AdvertisingSetParameters set not valid", __func__);
    return -1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  r = AppendDictEntry(message, "Legacy", parameters->isLegacy());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "Anonymous", parameters->isAnonymous());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "IncludeTxPower", parameters->includeTxPower());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "PrimaryPhy", (int32_t) (parameters->getPrimaryPhy()));
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "SecondaryPhy", (int32_t) (parameters->getSecondaryPhy()));
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "Connectable", parameters->isConnectable());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "Scannable", parameters->isScannable());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "Interval", (int32_t) (parameters->getInterval()));
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "TxPowerLevel", (int32_t) parameters->getTxPowerLevel());
  if (r < 0) {
    return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }

  return 1;
}

int PopulateLeAdvertiseData(sd_bus_message *message, gatt::AdvertiseData *advData) {
  if (advData == nullptr) {
    // This is a valid case
    ALOGE(LOGTAG " %s - > AdvertiseData not available", __func__);
    return 1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  std::vector<uint8_t> rawData = advData->getRawData();
  if(!rawData.empty()) {
    r = AppendDictEntry(message, "RawData", rawData.data(), rawData.size());
    if (r < 0) {
      return r;
    }
  }
  r = AppendDictEntry(message, "IncludeTxPowerLevel", advData->getIncludeTxPowerLevel());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "IncludeDeviceName", advData->getIncludeDeviceName());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "ServiceUuids", advData->getServiceUuids());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "ServiceData", advData->getServiceData());
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }

  return 1;
}

int PopulateLePeriodicAdvertiseParameters(sd_bus_message *message, gatt::PeriodicAdvertiseParameters *periodicParams) {
  if (periodicParams == nullptr) {
    // This is a valid case
    ALOGE(LOGTAG " %s - > PeriodicAdvertiseParameters not available", __func__);
    return 1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  r = AppendDictEntry(message, "IncludeTxPower", periodicParams->getIncludeTxPower());
  if (r < 0) {
    return r;
  }
  r = AppendDictEntry(message, "Interval", (int32_t) periodicParams->getInterval());
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int PopulateAdvertisingSet(sd_bus_message *message, gatt::AdvertisingSetParameters *advSetParameters,
        gatt::AdvertiseData *advertiseData, gatt::AdvertiseData *scanResponse,
        gatt::PeriodicAdvertiseParameters *periodicParameters, gatt::AdvertiseData *periodicData,
        uint32_t duration, uint32_t maxExtAdvEvents) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  // AdvertisingSet Dict Entries
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  // Key
  r = sd_bus_message_append_basic(message, 's', "advertisingSet");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
  if (r < 0) {
    return r;
  }
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  //AdvertisingSetParameters
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', "AdvertisingSetParameters");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
  if (r < 0)
    return r;
  r = PopulateLeAdvertisingSetParameters(message, advSetParameters);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  // AdvertiseData
  if (advertiseData != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "AdvertiseData");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = PopulateLeAdvertiseData(message, advertiseData);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  // ScanResponse
  if (scanResponse != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "ScanResponse");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = PopulateLeAdvertiseData(message, scanResponse);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  //PeriodicAdvertiseSetParameters
  if (periodicParameters != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "PeriodicAdvertiseSetParameters");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = PopulateLePeriodicAdvertiseParameters(message, periodicParameters);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  // PeriodicData
  if (periodicData != nullptr) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
    if (r < 0)
      return r;
    r = sd_bus_message_append_basic(message, 's', "PeriodicData");
    if (r < 0)
      return r;
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;
    r = PopulateLeAdvertiseData(message, periodicData);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }

  // Duration
  r = AppendDictEntry(message, "Duration", static_cast<int32_t>(duration));
  if (r < 0) {
    return r;
  }
  // MaxExtAdvEvents
  r = AppendDictEntry(message, "MaxExtAdvEvents", static_cast<int32_t>(maxExtAdvEvents));
  if (r < 0) {
    return r;
  }

  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }

  return 1;
}


int AppendDictEntry(sd_bus_message *message, std::string key, uint8_t *value, size_t length) {
  if (value == nullptr) {
    return -1;
  }
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "ay");
  if (r < 0)
    return r;
  r = sd_bus_message_append_array(message, 'y', value, length);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattDescriptor *> value) {
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_DICT_ENTRY, "sv");
  if (r < 0)
    return r;
  r = sd_bus_message_append_basic(message, 's', key.c_str());
  if (r < 0)
    return r;
  r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "av");
  if (r < 0)
    return r;

  r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "v");
  if (r < 0) {
    return r;
  }

  for (auto descriptor : value) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;

    r = PopulateGattDescriptor(message, descriptor);
    if (r < 0)
      return r;
    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;

  return 1;
}

int getServiceUuidsFromMessage(sd_bus_message *m, vector<bt::Uuid> *serviceUuids) {
  char type;
  const char *contents;
  const char *uuid;

  int res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "as");
  if (res < 0) {
    ALOGE(LOGTAG "Failed to enter container %s", strerror(-res));
    return res;
  }

  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "s");
  if (res < 0) {
    ALOGE(LOGTAG "Failed to enter array of strings %s", strerror(-res));
    return res;
  }
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    if (res == 0) {
      break;
    }

    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &uuid);
    if (res < 0) {
      return res;
    }
    ALOGD(LOGTAG "array entry value: %s", uuid);
    serviceUuids->push_back(bt::Uuid::FromString(uuid));
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getManufacturerDataFromMessage(sd_bus_message *m, std::map<int, std::vector<uint8_t>> *manufacturerData) {
  char type;
  const char *contents;
  char **array;
  uint8_t *data = nullptr;
  int manufacturerId;
  size_t array_size;

  int res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "av");
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "v");
  if (res < 0) {
    return res;
  }

  // For each entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a variant
    if (type != SD_BUS_TYPE_VARIANT) {
      ALOGE(LOGTAG "%s -> Type not variant, it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Variant
    res = sd_bus_message_enter_container(m, type, contents);
    ALOGE(LOGTAG "%s -> open Variant type: %c", __func__, type);
    if (res < 0) {
      return res;
    }

    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }

    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the manufacturer ID
    res = sd_bus_message_read(m, "i", &manufacturerId);
    if (res < 0) {
      return res;
    }
    ALOGD(LOGTAG "manufacturerId: %d", manufacturerId);
    // Get the data
    res = sd_bus_message_read_array(m, 'y', &data, &array_size);
    if (res < 0) {
      return res;
    }

    std::vector<uint8_t> dataVec(data, data + array_size);

    ALOGE(LOGTAG "%s -> dataVec.size(): %d", __func__, dataVec.size());
    manufacturerData->insert(std::pair<int, std::vector<uint8_t>> ( manufacturerId, dataVec ) );
    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }

  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getServiceDataFromMessage(sd_bus_message *m, std::map<bt::Uuid, std::vector<uint8_t>> *serviceData) {
  char type;
  const char *contents;
  char **array;
  uint8_t *data = nullptr;
  const char *uuid;
  size_t array_size;

  int res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "a{say}");
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{say}");
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not dict, it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the uuid
    const char *uuid;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &uuid);
    if (res < 0) {
      return res;
    }
    ALOGD(LOGTAG "array entry value: %s", uuid);
    // Get the data
    res = sd_bus_message_read_array(m, 'y', &data, &array_size);
    if (res < 0) {
      return res;
    }
    std::vector<uint8_t> dataVec(data, data + array_size);
    serviceData->insert(std::pair<bt::Uuid, std::vector<uint8_t>> ( Uuid::FromString(uuid), dataVec ) );
    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getAdvertiseDataFromMessage(sd_bus_message *m, gatt::AdvertiseData **advData) {
  std::vector<uint8_t> rawData;
  bool includeTxPowerLevel = false;
  bool includeDeviceName = false;
  std::string localName;
  bool localNameShortened;
  std::vector<bt::Uuid> serviceUuids;
  std::map<bt::Uuid, std::vector<uint8_t>> serviceData;
  std::map<int, std::vector<uint8_t>> manufacturerData;

  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not a dict, it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the  key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "RawData") == 0) {
      uint8_t *value = nullptr;
      size_t array_size;
      res = getByteArrayFromMessage(m, &value, &array_size);
      if (res < 0) {
        return res;
      }
      rawData.assign(value, value + array_size);
    } else
    if (strcasecmp(key, "IncludeTxPowerLevel") == 0) {
      res = getBoolFromMessage(m, &includeTxPowerLevel);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "IncludeDeviceName") == 0) {
      res = getBoolFromMessage(m, &includeDeviceName);
      if (res < 0) {
        return res;
	    }
    } else if (strcasecmp(key, "LocalName") == 0) {
      res = getStringFromMessage(m, &localName);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "LocalNameShortened") == 0) {
      res = getBoolFromMessage(m, &localNameShortened);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ServiceUuids") == 0) {
      res = getServiceUuidsFromMessage(m, &serviceUuids);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ServiceData") == 0) {
      res = getServiceDataFromMessage(m, &serviceData);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ManufacturerData") == 0) {
      res = getManufacturerDataFromMessage(m, &manufacturerData);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  gatt::AdvertiseData::Builder builder = gatt::AdvertiseData::Builder().setIncludeDeviceName(includeDeviceName)
                                  .setIncludeTxPowerLevel(includeTxPowerLevel)
                                  .setRawData(rawData)
                                  .setLocalNameShortened(localNameShortened)
                                  .setLocalName(localName);
  for (auto serviceUuid : serviceUuids) {
    builder.addServiceUuid(serviceUuid);
  }
  for (std::map<bt::Uuid, std::vector<uint8_t>>::iterator it = serviceData.begin();
          it != serviceData.end(); ++it) {
    builder.addServiceData(it->first,it->second);
  }
  for (std::map<int, std::vector<uint8_t>>::iterator it = manufacturerData.begin();
	 it != manufacturerData.end(); ++it) {
    builder.addManufacturerData(it->first, it->second);
  }

  *advData = builder.build();
  return 1;
}

int getAdvertisingSetParametersFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **advSetParameters) {
  bool legacyMode; // TODO: initiaize params to default values
  bool anonymus;
  bool includeTxPower;
  int32_t primaryPhy;
  int32_t secondaryPhy;
  bool connectable;
  bool scannable;
  int32_t interval;
  int32_t txPowerLevel;

  char type;
  const char *contents;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }

    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not array. it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }
    if (strcasecmp(key, "Legacy") == 0) {
      res = getBoolFromMessage(m, &legacyMode);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Anonymous") == 0) {
      res = getBoolFromMessage(m, &anonymus);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "IncludeTxPower") == 0) {
      res = getBoolFromMessage(m, &includeTxPower);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "PrimaryPhy") == 0) {
      res = getInt32FromMessage(m, &primaryPhy);
     if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "SecondaryPhy") == 0) {
      res = getInt32FromMessage(m, &secondaryPhy);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Connectable") == 0) {
      res = getBoolFromMessage(m, &connectable);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Scannable") == 0) {
      res = getBoolFromMessage(m, &scannable);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Interval") == 0) {
      res = getInt32FromMessage(m, &interval);
          if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "TxPowerLevel") == 0) {
      res = getInt32FromMessage(m, &txPowerLevel);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  *advSetParameters = gatt::AdvertisingSetParameters::Builder()
                            .setConnectable(connectable)
                            .setScannable(scannable)
                            .setLegacyMode(legacyMode)
                            .setAnonymous(anonymus)
                            .setIncludeTxPower(includeTxPower)
                            .setInterval(interval)
                            .setTxPowerLevel(txPowerLevel)
                            .setPrimaryPhy(primaryPhy)
                            .setSecondaryPhy(secondaryPhy)
                            .build();
  return 1;
}

int getPeriodicAdvertiseParametersFromMessage(sd_bus_message *m,
                                              gatt::PeriodicAdvertiseParameters **periodicParameters){
  bool includeTxPowerLevel = false;
  int32_t interval = 0;

  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not a dict, it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the  key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "IncludeTxPowerLevel") == 0) {
      res = getBoolFromMessage(m, &includeTxPowerLevel);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Interval") == 0) {
      res = getInt32FromMessage(m, &interval);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  gatt::PeriodicAdvertiseParameters::Builder builder =
                              gatt::PeriodicAdvertiseParameters::Builder().setInterval(interval)
                                                                          .setIncludeTxPower(includeTxPowerLevel);
  *periodicParameters = builder.build();
  return 1;
}

int getAdvertisingSetEntriesFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **advParameters,
                      gatt::AdvertiseData **advertiseData, gatt::AdvertiseData **scanResponse,
                      gatt::PeriodicAdvertiseParameters **periodicParameters, gatt::AdvertiseData **periodicData,
                      int32_t *duration, int32_t *maxExtAdvEvents){
  char type;
  const char *contents;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "a{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }
  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not array. it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "AdvertisingSetParameters") == 0) {
      res = getAdvertisingSetParametersFromMessage(m, advParameters);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "AdvertiseData") == 0) {
      res = getAdvertiseDataFromMessage(m, advertiseData);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "ScanResponse") == 0) {
      res = getAdvertiseDataFromMessage(m, scanResponse);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "PeriodicParameters") == 0) {
      res = getPeriodicAdvertiseParametersFromMessage(m, periodicParameters);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "PeriodicData") == 0) {
      res = getAdvertiseDataFromMessage(m, periodicData);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "Duration") == 0) {
      res = getInt32FromMessage(m, duration);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "MaxExtAdvEvents") == 0) {
      res = getInt32FromMessage(m, maxExtAdvEvents);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}

int getAdvertisingSetFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **advSetParameters,
                      gatt::AdvertiseData **advertiseData, gatt::AdvertiseData **scanResponse,
                      gatt::PeriodicAdvertiseParameters **periodicParameters, gatt::AdvertiseData **periodicData,
                      int32_t *duration, int32_t *maxExtAdvEvents) {
  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }
    if (strcasecmp(key, "advertisingSet") == 0) {
      res = getAdvertisingSetEntriesFromMessage(m, advSetParameters, advertiseData, scanResponse,
        periodicParameters, periodicData, duration, maxExtAdvEvents);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}


int getGattServiceFromMessage(sd_bus_message *m, GattService **gatt_service) {
  Uuid uuid;
  std::string device_string;
  int32_t instance_id;
  int service_type;
  bool advertise_preferred;
  std::vector<GattCharacteristic *> characteristics_list;
  std::vector<GattService *> included_services;

  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "Uuid") == 0) {
      std::string temp_uuid_string;
      res = getStringFromMessage(m, &temp_uuid_string);
      if (res < 0) {
        return res;
      }
      bool is_valid_uuid;
      uuid = Uuid::FromString(temp_uuid_string, &is_valid_uuid);
      if (!is_valid_uuid) {
        return -EINVAL;
      }
    } else if (strcasecmp(key, "instance") == 0) {
      res = getInt32FromMessage(m, &instance_id);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "advertisePreferred") == 0) {
      res = getBoolFromMessage(m, &advertise_preferred);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "deviceaddress") == 0) {
      res = getStringFromMessage(m, &device_string);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "serviceType") == 0) {
      res = getInt32FromMessage(m, &service_type);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "characteristics") == 0) {
      res = getCharacteristicListFromMessage(m, &characteristics_list);
      if (res < 0) {
        return res;
      }
      //    } else if (strcasecmp(key, "includedServices") == 0) {
      //      res = getServiceListFromMessage(m, &included_services);
      //      if (res < 0) {
      //        return res;
      //      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  GattService *temp_gatt_service = new GattService(uuid, instance_id, service_type);
  if (temp_gatt_service == nullptr) {
    return -1;
  }
  temp_gatt_service->setAdvertisePreferred(advertise_preferred);

  for (auto service : included_services) {
    temp_gatt_service->addIncludedService(service);
  }
  for (auto characteristic : characteristics_list) {
    temp_gatt_service->addCharacteristic(characteristic);
  }
  *gatt_service = temp_gatt_service;
  return 1;
}

int getStringFromMessage(sd_bus_message *m, std::string *return_string) {
  const char *string_property;
  int res;
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT,
      TYPE_TO_STR(SD_BUS_TYPE_STRING));
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &string_property);
  if (res < 0) {
    return res;
  }
  if (return_string != nullptr) {
    return_string->assign(string_property);
  } else {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getUInt32FromMessage(sd_bus_message *m, uint32_t *return_int) {
  int res;
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT,
      TYPE_TO_STR(SD_BUS_TYPE_UINT32));
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_UINT32, return_int);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getInt32FromMessage(sd_bus_message *m, int32_t *return_int) {
  int res;
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT,
      TYPE_TO_STR(SD_BUS_TYPE_INT32));
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_INT32, return_int);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getBoolFromMessage(sd_bus_message *m, bool *return_bool) {
  int res;
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT,
      TYPE_TO_STR(SD_BUS_TYPE_BOOLEAN));
  if (res < 0) {
    return res;
  }
  int b = 0;
  res = sd_bus_message_read_basic(m, SD_BUS_TYPE_BOOLEAN, &b);
  *return_bool = (b!=0);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getCharacteristicListFromMessage(sd_bus_message *m, std::vector<GattCharacteristic *> *return_characteristic_list) {
  char type;
  const char *contents;
  // Get the variant which is the Characteristic list.
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "aa{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array", __func__);

    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict
  for (;;) {
    // each dict is a variant so unpack that first.
    int res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }

    if (type != SD_BUS_TYPE_VARIANT) {
      ALOGE(LOGTAG "%s -> Type not variant", __func__);
      return -EINVAL;
    }

    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    GattCharacteristic *temp_characteristic_ptr;

    // for each dict in the array of dicts add the service.
    res = getCharacteristicFromMessage(m, &temp_characteristic_ptr);
    if (res < 0) {
      return res;
    }

    return_characteristic_list->push_back(temp_characteristic_ptr);

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}

int getServiceListFromMessage(sd_bus_message *m, std::vector<GattService *> *return_service_list) {
  char type;
  const char *contents;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "aa{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not Array", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict array
    if (type != SD_BUS_TYPE_VARIANT) {
      ALOGE(LOGTAG "%s -> Type not variant", __func__);
      return -EINVAL;
    }
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    GattService *temp_service_ptr;

    // for each dict in the array of dicts add the service.
    res = getGattServiceFromMessage(m, &temp_service_ptr);
    if (res < 0) {
      return res;
    }

    return_service_list->push_back(temp_service_ptr);

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}

int getCharacteristicFromMessage(sd_bus_message *m, gatt::GattCharacteristic **return_characteristic) {
  Uuid uuid;
  int32_t instance_id;
  int32_t permissions;
  int32_t properties;
  int32_t keySize;
  int32_t writeType;
  uint8_t *value = nullptr;
  std::vector<GattDescriptor *> descriptors;
  size_t array_size;

  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    ALOGE(LOGTAG "%s -> Type not array. it is:  %c", __func__, type);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      ALOGE(LOGTAG "%s -> Type not array. it is:  %c", __func__, type);
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "Uuid") == 0) {
      std::string temp_uuid_string;
      res = getStringFromMessage(m, &temp_uuid_string);
      if (res < 0) {
        return res;
      }
      bool is_valid_uuid;
      uuid = Uuid::FromString(temp_uuid_string, &is_valid_uuid);
      if (!is_valid_uuid) {
        return -EINVAL;
      }
    } else if (strcasecmp(key, "instance") == 0) {
      res = getInt32FromMessage(m, &instance_id);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "permissions") == 0) {
      res = getInt32FromMessage(m, &permissions);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "properties") == 0) {
      res = getInt32FromMessage(m, &properties);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "keySize") == 0) {
      res = getInt32FromMessage(m, &keySize);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "writeType") == 0) {
      res = getInt32FromMessage(m, &writeType);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "value") == 0) {
      res = getByteArrayFromMessage(m, &value, &array_size);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "descriptors") == 0) {
      res = getDescriptorListFromMessage(m, &descriptors);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  // Construct a new characteristic and assign it to the return pointer;
  GattCharacteristic *temp_char = new GattCharacteristic(uuid, instance_id, properties, permissions);
  if (temp_char == nullptr) {
    return -1;
  }
  temp_char->setKeySize(keySize);
  temp_char->setWriteType(writeType);
  temp_char->setValue(value, array_size);
  for (auto desc : descriptors) {
    temp_char->addDescriptor(desc);
  }
  *return_characteristic = temp_char;
  return 1;
}

int getDescriptorListFromMessage(sd_bus_message *m, std::vector<gatt::GattDescriptor *> *return_desc_list) {
  char type;
  const char *contents;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  if (type != SD_BUS_TYPE_VARIANT) {
    ALOGE(LOGTAG "%s -> Type not variant", __func__);
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Check that we have an array of dicts "aa{sv}"
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict
  for (;;) {
    // each dict is a variant so unpack that first.
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }

    if (type != SD_BUS_TYPE_VARIANT) {
      ALOGE(LOGTAG "%s -> Type not variant", __func__);
      return -EINVAL;
    }

    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    GattDescriptor *temp_desc_ptr;

    // for each dict in the array of dicts add the service.
    res = getDescriptorFromMessage(m, &temp_desc_ptr);
    if (res < 0) {
      return res;
    }

    return_desc_list->push_back(temp_desc_ptr);
    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}

int getDescriptorFromMessage(sd_bus_message *m, gatt::GattDescriptor **return_descriptor) {

  Uuid uuid;
  int32_t instance_id;
  int32_t permissions;
  uint8_t *value;
  char type;
  const char *contents;
  size_t array_size;

  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    if (strcasecmp(key, "Uuid") == 0) {
      std::string temp_uuid_string;
      res = getStringFromMessage(m, &temp_uuid_string);
      if (res < 0) {
        return res;
      }
      bool is_valid_uuid;
      uuid = Uuid::FromString(temp_uuid_string, &is_valid_uuid);
      if (!is_valid_uuid) {
        return -EINVAL;
      }
    } else if (strcasecmp(key, "instance") == 0) {
      res = getInt32FromMessage(m, &instance_id);
      if (res < 0) {
        return res;
      }
    } else if (strcasecmp(key, "permissions") == 0) {
      res = getInt32FromMessage(m, &permissions);
      if (res < 0) {
        return res;
      }

    } else if (strcasecmp(key, "value") == 0) {
      res = getByteArrayFromMessage(m, &value, &array_size);
      if (res < 0) {
        return res;
      }
    } else {
      res = sd_bus_message_skip(m, TYPE_TO_STR(SD_BUS_TYPE_VARIANT));
      if (res < 0) {
        return res;
      }
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  // Construct a new characteristic and assign it to the return pointer;
  GattDescriptor *temp_desc_ptr = new GattDescriptor(uuid, instance_id, permissions);
  if (temp_desc_ptr == nullptr) {
    return -1;
  }
  temp_desc_ptr->setValue(value, array_size);
  *return_descriptor = temp_desc_ptr;

  return 1;
}

int getByteArrayFromMessage(sd_bus_message *m, uint8_t **return_byte_array, size_t *array_size) {

  int res;
  res = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "ay");
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_read_array(m, 'y', return_byte_array, array_size);
  if (res < 0) {
    return res;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }
  return 1;
}

int getArgumentsFromMessage(sd_bus_message *m, SdBus::ArgumentMap &args) {
  char type;
  const char *contents;
  int res = sd_bus_message_peek_type(m, &type, &contents);
  if (res < 0) {
    return res;
  }
  // Enter the dictonary array
  if (type != SD_BUS_TYPE_ARRAY) {
    return -EINVAL;
  }

  res = sd_bus_message_enter_container(m, type, contents);
  if (res < 0) {
    return res;
  }

  // For each dict entry
  for (;;) {
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }
    // Check if it is a dict entry
    if (type != SD_BUS_TYPE_DICT_ENTRY) {
      return -EINVAL;
    }

    // Open Dict Entry
    res = sd_bus_message_enter_container(m, type, contents);
    if (res < 0) {
      return res;
    }

    // Get the Dict key
    const char *key;
    res = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
    if (res < 0) {
      return res;
    }

    // Get the Argument
    SdbusArgument argument;

    // each dict is a variant so unpack that first.
    res = sd_bus_message_peek_type(m, &type, &contents);
    if (res < 0) {
      return res;
    }
    // Break out of loop if end of array
    if (res == 0) {
      break;
    }

    if (type != SD_BUS_TYPE_VARIANT) {
      ALOGE(LOGTAG "%s -> Type not variant", __func__);
      return -EINVAL;
    }

    if (strcasecmp(contents, "b") == 0) {
      res = getBoolFromMessage(m, &argument.value_.type_bool);
      if (res < 0) {
        return res;
      }
      argument.type_ = SdbusArgument::BOOLEAN;
    } else if (strcasecmp(contents, "i") == 0) {
      res = getInt32FromMessage(m, &argument.value_.type_int32);
      if (res < 0) {
        return res;
      }
      argument.type_ = SdbusArgument::INT32;
    } else {
      ALOGE(LOGTAG "%s -> Type not recognised", __func__);
      return -EINVAL;
    }

    res = sd_bus_message_exit_container(m);
    if (res < 0) {
      return res;
    }
    // add the argument to the argument map;
    args[std::string(key)] = argument;
  }
  res = sd_bus_message_exit_container(m);
  if (res < 0) {
    return res;
  }

  return 1;
}

int PopulateArgumentMessage(sd_bus_message *message, SdBus::ArgumentMap &args) {
  // Open the array of entries
  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "{sv}");
  if (r < 0) {
    return r;
  }

  for (auto argument : args) {
    int r;
    std::string key = argument.first;
    switch (argument.second.type_) {
      case SdBus::SdbusArgument::BOOLEAN:
        r = AppendDictEntry(message, key, argument.second.value_.type_bool);
        if (r < 0)
          return r;
        break;
      case SdBus::SdbusArgument::INT32:
        r = AppendDictEntry(message, key, argument.second.value_.type_int32);
        if (r < 0)
          return r;
        break;
      default:
        break;
    }
  }

  // Close the Array
  r = sd_bus_message_close_container(message);
  if (r < 0) {
    return r;
  }
  return 1;
}

int PopulateGattServices(sd_bus_message *message, std::vector<GattService *> services)
{

  int r = sd_bus_message_open_container(message, SD_BUS_TYPE_ARRAY, "v");
  if (r < 0) {
    return r;
  }

  for (auto service : services) {
    r = sd_bus_message_open_container(message, SD_BUS_TYPE_VARIANT, "a{sv}");
    if (r < 0)
      return r;

    r = PopulateGattServiceMessage(message, service);
    if (r < 0)
      return r;

    r = sd_bus_message_close_container(message);
    if (r < 0)
      return r;
  }
  r = sd_bus_message_close_container(message);
  if (r < 0)
    return r;
}

}  // mamespace SdBus
