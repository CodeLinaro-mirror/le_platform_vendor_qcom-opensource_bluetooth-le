/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef GATT_NATIVE_INTERFACE_V2_HPP
#define GATT_NATIVE_INTERFACE_V2_HPP

#pragma once
#include "GattNativeDefines.hpp"
#include <hardware/bluetooth.h>
#include <hardware/bt_gatt.h>

namespace gatt {

class IGattNativeV2 {
  public:
    virtual int gattClientGetDeviceTypeNative(string address) = 0;
    virtual void gattClientRegisterAppNative(bt::Uuid uuid) = 0;
    virtual void gattClientUnregisterAppNative(int clientIf) = 0;
    virtual void registerScannerNative(bt::Uuid uuid) = 0;
    virtual void unregisterScannerNative(int scanner_id) = 0;
    virtual void gattClientScanNative(bool start) = 0;
    virtual void gattClientConnectNative(int clientif, string address, bool isDirect,
        int transport, bool opportunistic, int initiating_phys) = 0;
    virtual void gattClientDisconnectNative(int clientIf, string address, int conn_id) = 0;
    virtual void gattClientSetPreferredPhyNative(int clientIf, string address,
        int tx_phy, int rx_phy, int phy_options) = 0;
    virtual void gattClientReadPhyNative(int clientIf, string address) = 0;
    virtual void gattClientRefreshNative(int clientIf, string address) = 0;
    virtual void gattClientSearchServiceNative(int conn_id, bool search_all, bt::Uuid uuid) = 0;
    virtual void gattClientDiscoverServiceByUuidNative(int conn_id, bt::Uuid uuid) = 0;
    virtual void gattClientGetGattDbNative(int conn_id) = 0;
    virtual void gattClientReadCharacteristicNative(int conn_id, int handle, int authReq) = 0;
    virtual void gattClientReadUsingCharacteristicUuidNative(
        int conn_id, bt::Uuid uuid, int s_handle, int e_handle, int authReq) = 0;
    virtual void gattClientReadDescriptorNative(int conn_id, int handle, int authReq) = 0;
    virtual void gattClientWriteCharacteristicNative(int conn_id, int handle,
        int write_type, int auth_req, std::vector<uint8_t> vect_val) = 0;
    virtual void gattClientExecuteWriteNative(int conn_id, bool execute) = 0;
    virtual void gattClientWriteDescriptorNative(int conn_id, int handle, int auth_req,
        std::vector<uint8_t> vect_val) = 0;
    virtual void gattClientRegisterForNotificationsNative(
        int clientIf, string address, int handle, bool enable) = 0;
    virtual void gattClientReadRemoteRssiNative(int clientif, string address) = 0;
    virtual void gattSetScanParametersNative(int client_if, int scan_phy,
        std::vector<uint32_t> scan_interval, std::vector<uint32_t> scan_window) = 0;
    virtual void getOwnAddressNative(int advertiser_id) = 0;
    virtual void gattClientScanFilterParamAddNative(uint8_t client_if, uint8_t filt_index,
         std::unique_ptr<btgatt_filt_param_setup_t> filt_params) = 0;
    virtual void gattClientScanFilterParamDeleteNative(uint8_t client_if, uint8_t filt_index) = 0;
    virtual void gattClientScanFilterParamClearAllNative(uint8_t client_if) = 0;
    virtual void gattClientScanFilterAddNative(int client_if,int filter_index,
        std::vector<apcf_command_t> filters) = 0;
    virtual void gattClientScanFilterClearNative(int client_if, int filt_index) = 0;
    virtual void gattClientScanFilterEnableNative(int client_if, bool enable) = 0;
    virtual void gattClientConfigureMTUNative(int conn_id, int mtu) = 0;
    virtual void gattConnectionParameterUpdateNative(int client_if, string address,
        int min_interval,int max_interval, int latency, int timeout, int min_ce_len,
        int max_ce_len) = 0;
    virtual void gattClientConfigBatchScanStorageNative(int client_if, int max_full_reports_percent,
        int max_trunc_reports_percent, int notify_threshold_level_percent) = 0;
    virtual void gattClientStartBatchScanNative(int client_if, int scan_mode,
        int scan_interval_unit, int scan_window_unit, int addr_type, int discard_rule) = 0;
    virtual void gattClientStopBatchScanNative(int client_if) = 0;
    virtual void gattClientReadScanReportsNative(int client_if, int scan_type) = 0;
    virtual void gattServerRegisterAppNative(bt::Uuid uuid) = 0;
    virtual void gattServerUnregisterAppNative(int serverIf) = 0;
    virtual void gattServerConnectNative(int server_if, string address, bool is_direct,
        int transport) = 0;
    virtual void gattServerDisconnectNative(int serverIf, string address, int conn_id) = 0;
    virtual void gattServerSetPreferredPhyNative(int serverIf, string address,
        int tx_phy, int rx_phy, int phy_options) = 0;
    virtual void gattServerReadPhyNative(int serverIf, string address) = 0;
    virtual void gattServerAddServiceNative(int server_if, std::vector<gatt_db_element_t> service) = 0;
    virtual void gattServerStopServiceNative(int server_if, int svc_handle) = 0;
    virtual void gattServerDeleteServiceNative(int server_if, int svc_handle) = 0;
    virtual void gattServerSendIndicationNative(int server_if, int attr_handle,
       int conn_id, std::vector<uint8_t> vect_val) = 0;
    virtual void gattServerSendNotificationNative(int server_if, int attr_handle,
      int conn_id, std::vector<uint8_t> vect_val) = 0;
    virtual void gattServerSendResponseNative(int server_if, int conn_id,
        int trans_id, int status, int handle, int offset,
        std::vector<uint8_t> vect_val, int auth_req) = 0;
    virtual void startAdvertisingSetNative(advertise_parameters_t params, std::vector<uint8_t> adv_data,
        std::vector<uint8_t> scan_resp, periodic_advertising_parameters_t periodic_params,
        std::vector<uint8_t> periodic_data, int duration, int maxExtAdvEvents, int reg_id) = 0;
    virtual void stopAdvertisingSetNative(int advertiser_id) = 0;
    virtual void enableAdvertisingSetNative(int advertiser_id, bool enable, int duration, int maxExtAdvEvents) = 0;
    virtual void setAdvertisingDataNative(int advertiser_id, std::vector<uint8_t> data) = 0;
    virtual void setScanResponseDataNative(int advertiser_id, std::vector<uint8_t> data) = 0;
    virtual void setAdvertisingParametersNative(int advertiser_id, advertise_parameters_t parameters) = 0;
    virtual void setPeriodicAdvertisingParametersNative(int advertiser_id, periodic_advertising_parameters_t periodic_parameters) = 0;
    virtual void setPeriodicAdvertisingDataNative(int advertiser_id,std::vector<uint8_t> data) = 0;
    virtual void setPeriodicAdvertisingEnableNative(int advertiser_id, bool enable) = 0;
    virtual void startSyncNative(int sid, string address, int skip, int timeout, int reg_id) = 0;
    virtual void stopSyncNative(int sync_handle) = 0;
    virtual void gattTestNative(int command, bt::Uuid uuid1, string bda1, int p1, int p2, int p3, int p4, int p5) = 0;
};
}  // namespace gatt

#endif
