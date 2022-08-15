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

#ifdef __PEN_MAC_IOS__
#import "mac_ios_state.h"
#import "../../../src/state/state.h"
#import <CoreBluetooth/CoreBluetooth.h>

@class PenMacIOSPeripheralBluetooth;

@interface PenMacIOSPeripheralBluetooth : NSObject<CBPeripheralManagerDelegate>
@property CBPeripheralManager* peripheralManager;
@property CBMutableCharacteristic* updateValueCharacteristic;
@property NSData* updateValue;

/*Inherited from CoreBluetooth for the peripheral side*/
- (void)peripheralManagerDidUpdateState:(nonnull CBPeripheralManager *)peripheral;
- (void)peripheralManager:(CBPeripheralManager *)peripheral
            didAddService:(CBService *)service
                    error:(NSError *)error;
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral
                                       error:(NSError *)error;
- (void)peripheralManager:(CBPeripheralManager *)peripheral
    didReceiveReadRequest:(CBATTRequest *)request;
- (void)peripheralManager:(CBPeripheralManager *)peripheral
  didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests;
- (void)peripheralManager:(CBPeripheralManager *)peripheral
                  central:(CBCentral *)central
didSubscribeToCharacteristic:(CBCharacteristic *)characteristic;
- (void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral;

/*Peripheral side functions*/
-(void) Init;
-(void) AddService: (NSString*) service :(NSString*) characteristic :(void*)data :(long) length :(unsigned int) type;
-(unsigned int) UpdateCharacteristicValue: (NSString*) service :(NSString*) characteristicDescriptor
                                         :(void*) data;
+(PenMacIOSPeripheralBluetooth*) Get;
-(NSMutableDictionary<NSString*,NSMutableArray<CBMutableCharacteristic*>*>*) GetServices;
@end
#endif
