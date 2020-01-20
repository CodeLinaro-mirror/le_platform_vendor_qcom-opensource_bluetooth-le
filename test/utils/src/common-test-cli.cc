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

#include "common-test-cli.h"
#include <pthread.h>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "msgqueue.hpp"

using std::cin;
using std::string;

struct CommonTestCli::Private {
  explicit Private() {}
  std::thread input_thread_;
  std::thread main_worker_thread_;
  std::map<string, int> menu_map_;
  std::map<int, int> argument_map_;
  MenuCallback menu_callback_;
  gatt::MsgQueue<cli_message> cli_message_queue_;

  void PrintMenu();
};

void CommonTestCli::Private::PrintMenu() {
  if (menu_map_.empty()) {
    return;
  }
  for (auto menu_entry : menu_map_) {
    std::cout << menu_entry.first << std::endl;
  }
}

CommonTestCli::~CommonTestCli() = default;

CommonTestCli::CommonTestCli()
    : self_(new Private()) {}

void CommonTestCli::RegisterMenu(std::map<string, int> menu_map, std::map<int, int> argument_map, MenuCallback menu_callback) {
  self_->menu_map_ = menu_map;
  self_->argument_map_ = argument_map;
  self_->menu_callback_ = menu_callback;
}

bool CommonTestCli::init() {
  StartInputWorkerThread();
  StartMainWorkerThread();
  return true;
}

bool CommonTestCli::Shutdown() {
  if (self_->input_thread_.joinable()) {
    pthread_cancel(self_->input_thread_.native_handle());
    self_->input_thread_.join();
  }
  if (self_->main_worker_thread_.joinable()) {
    cli_message stop_message;
    stop_message.stop = true;
    self_->cli_message_queue_.add(stop_message);
    self_->main_worker_thread_.join();
  }
  return true;
}

void CommonTestCli::StartInputWorkerThread() {
  self_->input_thread_ = std::thread([this]() { InputWorkerThread(); });
}

void CommonTestCli::StartMainWorkerThread() {
  self_->main_worker_thread_ = std::thread([this]() { MainWorkerThread(); });
}

void CommonTestCli::MainWorkerThread() {
  bool done = false;
  while (!done) {
    auto message = self_->cli_message_queue_.remove();
    if (message.stop) {
      done = true;
    } else {
      if (self_->menu_callback_ != nullptr) {
        self_->menu_callback_(message.menu_selection_id, message.arguments);
      }
    }
  }
}

void CommonTestCli::InputWorkerThread() {
  bool done = false;
  while (!done) {
    string input_string;
    std::getline(cin, input_string);
    //std::cout << "hhhh" << std::endl;
    std::istringstream iss(input_string);
    std::vector<std::string> results;
    string argument;
    char delim = ' ';
    while (std::getline(iss, argument, delim)) {
      results.push_back(argument);
    }
    if (results.empty()) {
      std::cout << "Please Select an option" << std::endl;
      self_->PrintMenu();
      continue;
    }
    if (self_->menu_map_.empty()) {
      continue;
    }
    auto menu_item = self_->menu_map_.find(results[0]);
    if (menu_item == self_->menu_map_.end()) {
      std::cout << "Invalid option" << std::endl;
      self_->PrintMenu();
      continue;
    }
    int menu_id = menu_item->second;

    auto argument_entry = self_->argument_map_.find(menu_id);
    if (argument_entry == self_->argument_map_.end()) {
      std::cout << "Argument number map is not valid" << std::endl;
      self_->PrintMenu();
      continue;
    }
    int number_of_arguments_expected = argument_entry->second;
    if ((results.size() - 1) != number_of_arguments_expected) {
      std::cout << "Wrong number of arguments" << std::endl;
      self_->PrintMenu();
      continue;
    }
    cli_message new_cli_message;
    new_cli_message.menu_selection_id = menu_id;
    new_cli_message.arguments = results;
    self_->cli_message_queue_.add(new_cli_message);
  }
}
