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

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include "common-test-cli.h"
#include "gatt-server-test-service.hpp"


int main (int argc, char *argv[]) {

  sigset_t signal_set;
  if (sigemptyset(&signal_set) < 0) {
    fprintf(stdout, "***ERROR! Unable to create process signal mask\n");
    std::exit(EXIT_FAILURE);
  }
  if (sigaddset(&signal_set, SIGINT) < 0) {
    fprintf(stdout, "***ERROR! Unable to add to process signal mask\n");
    std::exit(EXIT_FAILURE);
  }
  if (sigaddset(&signal_set, SIGTERM) < 0) {
    fprintf(stdout, "***ERROR! Unable to add to process signal mask\n");
    std::exit(EXIT_FAILURE);
  }
  if (sigprocmask(SIG_BLOCK, &signal_set, nullptr) < 0) {
    fprintf(stdout, "***ERROR! ERROR: Unable to set process signal mask\n");
    std::exit(EXIT_FAILURE);
  }
  // Init cli thread
  fprintf(stdout, "Starting bluetooth-le-gatt-client-test\n");

  CommonTestCli common_test_cli;
  fprintf(stdout, "Created CommonTestCli\n");
  common_test_cli.init();
  fprintf(stdout, "Initialised common_test_cli\n");


  GattServerTestService test_service;
  fprintf(stdout, "Created GattServerTestService\n");

  test_service.RegisterCli(&common_test_cli);
  fprintf(stdout, "Registered common_test_cli to test_service\n");

  bool success;
  // Main loop waiting for signals
  fprintf(stdout, "Main loop waiting for signals\n");

  bool waiting_for_signals = true;
  while (waiting_for_signals) {
    // Block until a signal arrives
    int signal_number;
    fprintf(stdout, "sigwait\n");
    int error = sigwait(&signal_set, &signal_number);
    fprintf(stdout, "Signal received\n");
    // Check there was no error
    if (error) {
      success = false;
      break;
    }

    // Check which signal it was
    switch (signal_number) {
      case SIGINT:
      case SIGTERM:
        // Exit loop, terminate gracefully
        waiting_for_signals = false;
        break;

      default:
        waiting_for_signals = false;
        success = false;
        break;
    }
  }
  common_test_cli.Shutdown();

  return EXIT_SUCCESS;
}
