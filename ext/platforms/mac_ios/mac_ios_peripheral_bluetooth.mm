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

#include "mac_ios_peripheral_bluetooth.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSPeripheralBluetooth* instance;
static NSMutableDictionary<NSString*,NSMutableArray<CBMutableCharacteristic*>*>*services;

@implementation PenMacIOSPeripheralBluetooth

- (void)peripheralManagerDidUpdateState:(nonnull CBPeripheralManager *)peripheral {
    /*Update made to the peripheral bluetooth state*/
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
            didAddService:(CBService *)service
                    error:(NSError *)error {
    /*Notifies of errors when adding a service*/
    if (error) {
        NSLog(@"Error publishing service: %@", [error localizedDescription]);
    }
    
    NSArray* keys = [services allKeys];
    NSMutableArray<CBUUID*>* uuids;
    for(NSString* key in keys){
        [uuids addObject:[CBUUID UUIDWithString:key]];
    }
    
    [self.peripheralManager startAdvertising:@{ CBAdvertisementDataServiceUUIDsKey :uuids }];
}

- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral
                                       error:(NSError *)error {
    /*Notifies of errors once advertising starts*/
    if (error) {
        NSLog(@"Error advertising: %@", [error localizedDescription]);
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
    didReceiveReadRequest:(CBATTRequest *)request {
    /*A read request has been received*/
    CBService* service = request.characteristic.service;
    NSMutableArray<CBMutableCharacteristic*>* serviceCharacteristics = [services objectForKey:[service.UUID UUIDString]];
    
    if ([request.characteristic.UUID isEqual:[serviceCharacteristics objectAtIndex:0].UUID]
        && [serviceCharacteristics objectAtIndex:0].permissions == CBAttributePermissionsReadable
        && request.offset <= [serviceCharacteristics objectAtIndex:0].value.length) {
        request.value = [[serviceCharacteristics objectAtIndex:0].value
                subdataWithRange:NSMakeRange(request.offset,
                [serviceCharacteristics objectAtIndex:0].value.length - request.offset)];
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorSuccess];
    }else {
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorInvalidOffset];
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
  didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests {
    /*A write request has been received*/
    int valid = 1;
    
    for (CBATTRequest* request in requests){
        CBService* service = request.characteristic.service;
        NSMutableArray<CBMutableCharacteristic*>* serviceCharacteristics = [services objectForKey:[service.UUID UUIDString]];
        
        if ([request.characteristic.UUID isEqual:[serviceCharacteristics objectAtIndex:0].UUID]
            || [serviceCharacteristics objectAtIndex:0].permissions != CBAttributePermissionsWriteable
            || request.offset > [serviceCharacteristics objectAtIndex:0].value.length) {
            valid = 0;
            [self.peripheralManager respondToRequest:request withResult:CBATTErrorInvalidOffset];
        }
    }
    
    if(valid != 0){
        for (CBATTRequest* validRequest in requests){
            CBService* service = validRequest.characteristic.service;
            NSMutableArray<CBMutableCharacteristic*>* serviceCharacteristics = [services objectForKey:[service.UUID UUIDString]];
            [serviceCharacteristics objectAtIndex:0].value = validRequest.value;
        }
        
        [self.peripheralManager respondToRequest:[requests objectAtIndex:0] withResult:CBATTErrorSuccess];
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
                  central:(CBCentral *)central
didSubscribeToCharacteristic:(CBCharacteristic *)characteristic {
    /*A subscription has been established*/
    NSString* service = [NSString stringWithFormat:@"%@:%@", [characteristic.service.UUID UUIDString], [characteristic.descriptors objectAtIndex:0].value];
    (*pen::State::Get()->mobileMacIosOnBluetoothCallback)((char*)[service UTF8String], (long)characteristic.value.length, 1);
    [self.peripheralManager updateValue:self.updateValue forCharacteristic:characteristic onSubscribedCentrals:nil];
}

- (void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral{
    /*Resends value for a subscription if queue was full earlier*/
    [self.peripheralManager updateValue:self.updateValue forCharacteristic:self.updateValueCharacteristic onSubscribedCentrals:nil];
}

-(void)Init{
    /*Starts bluetooth for the peripheral side*/
    if(!self.peripheralManager){
        self.peripheralManager =
            [[CBPeripheralManager alloc] initWithDelegate:self queue:nil options:nil];
    }
}

-(void) AddService: (NSString*) service :(NSString*) characteristic :(void*)data :(long)length :(unsigned int) type{
    /*Creates the service with its characteristic*/
    [self Init];
    CBCharacteristicProperties properties = (type == 2) ? CBCharacteristicPropertyRead : CBCharacteristicPropertyWrite;
    CBAttributePermissions permissions = (type == 2) ? CBAttributePermissionsReadable : CBAttributePermissionsWriteable;
    NSData* dataBuffer = [NSData dataWithBytes:data length:length];
    CBMutableCharacteristic* newCharacteristic =
            [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:characteristic]
             properties:properties
             value:dataBuffer permissions:permissions];
    CBMutableDescriptor* descriptor = [[CBMutableDescriptor alloc] initWithType:[CBUUID UUIDWithString:characteristic] value:characteristic];
    newCharacteristic.descriptors = @[descriptor];
    NSMutableArray<CBMutableCharacteristic*>* characteristicsList;
    [characteristicsList addObject:newCharacteristic];
    CBMutableService* newService = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:service] primary:YES];
    newService.characteristics = @[newCharacteristic];
    [services setObject:characteristicsList forKey:[newService.UUID UUIDString]];
    [self.peripheralManager addService:newService];
}

-(void) UpdateCharacteristicValue: (NSString*) service :(NSString*) characteristicDescriptor :(void*) data {
    /*Determine whether a characteristic of a service is in use*/
    CBUUID* uuid = [CBUUID UUIDWithString:service];
    NSMutableArray<CBMutableCharacteristic*>* characteristics = [services objectForKey:[uuid UUIDString]];
    for(CBMutableCharacteristic* serviceCharacteristic in characteristics){
        if([[[serviceCharacteristic.descriptors objectAtIndex:0].UUID UUIDString] isEqualToString:characteristicDescriptor]){
            NSData* dataBuffer = [NSData dataWithBytes:data length:serviceCharacteristic.value.length];
            self.updateValueCharacteristic = serviceCharacteristic;
            self.updateValue = dataBuffer;
            break;
        }
    }
}

+(PenMacIOSPeripheralBluetooth*) Get{
    /*Returns an instance of PenMacIOSPeripheralBluetooth*/
    if (!instance)
        instance = [[PenMacIOSPeripheralBluetooth alloc] init];
    return instance;
}

-(NSMutableDictionary<NSString*,NSMutableArray<CBMutableCharacteristic*>*>*) GetServices {
    /*Returns the services of the peripheral device*/
    return services;
}
@end

void MapMacPenMacIOSPeripheralBluetoothAddService(const char* service, const char* characteristic, char* value, long length, unsigned int type){
    /*Starts bluetooth for the peripheral side*/
    [[PenMacIOSPeripheralBluetooth Get] AddService:[NSString stringWithUTF8String:service] :[NSString stringWithUTF8String:characteristic] :(void*)value :length :type];
}

void MapMacPenMacIOSPeripheralBluetoothUpdateCharacteristicValue(const char* service, const char* characteristic, char* value){
    /*Update the value of a characteristic*/
    [[PenMacIOSPeripheralBluetooth Get] UpdateCharacteristicValue:[NSString stringWithUTF8String:service] :[NSString stringWithUTF8String:characteristic] :(void*)value];
}

#endif
