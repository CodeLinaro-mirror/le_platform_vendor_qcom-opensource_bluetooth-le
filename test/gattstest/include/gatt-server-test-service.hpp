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

#ifndef GATT_SERVER_TEST_SERVICE_HPP_
#define GATT_SERVER_TEST_SERVICE_HPP_

#include <cstdint>

#include <string>
#include <vector>
#include "GattsTest.hpp"
#include "common-test-cli.h"
#include "AdvertisingSet.hpp"

using std::string;

class GattServerTestService {
  enum GattServerMenuOptions {
    GATTSTEST_OPTION,
    GATTSTEST_INIT_SERVER,
    GATTSTEST_ADDSERVER,
    GATTSTEST_ADDSERVICES,
    GATTSTEST_INIT_ADVERTISER,
    GATTSTEST_START_ADVERTISER,
    GATTSTEST_READPHY,
    GATTSTEST_SET_PREFERRED_PHY,
    GATTSTEST_STOP,
    GATTSTEST_UNREGISTER_SERVER,
    GATTSTEST_DISABLE,
    GATTSTEST_CANCEL_CONNECTION,
  };

  public:
  bool RegisterCli(CommonTestCli *common_test_cli);
  GattServerTestService();
  ~GattServerTestService();
  static void CliCallbackHandler(int id, std::vector<string> arguments);

  private:
  CommonTestCli *common_test_cli_;
  std::map<string, int> menu_;
  std::map<int, int> argument_map_;

  GattsTest *gattstest_ptr_ = nullptr;
};

#endif  GATT_SERVER_TEST_SERVICE_HPP_
