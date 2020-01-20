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

#ifndef COMMONTESTCLI_H
#define COMMONTESTCLI_H

#include <functional>
#include <map>
#include <memory>
#include <thread>
#include <vector>

using std::string;

typedef std::function<void(int, std::vector<string>)> MenuCallback;

struct cli_message {
  int menu_selection_id;
  std::vector<string> arguments;
  bool stop = false;
};

class CommonTestCli {
  public:
  ~CommonTestCli();
  void RegisterMenu(std::map<string, int>, std::map<int, int> argument_map, MenuCallback);
  explicit CommonTestCli();
  bool init();
  bool Shutdown();

  private:
  struct Private;
  std::unique_ptr<Private> self_;
  void StartInputWorkerThread();
  void InputWorkerThread();
  void MainWorkerThread();
  void StartMainWorkerThread();
};

#endif  // COMMONTESTCLI_H
