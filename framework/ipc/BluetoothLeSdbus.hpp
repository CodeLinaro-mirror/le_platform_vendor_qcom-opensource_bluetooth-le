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

#ifndef BLUETOOTH_LE_SDBUS_HPP
#define BLUETOOTH_LE_SDBUS_HPP

#include <systemdq/sd-bus.h>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include "GattCharacteristic.hpp"
#include "GattDescriptor.hpp"
#include "GattService.hpp"
#include "AdvertisingSetParameters.hpp"
#include "AdvertiseData.hpp"
#include "PeriodicAdvertiseParameters.hpp"
#include "uuid.h"

using gatt::GattService;
using gatt::GattDescriptor;
using gatt::GattCharacteristic;

#define TYPE_TO_STR(...) ((const char[]){__VA_ARGS__, 0})

namespace SdBus {

struct SdbusArgument {
  enum ArgumentType {
    BOOLEAN,
    INT32,
  };
  union ArgumentValue {
    std::int32_t type_int32;
    bool type_bool;
  };
  ArgumentType type_;
  ArgumentValue value_;
};
//typedef std::map<std::string, SdbusArgument> ArgumentMap;
using ArgumentMap = std::map<std::string, SdbusArgument>;

#ifdef __cplusplus
extern "C" {
#endif
int getGattServiceFromMessage(sd_bus_message *m, GattService **gatt_service);

int getStringFromMessage(sd_bus_message *sd_bus_message_ptr, std::string *return_string);
int getUInt32FromMessage(sd_bus_message *sd_bus_message_ptr, uint32_t *return_int);
int getInt32FromMessage(sd_bus_message *sd_bus_message_ptr, int32_t *return_int);
int getBoolFromMessage(sd_bus_message *sd_bus_message_ptr, bool *return_bool);
int getByteArrayFromMessage(sd_bus_message *sd_bus_message_ptr, uint8_t **return_byte_array, size_t *array_size);

int getArgumentsFromMessage(sd_bus_message *sd_bus_message_ptr, SdBus::ArgumentMap &args);
int getCharacteristicListFromMessage(sd_bus_message *sd_bus_message_ptr, std::vector<GattCharacteristic *> *return_string_vector);
int getServiceListFromMessage(sd_bus_message *sd_bus_message_ptr, std::vector<GattService *> *return_string_vector);

int getCharacteristicFromMessage(sd_bus_message *sd_bus_message_ptr, GattCharacteristic **return_characteristic);

int getDescriptorListFromMessage(sd_bus_message *sd_bus_message_ptr, std::vector<GattDescriptor *> *return_desc_list);
int getDescriptorFromMessage(sd_bus_message *sd_bus_message_ptr, GattDescriptor **return_descriptor);
int PopulateGattServices (sd_bus_message *message, std::vector<GattService *> services);
int PopulateGattServiceMessage(sd_bus_message *message, GattService *svc);
int PopulateGattCharacteristic(sd_bus_message *message, gatt::GattCharacteristic *gatt_characteristic);
int PopulateGattDescriptor(sd_bus_message *message, gatt::GattDescriptor *gatt_descriptor);
int PopulateArgumentMessage(sd_bus_message *message, ArgumentMap &args);

// LE Adveritiser
int getServiceUuidsFromMessage(sd_bus_message *m, vector<bt::Uuid> *serviceUuids);
int getServiceDataFromMessage(sd_bus_message *m, std::map<bt::Uuid, std::vector<uint8_t>> *serviceData);
int getAdvertiseDataFromMessage(sd_bus_message *m, gatt::AdvertiseData **advData);
int getAdvertisingSetParametersFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **advSetParameters);
int getPeriodicAdvertiseParametersFromMessage(sd_bus_message *m, gatt::PeriodicAdvertiseParameters **periodicParameters);
int getAdvertisingSetEntriesFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **parameters,
                      gatt::AdvertiseData **advertiseData, gatt::AdvertiseData **scanResponse,
                      gatt::PeriodicAdvertiseParameters **periodicParameters, gatt::AdvertiseData **periodicData,
                      int32_t *duration, int32_t *maxExtAdvEvents);
int getAdvertisingSetFromMessage(sd_bus_message *m, gatt::AdvertisingSetParameters **parameters,
                      gatt::AdvertiseData **advertiseData, gatt::AdvertiseData **scanResponse,
                      gatt::PeriodicAdvertiseParameters **periodicParameters, gatt::AdvertiseData **periodicData,
                      int32_t *duration, int32_t *maxExtAdvEvents);

int PopulateLeAdvertisingSetParameters(sd_bus_message *message, gatt::AdvertisingSetParameters *params);
int PopulateLeAdvertiseData(sd_bus_message *message, gatt::AdvertiseData *advData);
int PopulateLePeriodicAdvertiseParameters(sd_bus_message *message, gatt::PeriodicAdvertiseParameters *params);
int PopulateAdvertisingSet(sd_bus_message *message, gatt::AdvertisingSetParameters *parameters,
        gatt::AdvertiseData *advertiseData, gatt::AdvertiseData *scanResponse,
        gatt::PeriodicAdvertiseParameters *periodicParameters, gatt::AdvertiseData *periodicData,
        uint32_t duration, uint32_t maxExtAdvEvents);
#ifdef __cplusplus
}
#endif

int AppendDictEntry(sd_bus_message *message, std::string key, bool value);
int AppendDictEntry(sd_bus_message *message, std::string key, uint8_t *value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::string value);
int AppendDictEntry(sd_bus_message *message, std::string key, uint32_t value);
int AppendDictEntry(sd_bus_message *message, std::string key, int32_t value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<bt::Uuid> value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::map<bt::Uuid, std::vector<uint8_t>> value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattService *> value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattCharacteristic *> value);
int AppendDictEntry(sd_bus_message *message, std::string key, std::vector<gatt::GattDescriptor *> value);
}

#endif  //BLUETOOTH_LE_SDBUS_HPP
