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

#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <iterator>
#include <regex>
#include <systemdq/sd-bus.h>

#include "GattsTest.hpp"
#include "AdvertiseSettings.hpp"
#include "AdvertiseData.hpp"
#include "AdvertisingSetCallback.hpp"
#include "AdvertisingSet.hpp"
#include "AdvertisingSetParameters.hpp"
#include "PeriodicAdvertiseParameters.hpp"
#include "GattLeAdvertiser.hpp"


using namespace std;
using namespace gatt;

#define LOGTAG "GATTSTEST "
#define UNUSED

#define SERVER_CFG_FILE_PATH "/data/misc/bluetooth/ServerConfigFile.txt"
#define ADV_CFG_FILE_PATH "/data/misc/bluetooth/AdvertiserConfigFile.txt"
#define GATT_SUCCESS 0
#define AUTO_CONNECT 0
#define TRANSPORT 0
#define MAX_SERVER_INSTANCE 20
#define MAX_SERVICE_INSTANCE 5
#define INVALID_VALUE -1
#define VALID_VALUE 0
#define PERIODIC_INTERVAL 200
#define PHY_LE_1M 1
#define PHY_LE_2M 2
#define PHY_LE_CODED 3
#define PROPERTY_READ 2
#define SERVICE_LINE_MIN 0
#define SERVICE_LINE_MAX 4
#define MANUFACTURER_ID_LINE 5
#define MANUFACTURER_DATA_LINE 6
#define SERVICE_1 1
#define SERVICE_2 2
#define SERVICE_3 3
#define SERVICE_4 4
#define SERVICE_5 5


GattsTest *gattstest = NULL;
int num_of_server;
int num_of_devices;
int num_of_advertiser = 0;
GattServer *mgattServer = NULL;
gattstestServerCallback *gattstestServerCb = NULL;

sd_bus *bus = NULL;


map<int, AdvertisingSet*> advSetMap;
vector <string> connectedDevices;
unordered_map < gattstestServerCallback*, GattServer*> servCBInstanceMap;

map<gattstestServerCallback*,string> connectedDeviceMap;
map<string,GattServer*> DeviceMap;


vector <string> service_field;
list <GattCharacteristic> mCharList;

AdvertiseSettings *mAdvertiseSettings = NULL;
AdvertiseData *mAdvertiseData = NULL;
AdvertiseData *mScanResponseData = NULL;
AdvertiseData *mPeriodicData = NULL;
string receivedData;
string receivedDescValue;
GattCharacteristic *executeWriteChar;
GattDescriptor *executeWriteDesc;



AdvertisingSetParameters *mAdvertisingParameters;
PeriodicAdvertiseParameters *mPeriodicParams;
AdvertisingSet *mAdvertisingSet;

bool string_is_bdaddr(const char *string) {
 // assert(string != NULL);

  size_t len = strlen(string);
  if (len != 17)
    return false;

  for (size_t i = 0; i < len; ++i) {
    // Every 3rd char must be ':'.
    if (((i + 1) % 3) == 0 && string[i] != ':')
      return false;

    // All other chars must be a hex digit.
    if (((i + 1) % 3) != 0 && !isxdigit(string[i]))
      return false;
  }
  return true;
}

bool split (const string &s, char c,vector<string> &v)
{
  string::size_type i = 0;
  string::size_type j = s.find(c);
  //if comma separator found at the first index then the entry is invalid
  if ( j == 0 ) {
    return false;
  }
  //if comma separator not found in entire string
  if( j == string::npos) {
    v.push_back(s);
    return true;
  }
  while(j != string::npos) {
    v.push_back(s.substr(i,j-i));
    i = ++j;
    j = s.find(c,j);
    if(j == string::npos) {
      v.push_back(s.substr(i,s.length()));
      return true;
    }
  }
}

void gattstestServerCallback::onConnectionStateChange(string deviceAddress, int status,
                                                               int newState)
{
  bool connected= false;
  string address;
  GattServer *mServer;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator ptr;
  map <string,GattServer*> ::iterator dtr = DeviceMap.find(deviceAddress);
  map<gattstestServerCallback*,string> ::iterator iter;
  iter =connectedDeviceMap.find(gattstestServerCb);
  vector <string> ::iterator it;
  it = find(connectedDevices.begin(),connectedDevices.end(),deviceAddress);
  fprintf(stdout,"%s status = %d newState = %d\n", __FUNCTION__ , status , newState);
  fprintf(stdout,"%s device address: %s\n",__FUNCTION__, deviceAddress.c_str());
  if (newState == GattDevice::STATE_CONNECTED && status == GATT_SUCCESS) {
    fprintf(stdout,"The device %s got connected \n", deviceAddress.c_str());
    gattstestServerCb = this;
    if(it != connectedDevices.end()) {
      //Device already exists do not insert
    } else {
        connectedDevices.push_back(deviceAddress);
    }
    for(ptr = servCBInstanceMap.begin(); ptr != servCBInstanceMap.end() ; ++ptr ) {
      if(ptr->first == gattstestServerCb) {
        connected = true;
        break;
      }
    }
    if(connected) {
      mServer= ptr->second;
      if(dtr != DeviceMap.end()) {
      //Device Already exists do not add
    } else {
      DeviceMap.insert(pair <string,GattServer*> (deviceAddress,mServer));
    }
    mServer->connect(deviceAddress,AUTO_CONNECT);
    }
  } else if(newState == GattDevice::STATE_DISCONNECTED) {
    fprintf(stdout,"The device %s got disconnected \n", deviceAddress.c_str());
    if(dtr != DeviceMap.end()) {
      DeviceMap.erase(dtr);
    }
    for(it = connectedDevices.begin(); it != connectedDevices.end() ; ++it ) {
      if(*it == deviceAddress) {
        fprintf(stdout,"deviceAddress:  %s\n", (*it).c_str());
        connectedDevices.erase(it);
        break;
      }
    }
  }
  if(connectedDevices.size() != 0) {
    fprintf(stdout,"Connected Device list :");
    for(it = connectedDevices.begin(); it != connectedDevices.end() ; ++it ) {
      fprintf(stdout,"deviceAddress: %s\n", (*it).c_str());
    }
  }
}

void gattstestServerCallback::onServiceAdded(int status,GattService *service)
{
  fprintf(stdout,"%s status: %d\n",__FUNCTION__,status);
}

void gattstestServerCallback::onCharacteristicReadRequest(string deviceAddress, int requestId,
                                          int offset, GattCharacteristic *characteristic)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  uint8_t *value = NULL;
  value = characteristic->getValue();
  size_t value_length = characteristic->getValue();
  GattService *mService = characteristic->getService();
  Uuid s_uuid = mService->getUuid();
  Uuid c_uuid = characteristic->getUuid();
  fprintf(stdout,"%s value = %s\n", __FUNCTION__, value);
  fprintf(stdout,"%s service Uuid = %s\n", __FUNCTION__, s_uuid.ToString().c_str());
  fprintf(stdout,"%s characteristic uuid = %s\n", __FUNCTION__, c_uuid.ToString().c_str());
  GattServer *mServer = NULL;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator str;
  gattstestServerCb = this;
  for(str = servCBInstanceMap.begin(); str != servCBInstanceMap.end() ; ++str ) {
    if(str->first == gattstestServerCb) {
      break;
    }
  }
  mServer= str->second;
  bool status = mServer->sendResponse(deviceAddress,requestId,0,offset,value + offset, value_length - offset);
  if(status) {
    fprintf(stdout,"%s response sent \n", __FUNCTION__);
  }
}

void gattstestServerCallback::onCharacteristicWriteRequest(string deviceAddress,int requestId,
                            GattCharacteristic *characteristic,bool preparedWrite,bool responseNeeded,
                            int offset,uint8_t* value, int length)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  string temp((char *)value);
  GattServer *mServer = NULL;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator str;
  bool confirm  = false;
  gattstestServerCb = this;
  for(str = servCBInstanceMap.begin(); str != servCBInstanceMap.end() ; ++str ) {
    if(str->first == gattstestServerCb) {
      break;
    }
  }
  fprintf(stdout," %s -> New value written: %s\n",__FUNCTION__,temp.c_str());
  mServer= str->second;
  if(preparedWrite) {
     executeWriteChar = characteristic;
     receivedData += temp;
  } else {
    characteristic->setValue(value, length);
  }
  if (responseNeeded) {
    mServer->sendResponse(deviceAddress,requestId,0,offset,value,length);
  }
  int d = characteristic->getProperties() & GattCharacteristic::PROPERTY_NOTIFY;
  if((characteristic->getProperties() & GattCharacteristic::PROPERTY_NOTIFY) != 0) {
    confirm = false;
    mServer->notifyCharacteristicChanged(deviceAddress,*characteristic,confirm);
  } else if((characteristic->getProperties() & GattCharacteristic::PROPERTY_INDICATE) != 0) {
    confirm = true;
    mServer->notifyCharacteristicChanged(deviceAddress,*characteristic,confirm);
  }
}

void gattstestServerCallback::onDescriptorReadRequest(string deviceAddress, int requestId,
                                                              int offset, GattDescriptor *descriptor)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  uint8_t *value = NULL;
  Uuid desc_uuid = descriptor->getUuid();
  value = descriptor->getValue();
  size_t value_length = descriptor->getValueLength() - offset;
  fprintf(stdout,"%s Descriptor UUID: %s  value = %s\n", __FUNCTION__, desc_uuid.ToString().c_str(),descriptor->getValue());
  GattServer *mServer = NULL;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator str;
    gattstestServerCb = this;
    for(str = servCBInstanceMap.begin(); str != servCBInstanceMap.end() ; ++str ) {
    if(str->first == gattstestServerCb)
        break;
    }
    mServer= str->second;
    bool status = mServer->sendResponse(deviceAddress,requestId,0,offset,value+offset,value_length-offset);
    if(status) {
        fprintf(stdout,"%s response sent \n", __FUNCTION__);
    }
}

void gattstestServerCallback::onDescriptorWriteRequest(string deviceAddress, int requestId,
                                                    GattDescriptor *descriptor,bool preparedWrite,
                                                    bool responseNeeded, int offset, uint8_t * value,
                                                    int length)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  string temp((char *)value);
  fprintf(stdout," %s -> New value written: %s\n",__FUNCTION__,temp.c_str());

  GattCharacteristic *characteristic = descriptor->getCharacteristic();
  Uuid d_uid = descriptor->getUuid();
  Uuid c_uid = characteristic->getUuid();
  fprintf(stdout,"%s  descriptor_uuid: %s value = %s\n", __FUNCTION__, d_uid.ToString().c_str(),
                                                    temp.c_str());
  GattServer *mServer = NULL;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator str;
  gattstestServerCb = this;
  for(str = servCBInstanceMap.begin(); str != servCBInstanceMap.end() ; ++str ) {
    if(str->first == gattstestServerCb) {
      break;
    }
  }
  mServer= str->second;
  if(preparedWrite) {
     executeWriteDesc = descriptor;
     receivedDescValue += temp;
  } else {
    descriptor->setValue(value, length);
  }
  if (responseNeeded) {
    bool status = mServer->sendResponse(deviceAddress,requestId,0,offset,value,length);
    if (status) {
      fprintf(stdout,"%s response sent \n", __FUNCTION__);
    }
  }
}

void gattstestServerCallback::onExecuteWrite(string deviceAddress, int requestId, bool execute)
{
  fprintf(stdout,"%s deviceAddress: %s, requestID: %d execute %d\n", __FUNCTION__, deviceAddress.c_str(),
                                                              requestId, execute);
  GattServer *mServer = NULL;
  unordered_map <gattstestServerCallback*,GattServer*> ::iterator str;
  gattstestServerCb = this;
  for(str = servCBInstanceMap.begin(); str != servCBInstanceMap.end() ; ++str ) {
    if(str->first == gattstestServerCb) {
      break;
    }
  }
  mServer= str->second;
  if(execute) {
    executeWriteChar->setValue(receivedData);
    receivedData.clear();
  } else {
     receivedData.clear();
  }
  bool status = mServer->sendResponse(deviceAddress,requestId,GATT_SUCCESS,0,NULL,0);
  if (status) {
    fprintf(stdout,"%s response sent \n", __FUNCTION__);
  }
}

void gattstestServerCallback::onNotificationSent(string deviceAddress, int status)
{
  fprintf(stdout,"%s deviceAddress %s status %d\n", __FUNCTION__, deviceAddress.c_str(), status);
}

void gattstestServerCallback::onMtuChanged(string deviceAddress, int mtu)
{
  fprintf(stdout,"%s deviceAddress: %s mtu %d\n",__FUNCTION__,deviceAddress.c_str(), mtu);
}

void gattstestServerCallback::onPhyUpdate(string deviceAddress,int txPhy, int rxPhy, int status)
{
  fprintf(stdout,"%s deviceAddress: %s txphy: %d rxPhy: %d   status: %d\n",__FUNCTION__,
                                                    deviceAddress.c_str(),txPhy,rxPhy,status);
}

void gattstestServerCallback::onPhyRead(string deviceAddress,int txPhy,int rxPhy,int status)
{
  fprintf(stdout,"%s deviceAddress: %s, txPhy: %d rxPhy: %d status: %d\n", __FUNCTION__,
                                                    deviceAddress.c_str(), txPhy, rxPhy, status);
  fprintf(stdout,"%s deviceAddress: %s, txPhy: %d rxPhy: %d status: %d\n",__FUNCTION__,
                                                    deviceAddress.c_str(), txPhy, rxPhy, status);
}

void gattstestServerCallback::onConnectionUpdated(string deviceAddress,int interval,int latency,int timeout,int status)
{
  fprintf(stdout,"%s deviceAddress: %s,interval: %d,latency %d,timeout %d, status:%d\n", __FUNCTION__,
                                        deviceAddress.c_str(), interval, latency, timeout, status);
}


class gattstestAdvertiserCallback  :public AdvertisingSetCallback
{
  public:
  void onAdvertisingSetStarted (AdvertisingSet *advertisingSet, int txPower, int status) {
    fprintf(stdout,"%s status: %d  txpower: %d\n", __FUNCTION__, status, txPower);
    switch (status) {
      case AdvertisingSetCallback::ADVERTISE_SUCCESS:
        num_of_advertiser++;
        fprintf(stdout,"Advertising Set Success\n");
        fprintf(stdout,"onAdvertisingSetStarted - Success \n");
        fprintf(stdout,"AdvertiserID: %d\n", advertisingSet->getAdvertiserId());
        advSetMap.insert(pair <int,AdvertisingSet*> (num_of_advertiser,advertisingSet));
      break;
      case AdvertisingSetCallback::ADVERTISE_FAILED_ALREADY_STARTED:
        fprintf(stdout,"Advertising Already started\n");
      break;
      case AdvertisingSetCallback::ADVERTISE_FAILED_DATA_TOO_LARGE:
        fprintf(stdout,"Advertising Failed: Data too Large\n");
      break;
      case AdvertisingSetCallback::ADVERTISE_FAILED_FEATURE_UNSUPPORTED:
        fprintf(stdout,"Advertising Failed: Feature Unsupported\n");
      break;
      case AdvertisingSetCallback::ADVERTISE_FAILED_INTERNAL_ERROR:
        fprintf(stdout,"Advertising Failed: Internal Error\n");
      break;
      case AdvertisingSetCallback::ADVERTISE_FAILED_TOO_MANY_ADVERTISERS:
        fprintf(stdout,"Advertising Failed: Too Many Advertisers\n");
      break;
      default:
        fprintf(stdout,"Failure case unknown\n");
     }
  }

  void  onAdvertisingDataSet(AdvertisingSet *advertisingset,int status)
  {
    fprintf(stdout,"%s status: %d\n", __FUNCTION__, status);
    if (status == AdvertisingSetCallback::ADVERTISE_SUCCESS) {
      fprintf(stdout,"Advertising Data is set successfully\n");
      fprintf(stdout,"Advertiser ID  %d\n", advertisingset->getAdvertiserId());
    }
  }

  void onAdvertisingSetStopped (AdvertisingSet *advertisingSet)
  {
    fprintf(stdout,"%s Advertiser ID  %d\n",__FUNCTION__,advertisingSet->getAdvertiserId());
  }

  void onAdvertisingEnabled (AdvertisingSet *advertisingSet, bool enable, int status)
  {
    fprintf(stdout,"%s  enable: %d status %d\n",__FUNCTION__,enable,status);
    if (status == AdvertisingSetCallback::ADVERTISE_SUCCESS) {
      fprintf(stdout,"AdvertiserID: %d Advertising Enabled Succesfully\n", advertisingSet->getAdvertiserId());
    }
  }

  void onScanResponseDataSet (AdvertisingSet *advertisingSet, int status)
  {
    fprintf(stdout,"onScanResponseDataSet status: %d\n", status);
  }

  void onAdvertisingParametersUpdated (AdvertisingSet *advertisingSet, int txPower, int status)
  {
    fprintf(stdout,"onAdvertisingParametersUpdated txpower: %d status %d\n", txPower, status);
  }

  void onPeriodicAdvertisingParametersUpdated (AdvertisingSet *advertisingSet, int status)
  {
    fprintf(stdout,"onPeriodicParametersUpdated  status: %d\n", status);
  }

  void onPeriodicAdvertisingDataSet (AdvertisingSet *advertisingSet, int status)
  {
    fprintf(stdout,"onPeriodicAAdvertisingDataSet status: %d\n", status);
  }

  void onPeriodicAdvertisingEnabled (AdvertisingSet *advertisingSet, bool enable, int status)
  {
    fprintf(stdout,"onPeriodicAdvertisingEnabled enable : %d status: %d Advertiser id: %d\n", enable,
                                                      status, advertisingSet->getAdvertiserId());
  }

  void onOwnAddressRead (AdvertisingSet *advertisingSet, int addressType, string address)
  {
    fprintf(stdout,"onOwnAddressRead  addressType: %d  address: %s advertiser id: %d\n", addressType,
                                                address.c_str(), advertisingSet->getAdvertiserId());
  }

  void onStartSuccess(AdvertiseSettings *settingsInEffect)
  {
    fprintf(stdout,"%s\n",__FUNCTION__);
  }

  void onStartFailure(int errorCode)
  {
    ALOGE(LOGTAG "onStartFailure() %d\n", errorCode);
    fprintf(stdout," onStartFailure errorCode = %d\n", errorCode);
  }

};

map <int, GattServer*> servInstanceMap;
map <int,gattstestAdvertiserCallback*> advCBInstanceMap;
gattstestAdvertiserCallback *gattstestAdvCb = NULL;
GattLeAdvertiser *madvertiser = NULL;

GattsTest::GattsTest()
{
  fprintf(stdout,"gattstest instantiated \n");
}

GattsTest::~GattsTest()
{
  fprintf(stdout,"(%s) GATTSTEST DeInitialized\n",__FUNCTION__);
}


void GattsTest::ReadServerConfigurationFile()
{
  fprintf(stdout,"%d\n",__FUNCTION__);
  string ch;
  int line_num = 0;
  int desired_line = 7;
  bool status = false;
  std::ifstream infile(SERVER_CFG_FILE_PATH,std::ios::binary);
  //check whether file exists
  if(!infile) {
    fprintf(stdout,"Error opening file\n");
    return ;
  }

  while(!infile.eof()) {
    getline(infile,ch,'\r');
    if(std::regex_search(ch,std::regex("\\bServer[1-9]|Server[1-9][0-9]\\b"))) {
      while(line_num < desired_line) {
        getline(infile,ch,'\r');
        status = ParseServiceDetails(ch,line_num);
        if(!status) {
          fprintf(stdout,"Service Records are not consistent \n");
          break;
        }
        line_num++;
      }
    } else {
        fprintf(stdout,"Server Config File is incorrect \n");
        break;
      }
      line_num = 0;
    }
  fprintf(stdout,"File reading Done \n");
  //closing the file after reading
  infile.close();
}

bool GattsTest::ParseServiceDetails(string temp,int line_num)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  int pos=0;
  int manuID;
  string manuData;
  bool status = false;
  static int rec = 0;
  if (!regex_search(temp,regex("\\bService[1-5]|ManufacturerId|ManufacturerData\\b"))) {
      fprintf(stdout,"record incorrect \n");
      return false;
  } else {
    pos = temp.find(":");
    temp = temp.substr(pos + 1);
    stringstream ss(temp);
    ss >> temp;
    if(line_num == MANUFACTURER_ID_LINE) {
        istringstream(temp) >> manuID;
        manufacturerId_list.push_back(manuID);
        return true;
    } else if(line_num == MANUFACTURER_DATA_LINE) {
      manufacturerData_list.push_back(temp);
      return true;
    } else if(line_num >= SERVICE_LINE_MIN && line_num <= SERVICE_LINE_MAX) {
        status = split(temp,',',service_field);
        if(status) {
          ParseServiceElement(line_num);
          service_field.clear();
          return true;
        } else {
          fprintf(stdout,"Invalid service entry\n");
          return false;
        }
    }
  }
}

void GattsTest::ParseServiceElement(int instance)
{
  fprintf(stdout,"%s instance: %d\n",__FUNCTION__,instance);
  string parameter;
  int property;
  int permissions;
  Service *service_temp = new Service;
  service_temp->s_uuid = "";
  service_temp->c_uuid = "";
  service_temp->d_uuid = "";
  service_temp->c_property = INVALID_VALUE;
  service_temp->c_permissions = INVALID_VALUE;
  service_temp->d_permissions = INVALID_VALUE;
  int len = service_field.size();
  if(instance >= SERVICE_LINE_MIN && instance <= SERVICE_LINE_MAX) {
      if(service_field[0].empty()) {
        fprintf(stdout,"Service details are empty\n");
        service_temp = NULL;
      } else {
        if(len >= 1) {
          service_temp->s_uuid = service_field[0];
        }
        if(len >= 2) {
          service_temp->c_uuid = service_field[1];
        }
        if(len >= 3) {
          istringstream(service_field[2]) >> property;
          service_temp->c_property = property;
        }
        if(len >= 4) {
          istringstream(service_field[3]) >> permissions;
          service_temp->c_permissions = permissions;
        }
        if(len >= 5) {
          service_temp->d_uuid = service_field[4];
        }
        if(len >= 6) {
          istringstream(service_field[5]) >> permissions;
          service_temp->d_permissions = permissions;
        }
      }
      service_list[instance].push_back(service_temp);
  }
}

void GattsTest::AddServer()
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  if(num_of_server <= MAX_SERVER_INSTANCE) {
    num_of_server++;
    fprintf(stdout,"Adding Server Instance : %d\n", num_of_server);
    mgattServer = new GattServer(TRANSPORT);
    servInstanceMap.insert(pair <int,GattServer*> (num_of_server,mgattServer));
    fprintf(stdout,"Adding Server CallBack : %d \n", num_of_server);
    gattstestServerCb = new gattstestServerCallback();
    servCBInstanceMap.insert(pair <gattstestServerCallback*,GattServer*> (gattstestServerCb,mgattServer));
    mgattServer->registerCallback(*gattstestServerCb);
    fprintf(stdout,"Adding Advertiser Callback: %d \n", num_of_server);
    gattstestAdvCb    = new gattstestAdvertiserCallback();
    advCBInstanceMap.insert(pair <int,gattstestAdvertiserCallback*> (num_of_server,gattstestAdvCb));
  } else {
    fprintf(stdout,"The number of servers that can be created has reached it's limit of 20 \n");
    fprintf(stdout,"Server Not created\n");
  }
}

bool GattsTest::AddService(string server_instance,string service_instance)
{
  fprintf(stdout,"%s  \n",__FUNCTION__);
  int server_inst;
  int service_inst;
  istringstream(server_instance) >> server_inst;
  istringstream(service_instance) >> service_inst;
  fprintf(stdout,"server_inst: %d service_inst %d\n", server_inst, service_inst);
  Uuid  temp_UUID;
  string uid;
  GattServer *mServer = NULL;
  GattService *mService = NULL;
  Service *service_temp;
  int property = 0;
  int permissions = 0;
  string char_val = "QTI_LE";
  string desc_val = "QTI_DESC";
  if(server_inst > num_of_server) {
    fprintf(stdout,"Please create the server instance first \n");
    return false;
  } else if((server_inst <= 0) || (server_inst > MAX_SERVER_INSTANCE) || (service_inst <=0) || (service_inst > MAX_SERVICE_INSTANCE) ) {
    fprintf(stdout,"Incorrect instance values  \n" );
    return false;
  } else if (service_inst < 6) {
    fprintf(stdout,"server_inst = %d \n",server_inst);
    mServer = servInstanceMap[server_inst];
    service_temp = service_list[service_inst - 1][server_inst -1];
    if(service_temp == NULL) {
      fprintf(stdout,"Service details are not available, service cannot be added \n");
      ALOGE(LOGTAG"Service details are not available, service cannot be added ");
      return false;
    } else {
      uid = service_temp->s_uuid;
      temp_UUID = Uuid::FromString(uid);
      mService  = new GattService(temp_UUID,GattService::SERVICE_TYPE_PRIMARY);
      uid = service_temp->c_uuid;
      if(!uid.empty()) {
        temp_UUID = Uuid::FromString(uid);
        property = service_temp->c_property;
        if(property == INVALID_VALUE) {
          property = 2;
        }
        permissions = service_temp->c_permissions;
        if(permissions == INVALID_VALUE) {
          permissions = 1;
        }
        AddCharacteristics(temp_UUID,property,permissions,char_val);
        uid = service_temp->d_uuid;
        if(!uid.empty()) {
          temp_UUID = Uuid::FromString(uid);
          permissions = service_temp->d_permissions;
          AddDescriptors(temp_UUID,permissions,desc_val);
          mgattCharacteristic->addDescriptor(mgattDescriptor);
        } else {
          fprintf(stdout,"No descriptor added\n");
        }
        mService->addCharacteristic(mgattCharacteristic);
        } else {
        fprintf(stdout,"No Characteristics added\n");
      }
    }
  }
  mServer->addService(*mService);
  return true;
}

bool GattsTest::ReadAdvertiserConfigFile()
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  string ch;
  int pos=0;
  int line_num = 0;
  int desired_line = 12;
  std::ifstream infile(ADV_CFG_FILE_PATH,std::ios::binary);
  if(!infile) {
    fprintf(stdout,"File doesn't exist \n");
    return false;
  }
  while(!infile.eof()) {
    getline(infile,ch,'\n');
    if(regex_search(ch,regex("\\bAdvertisingSet[1-9]|AdvertisingSet[1-9][0-9]\\b"))) {
      set_temp = new AdvertiseSet;
      set_temp->tx_power = INVALID_VALUE;
      set_temp->legacyflag = INVALID_VALUE;
      set_temp->periodicflag = INVALID_VALUE;
      set_temp->connectableflag = INVALID_VALUE;
      set_temp->scannableflag = INVALID_VALUE;
      set_temp->anonymousflag = INVALID_VALUE;
      set_temp->includeTxPowerflag= INVALID_VALUE;
      set_temp->primary_phy= INVALID_VALUE;
      set_temp->secondary_phy= INVALID_VALUE;
      set_temp->interval = INVALID_VALUE;
      set_temp->timeout_legacy= INVALID_VALUE;
      set_temp->advertise_mode= INVALID_VALUE;
      while(line_num < desired_line) {
        fprintf(stdout,"line_num < desired_line  %d < %d\n", line_num, desired_line);
        getline(infile,ch,'\n');
        ParseAdvertiserDetails(ch);
        line_num++;
      }
      AdvSet_list.push_back(set_temp);
    }else {
    fprintf(stdout,"There are no Advertising Set records in the file \n");
    break;
    }
    line_num = 0;
  }
  madvertiser = GattLeAdvertiser::getGattLeAdvertiser();
  fprintf(stdout,"File reading done mAdvertiser %p \n", madvertiser);
  infile.close();
  return true;
}

void GattsTest::ParseAdvertiserDetails(string temp)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  int pos = 0;
  int parameter = 0;
  string data;
  pos = temp.find(":");
  data = temp.substr(pos + 1);
  istringstream(data) >> parameter;
  if(regex_search(temp,regex("\\bTxPower\\b"))){
    set_temp->tx_power = parameter;
  } else if(regex_search(temp,regex("\\bLegacyFlag\\b"))) {
    set_temp->legacyflag = parameter;
  } else if(regex_search(temp,regex("\\bPeriodicFlag\\b"))) {
    set_temp->periodicflag = parameter;
  } else if(regex_search(temp,regex("\\bConnectableFlag\\b"))) {
    set_temp->connectableflag = parameter;
  } else if(regex_search(temp,regex("\\bScannableFlag\\b"))) {
    set_temp->scannableflag = parameter;
  } else if(regex_search(temp,regex("\\bAnonymousFlag\\b"))) {
    set_temp->anonymousflag = parameter;
  } else if(regex_search(temp,regex("\\bIncludePower\\b"))) {
    set_temp->includeTxPowerflag= parameter;
  } else if(regex_search(temp,regex("\\bPrimaryPhy\\b"))) {
    set_temp->primary_phy= parameter;
  } else if(regex_search(temp,regex("\\bSecondaryPhy\\b"))) {
    set_temp->secondary_phy= parameter;
  } else if(regex_search(temp,regex("\\bInterval\\b"))) {
    set_temp->interval = parameter;
  } else if(regex_search(temp,regex("\\bTimeOutLegacy\\b"))) {
    set_temp->timeout_legacy= parameter;
  } else if(regex_search(temp,regex("\\bAdvertiseMode\\b"))) {
    set_temp->advertise_mode= parameter;
  }
}

bool GattsTest::StartAdvertisement(string        instanceID)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  int instance = 0;
  istringstream(instanceID) >> instance;
  if (instance <= 0 || instance > MAX_SERVER_INSTANCE) {
    fprintf(stdout,"Invalid input argument \n");
    return false;
  }
  int legacyflag = 0;
  AdvertiseSet *temp = NULL;
  bool status = false;
  status = BuildAdvertisingParameters(instance);
  if(!status) {
    fprintf(stdout,"Advertising Parameters not set \n");
    return false;
  }
  status = BuildAdvertisingData(instance);
  if(!status) {
    fprintf(stdout,"Advertising Data not set \n");
    return false;
  }
  status = SetPeriodicAdvertisingParameters(instance);
  if(!status) {
    fprintf(stdout,"Periodic Advertising parameters not set \n");
    return false;
  }
  status = SetPeriodicAdvertisingData(instance);
  if(!status) {
    fprintf(stdout,"Periodic Advertising Data not set \n");
    return false;
  }
  status = SetScanResponseData(instance);
  if(!status) {
    fprintf(stdout,"Scan Response Data not set \n");
    return false;
  }
  //fetching advertiser Callback instance for the server/advertiser instance key
  gattstestAdvCb = advCBInstanceMap[instance];
  //Finding corresponding Legacy flag details for the corresponding advertiser
  temp = AdvSet_list[instance -1];
  legacyflag = temp->legacyflag;
  try {
    if(legacyflag) {
      fprintf(stdout,"Legacy StartAdvertisement\n");
      madvertiser->startAdvertising(mAdvertiseSettings,mAdvertiseData,mScanResponseData,gattstestAdvCb);
    } else {
      madvertiser->startAdvertisingSet(mAdvertisingParameters,
                         mAdvertiseData,mScanResponseData,mPeriodicParams,mPeriodicData,gattstestAdvCb);
    }
  } catch(const std::exception &ex) {
    fprintf(stdout,"%s start Advertising exception  %s\n", __FUNCTION__, ex.what());
    return false;
  }
  return true;
}

bool GattsTest::BuildAdvertisingParameters(int instance)
{
  int connectableflag;
  int scannableflag;
  int legacyflag;
  int anonymousflag;
  int periodicflag;
  int includeTxPowerflag;
  int primary_phy;
  int secondary_phy;
  int interval;
  int tx_power;
  int power_mode;
  int timeout_legacy;
  int advertise_mode;
  fprintf(stdout,"%s\n",__FUNCTION__);
  AdvertiseSet *temp;
  temp = AdvSet_list[instance - 1];
  if(temp == NULL) {
    ALOGE(LOGTAG"%s Advertising Configuration not found", __FUNCTION__);
    return false;
  }
  legacyflag = temp->legacyflag;
  //input validation
  if (legacyflag < VALID_VALUE && connectableflag < VALID_VALUE && scannableflag < VALID_VALUE &&
      periodicflag < VALID_VALUE && anonymousflag < VALID_VALUE && includeTxPowerflag < VALID_VALUE &&
      primary_phy < VALID_VALUE && secondary_phy < VALID_VALUE && interval < VALID_VALUE &&
      power_mode < VALID_VALUE && timeout_legacy < VALID_VALUE && advertise_mode < VALID_VALUE) {
    fprintf(stdout,"Incorrect flag value \n");
    return false;
  }
  if (legacyflag > 1 && connectableflag > 1 && scannableflag > 1 && periodicflag > 1 && anonymousflag > 1 && includeTxPowerflag > 1) {
    fprintf(stdout,"Flags values set incorrectly, Please check 'legacyflag' 'connectableflag' 'scannableflag' periodicflag' 'anonymousflag' includetxpowerflag' \n");
    return false;
  }

  try {
    if(legacyflag) {
      fprintf(stdout," Legacy Advertising will be used \n");
      mAdvertiseSettings = AdvertiseSettings::Builder()
                           .setAdvertiseMode(temp->advertise_mode)
                           .setTxPowerLevel(temp->tx_power)
                           .setConnectable(temp->connectableflag)
                           .setTimeout(temp->timeout_legacy)
                           .build();

      fprintf(stdout,"Advertising Settings connectable = %d \
            TxPowerLevel = %d AdvertiseMode =%d TimeOut = %d\n",
            mAdvertiseSettings->isConnectable(),
            mAdvertiseSettings->getTxPowerLevel(), mAdvertiseSettings->getMode(),
            mAdvertiseSettings->getTimeout());
    } else {
      mAdvertisingParameters = AdvertisingSetParameters::Builder()
                              .setConnectable(temp->connectableflag)
                              .setScannable(temp->scannableflag)
                              .setLegacyMode(temp->legacyflag)
                              .setAnonymous(temp->anonymousflag)
                              .setIncludeTxPower(temp->includeTxPowerflag)
                              .setPrimaryPhy(temp->primary_phy)
                              .setSecondaryPhy(temp->secondary_phy)
                              .setInterval(temp->interval)
                              .setTxPowerLevel(temp->tx_power)
                              .build();

    fprintf(stdout,"Advertising parameters connectable = %d \
            scannable= %d LegacyMode =%d anonymous = %d \
            includeTxPower = %d primaryphy = %d secondaryphy = %d \
            interval = %d Txpower = %d \n", mAdvertisingParameters->isConnectable(),
            mAdvertisingParameters->isScannable(), mAdvertisingParameters->isLegacy(),
            mAdvertisingParameters->isAnonymous(), mAdvertisingParameters->includeTxPower(),
            mAdvertisingParameters->getPrimaryPhy(), mAdvertisingParameters->getSecondaryPhy(),
            mAdvertisingParameters->getInterval(), mAdvertisingParameters->getTxPowerLevel());
    }
  }
  catch(const std::exception &ex) {
    fprintf(stdout,"%s exception caught: %s\n", __FUNCTION__, ex.what());
    fprintf(stdout,"exception: %s\n", ex.what());
    return false;
  }
  return true;
}

bool GattsTest::BuildAdvertisingData(int instance) {
  int includeTxPowerflag;
  string mManufacturerID;
  string mManufacturerData;
  string service1_uuid;
  Service *temp;
  AdvertiseSet *set = NULL;
  string service_data= "QTI_SERVICE_DATA";
  string service_data_uuid = "0000AAAA-0000-1000-8000-00805F9B34FB";
  Uuid mUuid;
  set = AdvSet_list[instance -1];
  if(set == NULL) {
    ALOGE(LOGTAG"%s Advertising Configuration not found", __FUNCTION__);
    return false;
  }
  fprintf(stdout,"%s\n",__FUNCTION__);
  AdvertiseData::Builder builder = AdvertiseData::Builder().setIncludeDeviceName(true)
                                  .setIncludeTxPowerLevel(set->includeTxPowerflag);
  mManufacturerID = manufacturerId_list[instance-1];
  mManufacturerData = manufacturerData_list[instance-1];
  int legacyflag =  set->legacyflag;
  //If legacy flag is not set then add manufacturer and Service data
  if(!legacyflag) {
  //If manufacturer Data and ID are not empty then add to Advertising Data
    if(mManufacturerID != "" && mManufacturerData != "" ) {
      int id=0;
      istringstream(mManufacturerID) >> id;
      fprintf(stdout,"ManufacturerID: %d\n", id);
      fprintf(stdout,"Manufacturer Data: %s\n", mManufacturerData.c_str());
      std::vector<uint8_t> vec(mManufacturerData.begin(), mManufacturerData.end());
      builder.addManufacturerData(id,vec);
    }
    temp= service_list[SERVICE_1][instance -1];
    if(!temp->s_uuid.empty()) {
      mUuid = bt::Uuid::FromString(temp->s_uuid);
      builder.addServiceUuid(mUuid);
      mUuid = bt::Uuid::FromString(service_data_uuid);
      std::vector<uint8_t> vec(service_data.begin(), service_data.end());
      builder.addServiceData(mUuid,vec);
    }
  }
  mAdvertiseData = builder.build();
  fprintf(stdout,"AdvertiseData IncludeDevicename: %d IncludeTxPowerLevel: %d \n",
              mAdvertiseData->getIncludeDeviceName(), mAdvertiseData->getIncludeTxPowerLevel());
  if(!legacyflag) {
    fprintf(stdout,"manufacturer_data size:  %d\n", mAdvertiseData->getManufacturerSpecificData().size());
    fprintf(stdout,"serviceData size:  %d\n", mAdvertiseData->getServiceData().size());
  }
  return true;
}

bool GattsTest::SetPeriodicAdvertisingData(int instance)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  int periodic_flag;
  AdvertiseSet *temp = NULL;
  temp = AdvSet_list[instance -1];
  if(temp == NULL) {
    ALOGE(LOGTAG"%s Advertising Configuration not found", __FUNCTION__);
    return false;
  }
  periodic_flag = temp->periodicflag;
  if(periodic_flag) {
    mPeriodicData = mAdvertiseData;
  } else {
    mPeriodicData = NULL;
  }
  return true;
}


bool GattsTest::SetPeriodicAdvertisingParameters(int instance)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  int periodic_flag;
  AdvertiseSet *temp = NULL;
  temp = AdvSet_list[instance -1];
  if(temp == NULL) {
    ALOGE(LOGTAG"%s Advertising Configuration not found", __FUNCTION__);
    return false;
  }
  periodic_flag = temp->periodicflag;
  int include_txpower;
  int periodic_interval = PERIODIC_INTERVAL;
  include_txpower = temp->includeTxPowerflag;

  if(periodic_flag) {
    mPeriodicParams = PeriodicAdvertiseParameters::Builder()
                      .setIncludeTxPower(include_txpower)
                      .setInterval(periodic_interval)
                      .build();

    fprintf(stdout,"SetPeriodicAdvertisingParameters:: IncludeTxPower: %d interval %d\n",
              mPeriodicParams->getIncludeTxPower() ,mPeriodicParams->getInterval());
  } else {
    mPeriodicParams = NULL;
  }
  return true;
}

bool GattsTest::SetScanResponseData(int instance)
{
  fprintf(stdout,"%s \n",__FUNCTION__);
  int scannable_flag;
  AdvertiseSet *temp = NULL;
  temp = AdvSet_list[instance -1];
  if(temp == NULL) {
    ALOGE(LOGTAG"%s Advertising Configuration not found", __FUNCTION__);
    return false;
  }
  scannable_flag = temp->scannableflag;
  if(scannable_flag) {
    mScanResponseData = mAdvertiseData;
  } else {
    mScanResponseData = NULL;
  }
  return true;
}

bool GattsTest::UnregisterServer(string instance)
{
  GattServer *mServer;
  fprintf(stdout,"%s \n",__FUNCTION__);
  int instanceId;
  istringstream(instance) >> instanceId;
  if(instanceId <=0 || instanceId > num_of_server || instanceId > MAX_SERVER_INSTANCE) {
    fprintf(stdout,"Server instance value invalid, Please type a valid instance\n");
    return false;
  }
  if(num_of_server <= MAX_SERVER_INSTANCE) {
    mServer = servInstanceMap[instanceId];
    mServer->close();
    servInstanceMap.erase(instanceId);
    num_of_server--;
    return true;
  } else {
    fprintf(stdout,"There are no more servers to unregister \n");
    return false;
  }
}

bool GattsTest::StopAdvertisement(string instance)
{
  fprintf(stdout,"StopAdvertisement \n");
  int instanceId;
  istringstream(instance) >> instanceId;
  if(instanceId <=0 || instanceId > num_of_server) {
    fprintf(stdout,"Server instance value invalid, Please type a valid instance\n");
    return false;
  } else {
    AdvertisingSetCallback *mAdvSetCB;
    mAdvSetCB = advCBInstanceMap[instanceId];
    madvertiser->stopAdvertising(mAdvSetCB);
  }
}


bool GattsTest::AddCharacteristics(Uuid uid,int property, int permissions, string val)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  mgattCharacteristic = new GattCharacteristic(uid,property,permissions);
  uint8_t char_val[val.length()+1];
  std::copy(val.begin(),val.end(),char_val);
  char_val[val.length()] = '\0';
  mgattCharacteristic->setValue(char_val, val.length()+1);
  fprintf(stdout,"CharacteristicUUID: %s  \n", uid.ToString().c_str());
  fprintf(stdout,"Characteristic Property: %d \n", mgattCharacteristic->getProperties());
  fprintf(stdout,"characteristic Permissions: %d \n", mgattCharacteristic->getPermissions());
  fprintf(stdout,"characteristic value: %s\n", mgattCharacteristic->getValue());
  return true;
}

bool GattsTest::AddDescriptors(Uuid uid,int permissions,string value)
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  fprintf(stdout,"string value =  %s\n", value.c_str());
  mgattDescriptor = new GattDescriptor(uid,permissions);
  uint8_t dsc_val[value.length()+1];
  std::copy(value.begin(),value.end(),dsc_val);
  dsc_val[value.length()] = '\0';
  mgattDescriptor->setValue(dsc_val, value.length()+1);
  fprintf(stdout,"Descriptor UUID: %s  \n", uid.ToString().c_str());
  fprintf(stdout,"Descriptor Permissions: %d\n ", mgattDescriptor->getPermissions());
  return true;
}

bool GattsTest::ReadPhy(string instance,string deviceAddress)
{
  fprintf(stdout,"%s Address: %s\n", __FUNCTION__, deviceAddress.c_str());
  vector <string> ::iterator str;
  bool connected= false;
  int instanceId;
  istringstream(instance) >> instanceId;
  GattServer *mServer;

  if(instanceId <=VALID_VALUE || instanceId > num_of_server || instanceId > MAX_SERVER_INSTANCE) {
    fprintf(stdout,"Server instance value invalid, Please type a valid instance\n");
    return false;
  } else {
    mServer = servInstanceMap[instanceId];
    for(str = connectedDevices.begin(); str != connectedDevices.end(); str++) {
      if(deviceAddress == *str) {
        fprintf(stdout,"Present in connected device list \n");
        connected = true;
        break;
      }
    }
  }
  if(connected) {
    mServer->readPhy(deviceAddress);
    return true;
  } else {
    fprintf(stdout,"Device is not present in connected list\n");
    return false;
  }
}

bool GattsTest::SetPreferredPhy(string deviceAddress,string instance,string txPhy,string rxPhy,int phyOptions)
{
  fprintf(stdout,"%s Address: %s  txPhy: %s rxPhy: %s phyOptions: %d\n", __FUNCTION__, deviceAddress.c_str(), txPhy.c_str(), rxPhy.c_str(), phyOptions);
  int instanceId = 0;
  int tx_phy = 0;
  int rx_phy = 0;
  istringstream(instance) >> instanceId;
  istringstream(txPhy) >> tx_phy;
  istringstream(rxPhy) >> rx_phy;
  GattServer *mServer;
  vector <string> ::iterator str;
  bool connected= false;
  if (instanceId <= VALID_VALUE || instanceId > num_of_server || instanceId > MAX_SERVER_INSTANCE) {
    fprintf(stdout,"Server instance value invalid, Please type a valid instance\n");
    return false;
  } else {
    if((tx_phy != PHY_LE_1M) && (tx_phy != PHY_LE_2M) && (tx_phy != PHY_LE_CODED)) {
      fprintf(stdout,"Enter a valid tx phy option \n");
      return false;
  }
  if((rx_phy != PHY_LE_1M) && (rx_phy != PHY_LE_2M) && (rx_phy != PHY_LE_CODED)) {
    fprintf(stdout,"Enter a valid rx phy option \n");
    return false;
  }
  mServer = servInstanceMap[instanceId];
  for(str = connectedDevices.begin(); str != connectedDevices.end(); str++) {
    if(deviceAddress == *str) {
      fprintf(stdout,"Present in connected device list \n");
      connected = true;
      break;
    }
  }
  if(connected) {
    mServer->setPreferredPhy(deviceAddress,tx_phy,rx_phy,phyOptions);
    return true;
  } else {
    return false;
  }
  }
}

bool GattsTest::EnablePeriodicAdvertising(bool enable)
{
  fprintf(stdout,"%s \n", __FUNCTION__);
  mAdvertisingSet->setPeriodicAdvertisingEnabled(enable);
}

void GattsTest::CancelConnection(string remoteAddress)
{
  fprintf(stdout,"%s \n", __FUNCTION__);
  GattServer *mServer = NULL;
  bool connected = false;
  map <string,GattServer*> ::iterator dtr = DeviceMap.find(remoteAddress);
  for(dtr = DeviceMap.begin(); dtr != DeviceMap.end() ; ++dtr) {
    if(remoteAddress == dtr->first) {
      mServer = dtr->second;
      connected = true;
      break;
    }
  }
  if(connected) {
    mServer->cancelConnection(remoteAddress);
  } else {
    fprintf(stdout,"Device %s is not connected\n", remoteAddress.c_str());
  }
}

bool GattsTest::DisableGATTSTEST()
{
  fprintf(stdout,"%s\n",__FUNCTION__);
  GattServer *mServer = NULL;
  gattstestServerCallback *mServercallback = NULL;
  gattstestAdvertiserCallback *mAdvertisercallback = NULL;
  map <int, GattServer*> ::iterator itr;
  map <int,gattstestAdvertiserCallback*> ::iterator at;
  servInstanceMap.clear();
  unordered_map  <gattstestServerCallback*,GattServer*> ::iterator it;
  for(it = servCBInstanceMap.begin(); it != servCBInstanceMap.end(); ++it) {
    mServercallback = it->first;
    delete(mServercallback);
    mServer = it->second;
    mServer->close();
    delete(mServer);
  }
  servCBInstanceMap.clear();
  for(at = advCBInstanceMap.begin(); at != advCBInstanceMap.end(); ++at) {
    mAdvertisercallback = at->second;
    delete(mAdvertisercallback);
  }
  advCBInstanceMap.clear();
  delete(madvertiser);
  num_of_server = 0;
  return true;
}




