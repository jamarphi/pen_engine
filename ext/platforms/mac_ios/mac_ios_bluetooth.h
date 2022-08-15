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

@class IOSBluetooth;

@interface IOSBluetooth : NSObject<CBCentralManagerDelegate, CBPeripheralDelegate>
@property CBCentralManager* manager;
@property NSString* characteristicDescriptor;

/*Inherited from CoreBluetooth*/
- (void)centralManagerDidUpdateState:(nonnull CBCentralManager *)central;
- (void)centralManager:(CBCentralManager *)central
 didDiscoverPeripheral:(CBPeripheral *)peripheral
     advertisementData:(NSDictionary *)advertisementData
                  RSSI:(NSNumber *)RSSI;
- (void)centralManager:(CBCentralManager *)central
  didConnectPeripheral:(CBPeripheral *)peripheral;
- (void)peripheral:(CBPeripheral *)peripheral
didDiscoverServices:(NSError *)error;
- (void)peripheral:(CBPeripheral *)peripheral
didDiscoverCharacteristicsForService:(CBService *)service
             error:(NSError *)error;
- (void)peripheral:(CBPeripheral *)peripheral
didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error;
- (void)peripheral:(CBPeripheral *)peripheral
didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error;
- (void)peripheral:(CBPeripheral *)peripheral
didWriteValueForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error;

-(void) Init;
-(void) Connect:(CBPeripheral*) peripheral;
-(void) Read:(NSString*) device;
-(void) Write: (const void*) bytes :(long)length;
+(IOSBluetooth*) Get;
-(NSMutableArray<CBPeripheral*>*) GetPeripherals;
-(NSMutableArray<CBUUID*>*) GetPreferredDevices;
-(NSMutableArray<CBPeripheral*>*) GetReceivers;
-(NSMutableArray<CBCharacteristic*>*) GetCharacteristics;
@end
#endif
