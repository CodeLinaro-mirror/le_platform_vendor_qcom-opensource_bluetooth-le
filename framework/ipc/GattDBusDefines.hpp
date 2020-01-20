/*
Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.

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

#define GATT_MANAGER_SERVICE_NAME "com.qualcomm.qti.Ble"
#define GATT_SERVER_LISTENER_SERVICE_NAME "com.qualcomm.qti.BleListeners"

#define GATT_SERVER_MANAGER_INTERFACE "com.qualcomm.qti.Ble.GattServerManager"
#define GATT_SERVER_LISTENER_INTERFACE "com.qualcomm.qti.BleListeners.GattServer"
#define GATT_SERVER_MANAGER_OBJECT_PATH "/com/qualcomm/qti/Ble/GattServerManager"
#define GATT_SERVER_LISTENER_OBJECT_PATH "/com/qualcomm/qti/BleListeners/GattServerManagerListener"

#define GATT_SERVER_ADVERTISER_MANAGER_INTERFACE "com.qualcomm.qti.Ble.LeAdvertiserManager"
#define GATT_SERVER_ADVERTISER_LISTENER_INTERFACE "com.qualcomm.qti.BleListeners.LeAdvertiser"
#define GATT_SERVER_ADVERTISER_MANAGER_OBJECT_PATH "/com/qualcomm/qti/Ble/LeAdvertiserManager"
#define BLE_LISTENER_OBJECT_PATH "/com/qualcomm/qti/BleListeners/LeAdvertiserListener"

#define GATT_CLIENT_LISTENER_NAME "com.qualcomm.qti.BleListeners"
#define GATT_CLIENT_LISTENER_INTERFACE "com.qualcomm.qti.BleListeners.GattClient"
#define GATT_CLIENT_LISTENER_OBJECT_PATH "/com/qualcomm/qti/BleListeners/GattClientListener"
#define GATT_CLIENT_MANAGER_INTERFACE "com.qualcomm.qti.Ble.GattClientManager"
#define GATT_CLIENT_MANAGER_OBJECT_PATH "/com/qualcomm/qti/Ble/GattClientManager"

#define GATT_CLIENT_SCANNER_MANAGER_INTERFACE "com.qualcomm.qti.Ble.LeScannerManager"
#define GATT_CLIENT_SCANNER_LISTENER_INTERFACE "com.qualcomm.qti.BleListeners.LeScanner"
#define GATT_CLIENT_SCANNER_MANAGER_OBJECT_PATH "/com/qualcomm/qti/Ble/LeScannerManager"
#define BLE_CLIENT_LISTENER_OBJECT_PATH "/com/qualcomm/qti/BleListeners/LeScannerListener"
