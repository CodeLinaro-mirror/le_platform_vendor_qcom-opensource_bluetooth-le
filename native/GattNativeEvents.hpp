/******************************************************************************
 *
 *  Copyright (c) 2016-2019, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once

#include "GattNativeDefines.hpp"

using std::string;


#define GATT_MSG_BASE           (3000)
#define GATT_MSG_LAST           (4000)

/**
 *  list of NATIVE EVENTS 
 */
typedef enum {
    BTGATTS_REGISTER_APP_EVENT = GATT_MSG_BASE,
    BTGATTS_CONNECTION_EVENT,
    BTGATTS_SERVICE_ADDED_EVENT,
    BTGATTS_INCLUDED_SERVICE_ADDED_EVENT,
    BTGATTS_CHARACTERISTIC_ADDED_EVENT,
    BTGATTS_DESCRIPTOR_ADDED_EVENT,
    BTGATTS_SERVICE_STARTED_EVENT,
    BTGATTS_SERVICE_STOPPED_EVENT,
    BTGATTS_SERVICE_DELETED_EVENT,
    BTGATTS_REQUEST_READ_CHARACTERISTIC_EVENT,
    BTGATTS_REQUEST_READ_DESCRIPTOR_EVENT,
    BTGATTS_REQUEST_WRITE_CHARACTERISTIC_EVENT,
    BTGATTS_REQUEST_WRITE_DESCRIPTOR_EVENT,
    BTGATTS_REQUEST_EXEC_WRITE_EVENT,
    BTGATTS_RESPONSE_CONFIRMATION_EVENT,
    BTGATTS_INDICATION_SENT_EVENT,
    BTGATTS_CONGESTION_EVENT,
    BTGATTS_MTU_CHANGED_EVENT,
    BTGATTS_PHY_UPDATED_EVENT,
    BTGATTS_CONN_UPDATED_EVENT,
    BTGATTS_READ_PHY_EVENT,

    BTGATTC_REGISTER_APP_EVENT,
    BTGATTC_OPEN_EVENT,
    BTGATTC_CLOSE_EVENT,
    BTGATTC_SEARCH_COMPLETE_EVENT,
    BTGATTC_SEARCH_RESULT_EVENT,
    BTGATTC_GET_CHARACTERISTIC_EVENT,
    BTGATTC_GET_DESCRIPTOR_EVENT,
    BTGATTC_GET_INCLUDED_SERVICE_EVENT,
    BTGATTC_REGISTER_FOR_NOTIFICATION_EVENT,
    BTGATTC_NOTIFY_EVENT,
    BTGATTC_READ_CHARACTERISTIC_EVENT,
    BTGATTC_WRITE_CHARACTERISTIC_EVENT,
    BTGATTC_READ_DESCRIPTOR_EVENT,
    BTGATTC_WRITE_DESCRIPTOR_EVENT,
    BTGATTC_EXECUTE_WRITE_EVENT,
    BTGATTC_REMOTE_RSSI_EVENT,
    BTGATTC_CONFIGURE_MTU_EVENT,
    BTGATTC_CONGESTION_EVENT,
    BTGATTC_GET_GATT_DB_EVENT,
    BTGATTC_PHY_UPDATED_EVENT,
    BTGATTC_CONN_UPDATED_EVENT,
    BTGATTC_READ_PHY_EVENT,

    BLEADVERTISER_SET_ADVERTISING_DATA_EVENT,
    BLEADVERTISER_SET_SCAN_RESPONSE_DATA_EVENT,
    BLEADVERTISER_SET_PERIODIC_ADVERTISING_DATA_EVENT,
    BLEADVERTISER_SET_PERIODIC_ADVERTISING_PARAMETER_EVENT,
    BLEDAVERTISER_GET_OWN_ADDRESS_EVENT,
    BLEDAVERTISER_ADVERTISING_SET_ENABLE_EVENT,
    BLEADVERTISER_ADVERTISING_PARAMETER_UPDATED_EVENT,
    BLEDAVERTISER_ADVERTISING_SET_START_EVENT,
    BLEDAVERTISER_PERIODIC_ADVERTISING_SET_ENABLE_EVENT,

    BLESCANNER_REGISTER_SCANNER_EVENT,
    BLESCANNER_SCAN_PARAMS_COMPLETE_EVENT,
    BLESCANNER_SCAN_RESULT_EVENT,
    BLESCANNER_SCAN_FILTER_CFG_EVENT,
    BLESCANNER_SCAN_FILTER_PARAM_EVENT,
    BLESCANNER_SCAN_FILTER_STATUS_EVENT,
    BLESCANNER_BATCHSCAN_CFG_STORAGE_EVENT,
    BLESCANNER_BATCHSCAN_START_EVENT,
    BLESCANNER_BATCHSCAN_STOP_EVENT,
    BLESCANNER_BATCHSCAN_REPORTS_EVENT,
    BLESCANNER_BATCHSCAN_THRESHOLD_EVENT,
    BLESCANNER_TRACK_ADV_EVENT_EVENT,
    BLESCANNER_PERIODIC_ADVERTISING_SYNC_START_EVENT,
    BLESCANNER_PERIODIC_ADVERTISING_SYNC_LOST_EVENT,
    BLESCANNER_PERIODIC_ADVERTISING_SYNC_REPORT_EVENT,
    BLESCANNER_BATCHSCAN_TIMEOUT_EVENT,
	GATT_EVENT_ADAPTER_PROPERTIES,

    EXIT = GATT_MSG_LAST // Last command enum
} BluetoothEventId;

/**
 * GATT client  Structures
 */
typedef struct{
    BluetoothEventId event_id;
    int status;
    int clientIf;
    bt::Uuid app_uuid;
} GattcRegisterAppEvent;


typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    int clientIf;
    string *bda;
} GattcOpenEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    int clientIf;
    string *bda;
} GattcCloseEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
} GattcSearchCompleteEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    gatt::gatt_srvc_id_t srvc_id;
} GattcSearchResultEvent;

typedef struct{
   BluetoothEventId event_id;
   int conn_id;
   int status;
   gatt::gatt_srvc_id_t srvc_id;
   gatt::gatt_id_t char_id;
   int char_prop;
} GattcGetCharacteristicEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    gatt::gatt_srvc_id_t srvc_id;
    gatt::gatt_id_t char_id;
    gatt::gatt_id_t descr_id;
} GattcGetDescriptorEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    gatt::gatt_srvc_id_t srvc_id;
    gatt::gatt_srvc_id_t incl_srvc_id;
} GattcGetIncludedServiceEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int registered;
    int status;
    int handle;
} GattcRegisterForNotificationEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    gatt::gatt_notify_params_t p_data;
} GattcNotifyEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    gatt::gatt_read_params_t p_data;
} GattcReadCharacteristicEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    int handle;
} GattcWriteCharacteristicEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
} GattcExecuteWriteEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    gatt::gatt_read_params_t p_data;
} GattcReadDescriptorEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    uint16_t handle;
} GattcWriteDescriptorEvent;

typedef struct{
    BluetoothEventId event_id;
    int client_if;
    string *bda;
    int rssi;
    int status;
} GattcRemoteRssiEvent;

typedef struct{
    BluetoothEventId event_id;
    int status;
    int client_if;
} GattcAdvertiseEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    int status;
    int mtu;
} GattcConfigureMtuEvent;

typedef struct{
    BluetoothEventId event_id;
    int conn_id;
    bool congested;
} GattcCongestionEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    gatt::gatt_db_element_t *db;
    int count;
} GattcGetGattDbEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    uint8_t tx_phy;
    uint8_t rx_phy;
    uint8_t status;
} GattcPhyUpdatedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint8_t status;
} GattcConnUpdatedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int clientIf;
    string *bda;
    uint8_t tx_phy;
    uint8_t rx_phy;
    uint8_t status;
} GattcReadPhyEvent;

typedef struct{
    BluetoothEventId event_id;
} GattcEvent;

/* GATT Server Structures */
typedef struct {
    BluetoothEventId event_id;
    int status;
    int server_if;
    bt::Uuid uuid;
} GattsRegisterAppEvent;

typedef struct {
    BluetoothEventId event_id;
    int conn_id;
    int server_if;
    int connected;
    string *bda;
} GattsConnectionEvent;

typedef struct {
    BluetoothEventId event_id;
    int status;
    int server_if;
    std::vector<gatt::gatt_db_element_t> *service;
} GattsServiceAddedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    int srvc_handle;
    int incl_srvc_handle;
} GattsIncludedServiceAddedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    bt::Uuid char_id;
    int srvc_handle;
    int char_handle;
} GattsCharacteristicAddedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    bt::Uuid descr_id;
    int srvc_handle;
    int descr_handle;
} GattsDescriptorAddedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    int srvc_handle;
} GattsServiceStartedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    int srvc_handle;
} GattsServiceStoppedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int server_if;
    int srvc_handle;
} GattsServiceDeletedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int trans_id;
    string *bda;
    int attr_handle;
    int offset;
    bool is_long;
} GattsRequestReadCharacteristicEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int trans_id;
    string *bda;
    int attr_handle;
    int offset;
    bool is_long;
} GattsRequestReadDescriptorEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int trans_id;
    string *bda;
    int attr_handle;
    int offset;
    bool need_rsp;
    bool is_prep;
    std::vector<uint8_t> *value;
} GattsRequestWriteCharacteristicEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int trans_id;
    string *bda;
    int attr_handle;
    int offset;
    bool need_rsp;
    bool is_prep;
    std::vector<uint8_t> *value;
} GattsRequestWriteDescriptorEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int trans_id;
    string *bda;
    int exec_write;
} GattsRequestExecWriteEvent;

typedef struct
{
    BluetoothEventId event_id;
    int status;
    int handle;
} GattsResponseConfirmationEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int status;
} GattsIndicationSentEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    bool congested;
} GattsCongestionEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    int mtu;
} GattsMTUchangedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    uint8_t tx_phy;
    uint8_t rx_phy;
    uint8_t status;
} GattsPhyUpdatedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int conn_id;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint8_t status;
} GattsConnUpdatedEvent;

typedef struct
{
    BluetoothEventId event_id;
    int serverIf;
    string *bda;
    uint8_t tx_phy;
    uint8_t rx_phy;
    uint8_t status;
} GattsReadPhyEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
} BleAdvertiserSetAdvDataEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
} BleAdvertiserSetScanRespEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
} BleAdvertiserSetPeriodicAdvDataEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
} BleAdvertiserSetPeriodicAdvParamEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t address_type;
    string *bda;
} BleAdvertiserGetOwnAddressEvent;

typedef struct
{
    BluetoothEventId event_id;
    int reg_id;
    uint8_t advertiser_id;
    int8_t tx_power;
    uint8_t status;
} BleAdvertiserAdvSetStartEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
    bool isEnabled;
} BleAdvertiserAdvSetEnableEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    int8_t tx_power;
    uint8_t status;
} BleAdvertiserAdvSetParamUpdateEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint8_t advertiser_id;
    uint8_t status;
    bool isEnabled;
} BleAdvertiserPeriodicAdvSetEnableEvent;

typedef struct{
    BluetoothEventId event_id;
    bt::Uuid app_uuid;
    uint8_t scannerId;
    uint8_t status;
} BleScannerRegisterScannerEvent;

typedef struct{
    BluetoothEventId event_id;
    int client_if;
    uint8_t status;
} BleScannerScanParamCompleteEvent;

typedef struct{
    BluetoothEventId event_id;
    uint16_t event_type;
    uint8_t addr_type;
    string *bda;
    uint8_t primary_phy;
    uint8_t secondary_phy;
    uint8_t advertising_sid;
    int8_t tx_power;
    int8_t rssi;
    uint16_t periodic_adv_int;
    std::vector<uint8_t> *adv_data;
} BleScannerScanResultEvent;

typedef struct{
    BluetoothEventId event_id;
    int action;
    int client_if;
    int status;
    int filt_type;
    int avbl_space;
} BleScannerScanFilterCfgEvent;

typedef struct{
    BluetoothEventId event_id;
    int action;
    int client_if;
    int status;
    int avbl_space;
} BleScannerScanFilterParamEvent;

typedef struct{
    BluetoothEventId event_id;
    int action;
    int client_if;
    int status;
} BleScannerScanFilterStatusEvent;

typedef struct{
    BluetoothEventId event_id;
    int client_if;
    int status;
} BleScannerBatchscanCfgStorageEvent;

typedef struct
{
    BluetoothEventId event_id;
    int client_if;
    int status;
} BleScannerBatchscanStartEvent;

typedef struct
{
    BluetoothEventId event_id;
    int client_if;
    int status;
} BleScannerBatchscanStopEvent;

typedef struct
{
    BluetoothEventId event_id;
    int client_if;
    int status;
    int report_format;
    int num_records;
    std::vector<uint8_t> *data;
} BleScannerBatchscanReportsEvent;

typedef struct
{
    BluetoothEventId event_id;
    int client_if;
} BleScannerBatchscanThresholdEvent;

typedef struct
{
    BluetoothEventId event_id;
    gatt::gatt_track_adv_info_t p_adv_track_info;
} BleScannerTrackAdvEvent;

typedef struct
{
    BluetoothEventId event_id;
    void *scanmanager;
} BleScannerBatchscantimeoutEvent;

typedef struct
{
    BluetoothEventId event_id;
    int reg_id;
    uint8_t status;
    uint16_t sync_handle;
    uint8_t sid;
    uint8_t address_type;
    string *bda;
    uint8_t phy;
    uint16_t interval;
} BleScannerPeriodicAdvSyncStartEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint16_t sync_handle;
} BleScannerPeriodicAdvSyncLostEvent;

typedef struct
{
    BluetoothEventId event_id;
    uint16_t sync_handle;
    int8_t tx_power;
    int8_t rssi;
    uint8_t data_status;
    std::vector<uint8_t> *data;
} BleScannerPeriodicAdvSyncReportEvent;

typedef struct {
  BluetoothEventId event_id;
  int type;
  int len;
  const void *val;
} GattAdapterPropertyEvent;


typedef union {
    BluetoothEventId                        event_id;
    GattsRegisterAppEvent                   gatts_register_app_event;
    GattsConnectionEvent                    gatts_connection_event;
    GattsServiceAddedEvent                  gatts_service_added_event;
    GattsIncludedServiceAddedEvent          gatts_included_service_added_event;
    GattsCharacteristicAddedEvent           gatts_characteristic_added_event;
    GattsDescriptorAddedEvent               gatts_descriptor_added_event;
    GattsServiceStartedEvent                gatts_service_started_event;
    GattsServiceStoppedEvent                gatts_service_stopped_event;
    GattsServiceDeletedEvent                gatts_service_deleted_event;
    GattsRequestReadCharacteristicEvent     gatts_request_read_characteristic_event;
    GattsRequestReadDescriptorEvent         gatts_request_read_descriptor_event;
    GattsRequestWriteCharacteristicEvent    gatts_request_write_characteristic_event;
    GattsRequestWriteDescriptorEvent        gatts_request_write_descriptor_event;
    GattsRequestExecWriteEvent              gatts_request_exec_write_event;
    GattsResponseConfirmationEvent          gatts_response_confirmation_event;
    GattsIndicationSentEvent                gatts_indication_sent_event;
    GattsCongestionEvent                    gatts_congestion_event;
    GattsMTUchangedEvent                    gatts_mtu_changed_event;
    GattsPhyUpdatedEvent                    gatts_phy_updated_event;
    GattsConnUpdatedEvent                   gatts_conn_updated_event;
    GattsReadPhyEvent                       gatts_read_phy_event;

    GattcRegisterAppEvent                   gattc_register_app_event;
    GattcOpenEvent                          gattc_open_event;
    GattcCloseEvent                         gattc_close_event;
    GattcSearchCompleteEvent                gattc_search_complete_event;
    GattcSearchResultEvent                  gattc_search_result_event;
    GattcGetCharacteristicEvent             gattc_get_characteristic_event;
    GattcGetDescriptorEvent                 gattc_get_descriptor_event;
    GattcGetIncludedServiceEvent            gattc_get_included_service_event;
    GattcRegisterForNotificationEvent       gattc_register_for_notification_event;
    GattcNotifyEvent                        gattc_notify_event;
    GattcReadCharacteristicEvent            gattc_read_characteristic_event;
    GattcWriteCharacteristicEvent           gattc_write_characteristic_event;
    GattcReadDescriptorEvent                gattc_read_descriptor_event;
    GattcWriteDescriptorEvent               gattc_write_descriptor_event;
    GattcExecuteWriteEvent                  gattc_execute_write_event;
    GattcRemoteRssiEvent                    gattc_remote_rssi_event;
    GattcAdvertiseEvent                     gattc_advertise_event;
    GattcConfigureMtuEvent                  gattc_configure_mtu_event;
    GattcCongestionEvent                    gattc_congestion_event;
    GattcGetGattDbEvent                     gattc_get_gatt_db_event;
    GattcPhyUpdatedEvent                    gattc_phy_updated_event;
    GattcConnUpdatedEvent                   gattc_conn_updated_event;
    GattcReadPhyEvent                       gattc_read_phy_event;

    BleAdvertiserSetAdvDataEvent            bleadverister_set_adv_data_event;
    BleAdvertiserSetScanRespEvent           bleadverister_set_scan_resp_event;
    BleAdvertiserSetPeriodicAdvDataEvent    bleadverister_set_periodic_adv_data_event;
    BleAdvertiserSetPeriodicAdvParamEvent   bleadverister_set_periodic_adv_param_event;
    BleAdvertiserGetOwnAddressEvent         bleadvertiser_get_own_address_event;
    BleAdvertiserAdvSetEnableEvent          bleadvertiser_adv_set_enable_event;
    BleAdvertiserAdvSetStartEvent           bleadvertiser_adv_set_start_event;
    BleAdvertiserAdvSetParamUpdateEvent     bleadvertiser_adv_set_param_update_event;
    BleAdvertiserPeriodicAdvSetEnableEvent  bleadverister_periodic_adv_set_enable_event;

    BleScannerRegisterScannerEvent          blescanner_register_scanner_event;
    BleScannerScanParamCompleteEvent        blescanner_scan_param_complete_event;
    BleScannerScanResultEvent               blescanner_scan_result_event;
    BleScannerScanFilterCfgEvent            blescanner_scan_filter_cfg_event;
    BleScannerScanFilterParamEvent          blescanner_scan_filter_param_event;
    BleScannerScanFilterStatusEvent         blescanner_scan_filter_status_event;
    BleScannerBatchscanCfgStorageEvent      blescanner_batchscan_cfg_storage_event;
    BleScannerBatchscanStartEvent           blescanner_batchscan_start_event;
    BleScannerBatchscanStopEvent            blescanner_batchscan_stop_event;
    BleScannerBatchscanReportsEvent         blescanner_batchscan_reports_event;
    BleScannerBatchscanThresholdEvent       blescanner_batchscan_threshold_event;
    BleScannerTrackAdvEvent                 blescanner_track_adv_event;
    BleScannerPeriodicAdvSyncStartEvent     blescanner_periodic_adv_sync_start_event;
    BleScannerPeriodicAdvSyncLostEvent      blescanner_periodic_adv_sync_lost_event;
    BleScannerPeriodicAdvSyncReportEvent    blescanner_periodic_adv_sync_report_event;
    BleScannerBatchscantimeoutEvent         BleScanner_batchscan_timeout_Event; //TODO: Anything else added recently?
    GattAdapterPropertyEvent                gatt_adapter_property_event;
} BtEvent;
