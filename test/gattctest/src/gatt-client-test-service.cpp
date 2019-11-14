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

#include "gatt-client-test-service.hpp"
#include <cstdint>
#include <iostream>
#include <cstring>

typedef enum {
  ZERO_PARAM,
  ONE_PARAM,
  TWO_PARAM,
  THREE_PARAM,
  FOUR_PARAM,
  FIVE_PARAM,
  SIX_PARAM,
} MaxParamCount;

GattClientTestService *g_test_service;

bool GattClientTestService::RegisterCli(CommonTestCli *common_test_cli) {
  if (common_test_cli == nullptr) {
    fprintf(stdout, "common_test_cli == nullptr\n");
    return false;
  }

  common_test_cli_ = common_test_cli;
  common_test_cli_->RegisterMenu(menu_, argument_map_, CliCallbackHandler);

  return true;
}

void GattClientTestService::CliCallbackHandler(int id, std::vector<string> arguments) {
  switch (id) {
    case GATTCTEST_INIT:
      fprintf(stdout, "ENABLE GATTCTEST \n");
      if (g_test_service->gattctest_ptr_ != nullptr) {
        fprintf(stdout, "gattctest_ptr_ already initialized \n");
        return;
      } else {
        // fprintf(stdout, "gattctest not initialized \n");
        // if (g_test_service->gatt_lib_service_ptr_) {
        //   fprintf(stdout, "gatt_lib_service_ptr_ != nullptr");
          // g_test_service->gattctest_ptr_ = new GattcTest(g_test_service->gatt_lib_service_ptr_);
          g_test_service->gattctest_ptr_ = new GattcTest();

          if (g_test_service->gattctest_ptr_) {
            fprintf(stdout, "enableGattctest\n");
            g_test_service->gattctest_ptr_->gattctest = g_test_service->gattctest_ptr_;
            g_test_service->gattctest_ptr_->enableGattctest();
            fprintf(stdout, " EnableGATTCTEST done \n");
          } else {
            fprintf(stdout, " GATTCTEST Alloc failed return failure \n");
          }
        // } else {
        //   fprintf(stdout, " gatt interface us null \n");
        // }
      }
      break;
    case GATTCTEST_SCAN_FILTER:
      fprintf(stdout, "Scan filter \n");
      if (g_test_service->gattctest_ptr_) {
        bool status = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper ScanFilter Type\n");
          break;
        }
        fprintf(stdout, "Do scan filtering\n");
        g_test_service->gattctest_ptr_->scanFilter(atoi(arguments[ONE_PARAM].c_str()),
            arguments[TWO_PARAM]);
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_SCANFILTER_MAN_DATA:
      fprintf(stdout, "Scan filter Manu Data\n");
      if (g_test_service->gattctest_ptr_) {
        bool status = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper Filter Type\n");
          break;
        }
        fprintf(stdout, "Do scan filtering\n");
        g_test_service->gattctest_ptr_->scanFilterManuData(atoi(arguments[ONE_PARAM].c_str()),
            arguments[TWO_PARAM], arguments[THREE_PARAM]);
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_SCAN_SETTINGS:
      fprintf(stdout, "Scan settings \n");
      if (g_test_service->gattctest_ptr_) {
        bool status = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper scanSetting Type\n");
          break;
        }
        status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper setting value \n");
          break;
        }
        fprintf(stdout, "Do scan settings\n");
        g_test_service->gattctest_ptr_->scanSettings(atoi(arguments[ONE_PARAM].c_str()),
            atoi(arguments[TWO_PARAM].c_str()));
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_CONN_PARAMS:
      fprintf(stdout, "Connection parameters \n");
      if (g_test_service->gattctest_ptr_) {
        bool status = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper auto value\n");
          break;
        }
        status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper phy value\n");
          break;
        }
        status = g_test_service->gattctest_ptr_->validateInput(arguments[THREE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper oppurtunistic value\n");
          break;
        }
        if ((atoi(arguments[ONE_PARAM].c_str()) != 0) && (atoi(arguments[ONE_PARAM].c_str()) != 1)) {
          ALOGW(LOGTAG "Enter correct auto value (0/1)");
          fprintf(stdout, "Enter correct auto value (0/1)\n");
        } else if ((atoi(arguments[THREE_PARAM].c_str()) != 0) && (atoi(arguments[THREE_PARAM].c_str()) != 1)) {
          ALOGW(LOGTAG "Enter correct oppurtunistic value (0/1)");
          fprintf(stdout, "Enter correct oppurtunistic value (0/1)\n");
        } else {
          g_test_service->gattctest_ptr_->gattConnParams((bool)(atoi(arguments[ONE_PARAM].c_str())),
              atoi(arguments[TWO_PARAM].c_str()),
              (bool)atoi(arguments[THREE_PARAM].c_str()));
        }
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_START_SCAN:
      fprintf(stdout, "trying to start scan \n");
      if (g_test_service->gattctest_ptr_) {
        fprintf(stdout, "starting scan \n");
        g_test_service->gattctest_ptr_->startScan();
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_BATCH_SCAN:
      fprintf(stdout, "trying to start batch scan \n");
      if (g_test_service->gattctest_ptr_) {
        bool valid = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
        if (!valid) {
          fprintf(stdout, "Enter proper auto value\n");
          break;
        }
        fprintf(stdout, "starting batch scan \n");
        g_test_service->gattctest_ptr_->testBatchscan(atoi(arguments[ONE_PARAM].c_str()));
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    case GATTCTEST_STOP_SCAN:
      if (g_test_service->gattctest_ptr_) {
        fprintf(stdout, "stopping scan \n");
        g_test_service->gattctest_ptr_->stopScan();
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;

    case GATTCTEST_CONNECT:
      fprintf(stdout, "GATTCTEST_CONNECT arguments count %d\n", arguments.size());
      if (arguments.size() <= ONE_PARAM) {
        fprintf(stdout, " BD address is NULL/Invalid \n");
        break;
      }
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        fprintf(stdout, "string_is_bdaddr\n");
        if (g_test_service->gattctest_ptr_ != nullptr) {
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper Transport Value\n");
            break;
          }
        } else {
          fprintf(stdout, "g_test_service->gattctest_ptr_ == nullptr\n");
        }
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "connecting \n");
          g_test_service->gattctest_ptr_->gattConnect(arguments[ONE_PARAM],
              atoi(arguments[TWO_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      }
      break;

    case GATTCTEST_DISCONNECT:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "disconnecting \n");
          g_test_service->gattctest_ptr_->gattDisconnect(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_DISCSRVC:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Discovering services \n");
          g_test_service->gattctest_ptr_->gattDiscoverServices(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_DISCSRVC_UUID:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Discovering services by uuid\n");
          Uuid uuid = uuid.FromString((arguments[TWO_PARAM]), NULL);
          g_test_service->gattctest_ptr_->gattDiscoverServicesByUuid(uuid, arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_RDCHAR_UUID:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "reading char by uuid\n");
          Uuid uuid = uuid.FromString((arguments[TWO_PARAM]), NULL);
          g_test_service->gattctest_ptr_->readCharacteristicUUID(arguments[ONE_PARAM], uuid);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_READPHY:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Reading PHY \n");
          g_test_service->gattctest_ptr_->gattClientReadPhy(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_READRSSI:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Reading RSSI \n");
          g_test_service->gattctest_ptr_->gattReadRemoteRssi(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_REFRESH:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "REFRESHING \n");
          g_test_service->gattctest_ptr_->gattRefresh(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_REQMTU:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper MTU Value\n");
            break;
          }
          fprintf(stdout, "REQUESTING MTU \n");
          g_test_service->gattctest_ptr_->gattrequestMtu(arguments[ONE_PARAM],
              atoi(arguments[TWO_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_SETPHY:
      if (string_is_bdaddr(arguments[THREE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Setting PHY \n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[ONE_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper TX Value\n");
            break;
          }
          status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper RX Value\n");
            break;
          }
          g_test_service->gattctest_ptr_->setPreferredPhy(atoi(arguments[ONE_PARAM].c_str()),
              atoi(arguments[TWO_PARAM].c_str()), 1, arguments[THREE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_GETSERVICES:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "getting services \n");
          g_test_service->gattctest_ptr_->getServices(arguments[ONE_PARAM]);
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_GETSRVC: {
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        Uuid uuid = uuid.FromString((arguments[TWO_PARAM]), NULL);
        bool status = g_test_service->gattctest_ptr_->validateInput(arguments[THREE_PARAM]);
        if (!status) {
          fprintf(stdout, "Enter proper instanceID\n");
          break;
        }
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "getting service  \n");
          g_test_service->gattctest_ptr_->getService(arguments[ONE_PARAM], uuid,
              atoi(arguments[THREE_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    }
    case GATTCTEST_RDWRDESC: {
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Reading writing DESC\n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper Read/Write Type\n");
            break;
          }
          status = g_test_service->gattctest_ptr_->validateInput(arguments[FOUR_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter instanceId\n");
            break;
          }
          int i = atoi(arguments[TWO_PARAM].c_str());
          int j = atoi(arguments[FOUR_PARAM].c_str());
          fprintf(stdout, "instanceid %d\n", j);
          if (i == 1) {
            g_test_service->gattctest_ptr_->writeDescriptor(arguments[ONE_PARAM],
                (uint8_t *)&(arguments[THREE_PARAM]), arguments[THREE_PARAM].length()+1, j);
          } else if (i == 2) {
            g_test_service->gattctest_ptr_->readDescriptor(arguments[ONE_PARAM],
                atoi(arguments[FOUR_PARAM].c_str()));
          } else {
            fprintf(stdout,
                "Enter the correct 2nd parameter.."
                "1 -write , 2 -read\n");
          }
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    }
    case GATTCTEST_RDWRCHAR: {
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Reading writing char\n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper Read/Write Type\n");
            break;
          }
          status = g_test_service->gattctest_ptr_->validateInput(arguments[FOUR_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter instanceId\n");
            break;
          }
          int i = atoi(arguments[TWO_PARAM].c_str());
          int j = atoi(arguments[FOUR_PARAM].c_str());
          fprintf(stdout, "instanceid %d\n", j);
          if (i == 1) {
            g_test_service->gattctest_ptr_->writeCharacteristic(arguments[ONE_PARAM],
                (uint8_t *)&(arguments[THREE_PARAM]), arguments[THREE_PARAM].length()+1,
                j);
          } else if (i == 2) {
            g_test_service->gattctest_ptr_->readCharacteristic(arguments[ONE_PARAM],
                atoi(arguments[FOUR_PARAM].c_str()));
          } else if (i == 3) {
            g_test_service->gattctest_ptr_->prepareWriteCharacteristic(arguments[ONE_PARAM],
                (uint8_t *)&(arguments[THREE_PARAM]), arguments[THREE_PARAM].length()+1, j);
          } else {
            fprintf(stdout,
                "Enter the correct 2nd parameter.."
                "1 -write , 2 -read\n");
          }
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, " BD address is NULL/Invalid \n");
      }
      break;
    }
    case GATTCTEST_GETCHARID:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "getting Characteristic \n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper InstanceID\n");
            break;
          }
          g_test_service->gattctest_ptr_->getCharacteristicById(arguments[ONE_PARAM],
              atoi(arguments[TWO_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_RELIABLEWRITE:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper InstanceID\n");
            break;
          }
          fprintf(stdout, "Reliablewrite Characteristic \n");
          g_test_service->gattctest_ptr_->reliableWrite(arguments[ONE_PARAM],
              atoi(arguments[TWO_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_GETDESCID:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "getting Descriptor \n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter proper InstanceID\n");
            break;
          }
          g_test_service->gattctest_ptr_->getDescriptorById(arguments[ONE_PARAM],
              atoi(arguments[TWO_PARAM].c_str()));
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_REQCONN_PRI:
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattctest_ptr_) {
          fprintf(stdout, "Requesting Connection Priority \n");
          bool status = g_test_service->gattctest_ptr_->validateInput(arguments[TWO_PARAM]);
          if (!status) {
            fprintf(stdout, "Enter Connection Priority\n");
            break;
          }
          int i = atoi(arguments[TWO_PARAM].c_str());
          if (i >= 0 && i <= 2) {
            g_test_service->gattctest_ptr_->reqConnPri(arguments[ONE_PARAM],
                atoi(arguments[TWO_PARAM].c_str()));
          } else {
            fprintf(stdout, "Enter 0/1/2 as priority\n");
          }
        } else {
          fprintf(stdout, "Do the GATTCINIT first\n");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTCTEST_CONN_DEVICES:
      if (g_test_service->gattctest_ptr_) {
        fprintf(stdout, "Listing Connected devices \n");
        g_test_service->gattctest_ptr_->list_conn_devices();
      } else {
        fprintf(stdout, "Do the GATTCINIT first\n");
      }
      break;
    default:
      fprintf(stdout, " Command not handled");
      break;
  }
}

GattClientTestService::GattClientTestService() {
  menu_ = {
      {"gattctest_init", GATTCTEST_INIT},
      {"gattctest_scanset", GATTCTEST_SCAN_SETTINGS},
      {"gattctest_scanFilter", GATTCTEST_SCAN_FILTER},
      {"gattctest_scanFilter_manData", GATTCTEST_SCANFILTER_MAN_DATA},
      {"gattctest_start_scan", GATTCTEST_START_SCAN},
      {"gattctest_stop_scan", GATTCTEST_STOP_SCAN},
      {"gattctest_batch_scan", GATTCTEST_BATCH_SCAN},
      {"gattctest_conn_params", GATTCTEST_CONN_PARAMS},
      {"gattctest_connect", GATTCTEST_CONNECT},
      {"gattctest_disconnect", GATTCTEST_DISCONNECT},
      {"gattctest_discsrvc", GATTCTEST_DISCSRVC},
      {"gattctest_rdchar_uuid", GATTCTEST_RDCHAR_UUID},
      {"gattctest_readPhy", GATTCTEST_READPHY},
      {"gattctest_readrssi", GATTCTEST_READRSSI},
      {"gattctest_reqMtu", GATTCTEST_REQMTU},
      {"gattctest_refresh", GATTCTEST_REFRESH},
      {"gattctest_setphy", GATTCTEST_SETPHY},
      {"gattctest_getservices", GATTCTEST_GETSERVICES},
      {"gattctest_reqconn_pri", GATTCTEST_REQCONN_PRI},
      {"gattctest_getcharid", GATTCTEST_GETCHARID},
      {"gattctest_reliablewrite", GATTCTEST_RELIABLEWRITE},
      {"gattctest_getdescid", GATTCTEST_GETDESCID},
      {"gattctest_getsrvc", GATTCTEST_GETSRVC},
      {"gattctest_RdWrDesc", GATTCTEST_RDWRDESC},
      {"gattctest_RdWrchar", GATTCTEST_RDWRCHAR},
      {"gattctest_conn_dev", GATTCTEST_CONN_DEVICES},
  };
  argument_map_ = {
      {GATTCTEST_INIT, ZERO_PARAM},
      {GATTCTEST_SCAN_SETTINGS,TWO_PARAM},
      {GATTCTEST_SCAN_FILTER, TWO_PARAM},
      {GATTCTEST_SCANFILTER_MAN_DATA, THREE_PARAM},
      {GATTCTEST_START_SCAN, ZERO_PARAM},
      {GATTCTEST_STOP_SCAN, ZERO_PARAM},
      {GATTCTEST_BATCH_SCAN, ONE_PARAM},
      {GATTCTEST_CONN_PARAMS, THREE_PARAM},
      {GATTCTEST_CONNECT, TWO_PARAM},
      {GATTCTEST_DISCONNECT, ONE_PARAM},
      {GATTCTEST_DISCSRVC, ONE_PARAM},
      {GATTCTEST_RDCHAR_UUID, TWO_PARAM},
      {GATTCTEST_READPHY, ONE_PARAM},
      {GATTCTEST_READRSSI, ONE_PARAM},
      {GATTCTEST_REQMTU, TWO_PARAM},
      {GATTCTEST_REFRESH, ONE_PARAM},
      {GATTCTEST_SETPHY, THREE_PARAM},
      {GATTCTEST_GETSERVICES, ONE_PARAM},
      {GATTCTEST_REQCONN_PRI, TWO_PARAM},
      {GATTCTEST_GETCHARID, TWO_PARAM},
      {GATTCTEST_RELIABLEWRITE, TWO_PARAM},
      {GATTCTEST_GETDESCID, TWO_PARAM},
      {GATTCTEST_GETSRVC, THREE_PARAM},
      {GATTCTEST_RDWRDESC, FOUR_PARAM},
      {GATTCTEST_RDWRCHAR, FOUR_PARAM},
      {GATTCTEST_CONN_DEVICES, ZERO_PARAM},

  };
  g_test_service = this;
}

GattClientTestService::~GattClientTestService() {
}
