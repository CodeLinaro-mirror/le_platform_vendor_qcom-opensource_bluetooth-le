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

#include "gatt-server-test-service.hpp"
#include <cstdint>
#include <iostream>

typedef enum {
  ZERO_PARAM,
  ONE_PARAM,
  TWO_PARAM,
  THREE_PARAM,
  FOUR_PARAM,
  FIVE_PARAM,
  SIX_PARAM,
} MaxParamCount;

GattServerTestService *g_test_service;


bool GattServerTestService::RegisterCli(CommonTestCli *common_test_cli) {
  if (common_test_cli == nullptr) {
    fprintf(stdout, "common_test_cli == nullptr\n");
    return false;
  }

  common_test_cli_ = common_test_cli;
  common_test_cli_->RegisterMenu(menu_, argument_map_, CliCallbackHandler);

  fprintf(stdout, "getting gattlib service instance\n");
  return true;
}
int server_num;

void GattServerTestService::CliCallbackHandler(int id, std::vector<string> arguments) {
  static bool init_server_file = 0;
  static bool init_advertiser_file = 0;
  static bool file_read = 0;

  switch (id) {
    case GATTSTEST_INIT_SERVER:
      fprintf(stdout, "ENABLE GATTSTEST\n");
      if (g_test_service->gattstest_ptr_) {
        fprintf(stdout, "Gattstest already initialized \n");
        return;
      } else {
          fprintf(stdout, "Initializing Gattstest \n");
          g_test_service->gattstest_ptr_ = new GattsTest();
          if (g_test_service->gattstest_ptr_) {
            fprintf(stdout, "Reading Server Configuration File .... \n");
            g_test_service->gattstest_ptr_->ReadServerConfigurationFile();
            init_server_file = true;
          } else {
            fprintf(stdout, " GATTSTEST Alloc failed return failure \n");
          }

      }

      break;
    case GATTSTEST_ADDSERVER:
        if (g_test_service->gattstest_ptr_) {
          if (init_server_file) {
            server_num++;
            fprintf(stdout, "Adding Server %d \n", server_num);
            g_test_service->gattstest_ptr_->AddServer();
          } else {
            fprintf(stdout, "Do g_test_service->gattstest_ptr__init_server first \n");
          }
        } else {
          fprintf(stdout, "Do Init first\n");
        }

      break;
    case GATTSTEST_ADDSERVICES:
      if (g_test_service->gattstest_ptr_) {
        fprintf(stdout, "AddServices \n");
        string server_instance = arguments[ONE_PARAM];
        string service_instance = arguments[TWO_PARAM];
        bool result = g_test_service->gattstest_ptr_->AddService(server_instance, service_instance);
        if (!result) {
          fprintf(stdout, "Service could not be added\n");
        }
      } else {
        fprintf(stdout, "Do Init first\n");
      }

      break;
    case GATTSTEST_INIT_ADVERTISER:
      if (g_test_service->gattstest_ptr_) {
        fprintf(stdout, "Initialize Advertiser \n");
        file_read = g_test_service->gattstest_ptr_->ReadAdvertiserConfigFile();
        init_advertiser_file = true;
        if (file_read)
          fprintf(stdout, "File read Succcessfully \n");
        else
          fprintf(stdout, "File not read \n");
      } else {
        fprintf(stdout, "Do Init first\n");
      }

      break;
    case GATTSTEST_START_ADVERTISER:
      if (g_test_service->gattstest_ptr_) {
        fprintf(stdout, "StartAdvertisement \n");
        string server_instance = arguments[ONE_PARAM];
        if (file_read && init_advertiser_file) {
          bool result = g_test_service->gattstest_ptr_->StartAdvertisement(server_instance);
          if (!result) {
            fprintf(stdout, "Advertisement has not started\n");
          }
        } else {
          fprintf(stdout, "Do init Advertiser first \n");
        }
      } else {
        fprintf(stdout, "Do Init first\n");
      }

      break;
    case GATTSTEST_READPHY:
      fprintf(stdout, "Read Phy \n");
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattstest_ptr_) {
          ALOGE(LOGTAG "g_test_service->gattstest_ptr_->ReadPhy");
          fprintf(stdout, "User input is %s \n", arguments[ONE_PARAM]);
          string server_instance = arguments[TWO_PARAM];
          bool status = g_test_service->gattstest_ptr_->ReadPhy(server_instance, arguments[ONE_PARAM]);
          if (!status) {
            fprintf(stdout, "tx/rx phy could not be read \n");
          }
        } else {
          fprintf(stdout, "Do Init first \n ");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTSTEST_SET_PREFERRED_PHY:
      fprintf(stdout, "Set Preferred Phy \n");
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattstest_ptr_) {
          string deviceAddress = arguments[ONE_PARAM];
          string server_instance = arguments[TWO_PARAM];
          string txOption = arguments[THREE_PARAM];
          string rxOption = arguments[FOUR_PARAM];
          int phyOption = AdvertisingSetParameters::PHY_OPTION_NO_PREFERRED;
          fprintf(stdout, "the user options are address: %s server_instance: %s txoption: %s rxoption: %s phyoption: %d \n", deviceAddress.c_str(), server_instance.c_str(), txOption.c_str(), rxOption.c_str(), phyOption);
          bool status = g_test_service->gattstest_ptr_->SetPreferredPhy(deviceAddress, server_instance, txOption, rxOption, phyOption);
          if (!status) {
            fprintf(stdout, "Phy preferences were not set \n");
          }
        } else {
          fprintf(stdout, "Do Init first \n ");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }
      break;
    case GATTSTEST_STOP:
      if (g_test_service->gattstest_ptr_) {
        fprintf(stdout, "Stop Advertisement \n");
        string server_instance = arguments[ONE_PARAM];
        g_test_service->gattstest_ptr_->StopAdvertisement(server_instance);
      } else {
        fprintf(stdout, "Do Init first \n ");
      }
      break;
    case GATTSTEST_UNREGISTER_SERVER:
      fprintf(stdout, "Unregister Server \n");
      if (g_test_service->gattstest_ptr_) {
        bool status = g_test_service->gattstest_ptr_->UnregisterServer(arguments[ONE_PARAM]);
        server_num--;
        if (status) {
          fprintf(stdout, "Server unregistered succesfully \n");
        } else {
          fprintf(stdout, "Server not unregistered\n");
        }
      } else {
        fprintf(stdout, "Do Init first \n ");
      }

      break;
    case GATTSTEST_DISABLE:
      fprintf(stdout, "Disable Gattstest \n");
      if (g_test_service->gattstest_ptr_) {
        g_test_service->gattstest_ptr_->DisableGATTSTEST();
        delete g_test_service->gattstest_ptr_;
        g_test_service->gattstest_ptr_ = NULL;
        server_num = 0;
      } else {
        fprintf(stdout, "Do Init first \n ");
      }

      break;
    case GATTSTEST_CANCEL_CONNECTION:
      fprintf(stdout, "Cancel Connection \n");
      if (string_is_bdaddr(arguments[ONE_PARAM].c_str())) {
        if (g_test_service->gattstest_ptr_) {
          string deviceAddress = arguments[ONE_PARAM];
          g_test_service->gattstest_ptr_->CancelConnection(deviceAddress);
        } else {
          fprintf(stdout, "Do Init first \n ");
        }
      } else {
        fprintf(stdout, "BD address is NULL/Invalid \n");
      }

      break;
    default:
      fprintf(stdout, " Command not handled");
      break;
  }
}

GattServerTestService::GattServerTestService() {
  menu_ = {
      {"gattstest_init_server", GATTSTEST_INIT_SERVER},
      {"gattstest_addservers", GATTSTEST_ADDSERVER},
      {"gattstest_addservices", GATTSTEST_ADDSERVICES},
      {"gattstest_init_advertiser", GATTSTEST_INIT_ADVERTISER},
      {"gattstest_start_advertiser", GATTSTEST_START_ADVERTISER},
      {"gattstest_readphy", GATTSTEST_READPHY},
      {"gattstest_set_preferred_phy", GATTSTEST_SET_PREFERRED_PHY},
      {"gattstest_stop", GATTSTEST_STOP},
      {"gattstest_disable", GATTSTEST_DISABLE},
      {"gattstest_cancel_connection", GATTSTEST_CANCEL_CONNECTION},
      {"gattstest_unregister_Server", GATTSTEST_UNREGISTER_SERVER},
  };

  argument_map_ = {
      {GATTSTEST_INIT_SERVER, ZERO_PARAM},
      {GATTSTEST_ADDSERVER, ZERO_PARAM},
      {GATTSTEST_ADDSERVICES, TWO_PARAM},
      {GATTSTEST_INIT_ADVERTISER, ZERO_PARAM},
      {GATTSTEST_START_ADVERTISER, ONE_PARAM},
      {GATTSTEST_READPHY, TWO_PARAM},
      {GATTSTEST_SET_PREFERRED_PHY, FOUR_PARAM},
      {GATTSTEST_STOP, ONE_PARAM},
      {GATTSTEST_DISABLE, ZERO_PARAM},
      {GATTSTEST_CANCEL_CONNECTION, ONE_PARAM},
      {GATTSTEST_UNREGISTER_SERVER, ONE_PARAM},
  };
  g_test_service = this;
}

GattServerTestService::~GattServerTestService() {
  if (g_test_service->gattstest_ptr_) {
    g_test_service->gattstest_ptr_->DisableGATTSTEST();
    delete g_test_service->gattstest_ptr_;
    g_test_service->gattstest_ptr_ = NULL;
    server_num = 0;
  }
}
