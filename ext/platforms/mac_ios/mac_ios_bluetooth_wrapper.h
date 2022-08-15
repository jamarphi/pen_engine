/*************************************************************************************************
 Copyright 2021 Jamar Phillip

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*************************************************************************************************/
#pragma once

#include "../../../src/state/config.h"
#include "mac_ios_cpp_objective_c_mapping.h"

namespace pen {
    namespace ios {
        namespace conn {
            namespace bt{
                static void Scan (std::vector<std::string> devices = {}){
                    /*Start scanning for devices*/
                    for(int i = 0; i < devices.size(); i++){
                        MapMacIOSBluetoothAddDevice(devices[i].c_str());
                    }
                    MapMacIOSBluetoothConnect();
                }
            
                static void Stop () {
                    /*Stops scanning for peripheral devices*/
                    MapMacIOSBluetoothStop();
                }
            
                static std::vector<std::string> QueryPotentialConnections(){
                    /*Return the potential connections*/
                    std::vector<std::string> devices;
                    unsigned int deviceCount = MapMacIOSBluetoothGetCountOfPeripherals();
                    for(int i = 0; i < deviceCount; i++){
                        devices.push_back(std::string(MapMacIOSBluetoothGetPeripheral()));
                    }
                    return devices;
                }
            
                static void Connect (std::string device, std::string descriptor) {
                    /*Connect to a peripheral device*/
                    MapMacIOSBluetoothConnect(device.c_str(), descriptor.c_str());
                }
            
                static void Read(const char* device){
                    /*Reads the value from the current characteristic of a given device*/
                    MapMacIOSBluetoothRead(device);
                }
            
                static void Write(char* data, long length){
                    /*Writes a value for the current characteristic to all the connected devices*/
                    MapMacIOSBluetoothWrite(data, length);
                }
            
                static void Disconnect(const char* device){
                    /*Disconnect from a given device*/
                    MapMacIOSBluetoothDisconnect(device);
                }
            }
        }
    }
}
