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
                enum TYPE {
                    CENTRAL = 0,
                    PERIPHERAL = 1,
                    READ = 2,
                    WRITE = 3
                };
            
                /*----Central side functions----*/
                static void Scan (std::vector<std::string> devices = {}){
                    /*Start scanning for devices*/
                    for(int i = 0; i < devices.size(); i++){
                        MapMacPenMacIOSCentralBluetoothAddDevice(devices[i].c_str());
                    }
                    MapMacPenMacIOSCentralBluetoothConnect();
                }
            
                static void Stop () {
                    /*Stops scanning for peripheral devices*/
                    MapMacPenMacIOSCentralBluetoothStop();
                }
            
                static std::vector<std::string> QueryPotentialConnections(){
                    /*Return the potential connections*/
                    std::vector<std::string> devices;
                    unsigned int deviceCount = MapMacPenMacIOSCentralBluetoothGetCountOfPeripherals();
                    for(int i = 0; i < deviceCount; i++){
                        devices.push_back(std::string(MapMacPenMacIOSCentralBluetoothGetPeripheral()));
                    }
                    return devices;
                }
            
                static void Connect (const std::string& device, const std::string& descriptor) {
                    /*Connect to a peripheral device*/
                    MapMacPenMacIOSCentralBluetoothConnect(device.c_str(), descriptor.c_str());
                }
            
                static void Read(const std::string& device){
                    /*Reads the value from the current characteristic of a given device*/
                    MapMacPenMacIOSCentralBluetoothRead(device.c_str());
                }
            
                static void Write(char* data, long length){
                    /*Writes a value for the current characteristic to all the connected devices*/
                    MapMacPenMacIOSCentralBluetoothWrite(data, length);
                }
            
                static void Disconnect(const std::string& device){
                    /*Disconnect from a given device*/
                    MapMacPenMacIOSCentralBluetoothDisconnect(device.c_str());
                }
            
                namespace rec {
                    /*----Peripheral side functions----*/
                    static void AddService(const std::string& service, const std::string& characteristic, char* value, long length, unsigned int type){
                        /*Adds a service for the peripheral side*/
                        MapMacPenMacIOSPeripheralBluetoothStart(service.c_str(), characteristic.c_str(), value, length, type);
                    }
                
                    static void UpdateCharacteristic(const std::string& service, const std::string& characteristic, char* value){
                        /*Updates the value of a characteristic*/
                        MapMacPenMacIOSPeripheralBluetoothUpdateCharacteristicValue(service.c_str(), characteristic.c_str(), value);
                    }
                }
            }
        }
    }
}
