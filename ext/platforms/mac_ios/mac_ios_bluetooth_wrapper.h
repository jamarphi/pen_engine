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
#include "../../../src/state/state.h"
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
#ifdef __PEN_MAC_IOS__
                    for(int i = 0; i < devices.size(); i++){
                        MapMacPenMacIOSCentralBluetoothAddDevice(devices[i].c_str());
                    }
                    MapMacPenMacIOSCentralBluetoothConnect();
#endif
                }
            
                static void Stop () {
                    /*Stops scanning for peripheral devices*/
#ifdef __PEN_MAC_IOS__
                    MapMacPenMacIOSCentralBluetoothStop();
#endif
                }
            
                static std::vector<std::string> QueryPotentialConnections(){
                    /*Return the potential connections*/
#ifdef __PEN_MAC_IOS__
                    std::vector<std::string> devices;
                    unsigned int deviceCount = MapMacPenMacIOSCentralBluetoothGetCountOfPeripherals();
                    for(int i = 0; i < deviceCount; i++){
                        devices.push_back(std::string(MapMacPenMacIOSCentralBluetoothGetPeripheral()));
                    }
                    return devices;
#else
                    return {};
#endif
                }
            
                static void Connect (const char* device, const char* descriptor) {
                    /*Connect to a peripheral device*/
#ifdef __PEN_MAC_IOS__
                    MapMacPenMacIOSCentralBluetoothConnect(device, descriptor);
#endif
                }
            
                static void Read(const char* device){
                    /*Reads the value from the current characteristic of a given device*/
#ifdef __PEN_MAC_IOS__
                    MapMacPenMacIOSCentralBluetoothRead(device);
#endif
                }
            
                static void Write(char* data, long length){
                    /*Writes a value for the current characteristic to all the connected devices*/
#ifdef __PEN_MAC_IOS__
                    MapMacPenMacIOSCentralBluetoothWrite(data, length);
#endif
                }
            
                static void Disconnect(const char* device){
                    /*Disconnect from a given device*/
#ifdef __PEN_MAC_IOS__
                    MapMacPenMacIOSCentralBluetoothDisconnect(device);
#endif
                }
                /*----Central side functions----*/
            
                namespace rec {
                    /*----Peripheral side functions----*/
                    static void AddService(const char* service, const char* characteristic, char* value, long length, unsigned int type){
                        /*Adds a service for the peripheral side*/
#ifdef __PEN_MAC_IOS__
                        MapMacPenMacIOSPeripheralBluetoothStart(service, characteristic, value, length, type);
#endif
                    }
                
                    static void UpdateCharacteristic(const char* service, const char* characteristic, char* value){
                        /*Updates the value of a characteristic*/
#ifdef __PEN_MAC_IOS__
                        MapMacPenMacIOSPeripheralBluetoothUpdateCharacteristicValue(service, characteristic, value);
#endif
                    }
                    /*----Peripheral side functions----*/
                }
            }
        }
    }
}
