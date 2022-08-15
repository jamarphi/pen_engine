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

#include "mac_ios_bluetooth.h"

#ifdef __PEN_MAC_IOS__
static IOSBluetooth* instance;
static NSMutableArray<CBPeripheral*>* peripherals;
static NSMutableArray<CBUUID*>* preferredDevices;
static NSMutableArray<CBPeripheral*>* receivers;
static NSMutableArray<CBCharacteristic*>* characteristics;

@implementation IOSBluetooth

- (void)centralManagerDidUpdateState:(nonnull CBCentralManager *)central {
    /*Update made to bluetooth state*/
    
}

- (void)centralManager:(CBCentralManager *)central
 didDiscoverPeripheral:(CBPeripheral *)peripheral
     advertisementData:(NSDictionary *)advertisementData
                  RSSI:(NSNumber *)RSSI {
    /*A new peripheral device is discovered*/
    NSLog(@"Discovered %@", peripheral.name);
    [peripherals addObject:peripheral];
    if([peripherals count] == [preferredDevices count] && [preferredDevices count] > 0){
        [self Stop];
    }
}

- (void)centralManager:(CBCentralManager *)central
  didConnectPeripheral:(CBPeripheral *)peripheral {
    /*The targeted peripheral is connected*/
    peripheral.delegate = self;
    if(![receivers containsObject:peripheral]){
        [receivers addObject:peripheral];
    }
    [peripheral discoverServices:nil];
}

- (void)peripheral:(CBPeripheral *)peripheral
didDiscoverServices:(NSError *)error {
    /*Retrieves available services from connected peripheral device*/
    for (CBService *service in peripheral.services) {
        [peripheral discoverCharacteristics:nil forService:service];
    }
}

- (void)peripheral:(CBPeripheral *)peripheral
didDiscoverCharacteristicsForService:(CBService *)service
             error:(NSError *)error {
    /*Retrieves the available characteristics for a service*/
    for (CBCharacteristic *characteristic in service.characteristics) {
        NSArray<CBDescriptor*>* descriptors = [characteristic descriptors];
        for(CBDescriptor* descriptor in descriptors){
            if([[descriptor.UUID UUIDString] isEqualToString:self.characteristicDescriptor]){
                [characteristics addObject:characteristic];
                [peripheral setNotifyValue:YES forCharacteristic:characteristic];
                break;
                break;
            }
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral
didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error {
    /*Returns updates from a read for a characteristic*/
    NSData* data = characteristic.value;
    char* dataBuffer = (char*)data.bytes;
    long length = (long)data.length;
    (*pen::State::Get()->mobileMacIosOnBluetoothCallback)(dataBuffer, length);
}

- (void)peripheral:(CBPeripheral *)peripheral
didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error {
    /*Returns updates from a subscription for a characteristic*/
    if (error) {
        NSLog(@"Error changing notification state: %@",
           [error localizedDescription]);
    }
    NSData* data = characteristic.value;
    char* dataBuffer = (char*)data.bytes;
    long length = (long)data.length;
    (*pen::State::Get()->mobileMacIosOnBluetoothCallback)(dataBuffer, length);
}

- (void)peripheral:(CBPeripheral *)peripheral
didWriteValueForCharacteristic:(CBCharacteristic *)characteristic
             error:(NSError *)error {
    /*A value has been written to the current characteristic*/
    if (error) {
        NSLog(@"Error writing characteristic value: %@",
            [error localizedDescription]);
    }
}

-(void) Init {
    /*Sets up bluetooth*/
    self.manager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:nil];
    [self.manager scanForPeripheralsWithServices:preferredDevices options:nil];
}

-(void) Stop {
    /*Stops the scanning of peripherals*/
    [self.manager stopScan];
}

-(void) Connect:(CBPeripheral*) peripheral{
    /*Connects to a peripheral device*/
    [self.manager connectPeripheral:peripheral options:nil];
}

-(void) Read:(NSString*) device{
    /*Reads the value from the current characteristic of a given device*/
    if(receivers != nil && characteristics != nil){
        for(CBPeripheral* receiver in receivers){
            if([receiver.name isEqualToString:device]){
                [receiver readValueForCharacteristic:[characteristics objectAtIndex:[receivers indexOfObject:receiver]]];
                break;
            }
        }
    }
}

-(void) Write: (const void*) bytes :(long)length {
    /*Writes a value for the current characteristic to all the connected devices*/
    NSData* data = [NSData dataWithBytes:bytes length:length];
    for(CBPeripheral* receiver in receivers){
        [receiver writeValue:data forCharacteristic:[characteristics objectAtIndex:[receivers indexOfObject:receiver]]
            type:CBCharacteristicWriteWithResponse];
    }
}

-(void) Disconnect: (NSString*) device {
    /*Disconnect from a given device*/
    unsigned int index = 0;
    for(CBPeripheral* receiver in receivers){
        if([receiver.name isEqualToString:device]){
            break;
        }
        index++;
    }
    
    if(index <= [receivers count] && [receivers count] > 0){
        [receivers removeObjectAtIndex:index];
        [characteristics removeObjectAtIndex:index];
    }
}

+(IOSBluetooth*) Get{
    /*Returns an instance of IOSBluetooth*/
    if (!instance)
        instance = [[IOSBluetooth alloc] init];
    return instance;
}

-(NSMutableArray<CBPeripheral*>*) GetPeripherals {
    /*Returns the peripheral devices that are of interest*/
    return peripherals;
}

-(NSMutableArray<CBUUID*>*) GetPreferredDevices {
    /*Returns the preferred devices*/
    return preferredDevices;
}

-(NSMutableArray<CBPeripheral*>*) GetReceivers {
    /*Returns the connected devices*/
    return receivers;
}

-(NSMutableArray<CBCharacteristic*>*) GetCharacteristics {
    /*Returns the characteristics of interest for the connected devices*/
    return characteristics;
}
@end

void MapMacIOSBluetoothAddDevice(const char* device){
    /*Add a device to preferred devices*/
    NSString* deviceStr = [NSString stringWithUTF8String:device];
    CBUUID* uuid = [CBUUID UUIDWithString:deviceStr];
    [[[IOSBluetooth Get] GetPreferredDevices] addObject:uuid];
}

void MapMacIOSBluetoothScan(){
    /*Start scanning for peripheral devices*/
    [[IOSBluetooth Get] Init];
}

void MapMacIOSBluetoothStop(){
    /*Stops scanning for peripheral devices*/
    [[IOSBluetooth Get] Stop];
}

unsigned int MapMacIOSBluetoothGetCountOfPeripherals(){
    /*Returns the number of peripherals*/
    return [[[IOSBluetooth Get] GetPeripherals] count];
}

const char* MapMacIOSBluetoothGetPeripheral(unsigned int index){
    /*Returns a peripheral device*/
    CBPeripheral* peripheral = [[[IOSBluetooth Get] GetPeripherals] objectAtIndex:index];
    return [peripheral.name UTF8String];
}

void MapMacIOSBluetoothConnect(const char* device, const char* deviceCharacteristicDescriptor){
    /*Connect to a peripheral device*/
    NSString* deviceStr = [NSString stringWithUTF8String:device];
    NSMutableArray<CBPeripheral*>* devices = [[IOSBluetooth Get] GetPeripherals];
    for(CBPeripheral* peripheral in devices){
        if([peripheral.name isEqualToString:deviceStr]){
            IOSBluetooth* inst = [IOSBluetooth Get];
            inst.characteristicDescriptor = [NSString stringWithUTF8String:deviceCharacteristicDescriptor];
            [inst Connect:peripheral];
            break;
        }
    }
}

void MapMacIOSBluetoothRead(const char* device){
    /*Reads the value from the current characteristic of a given device*/
    [[IOSBluetooth Get] Read:[NSString stringWithUTF8String:device]];
}

void MapMacIOSBluetoothWrite(char* data, long length){
    /*Reads the value of the current characteristic*/
    [[IOSBluetooth Get] Write:(const void*)data :length];
}

void MapMacIOSBluetoothDisconnect(const char* device){
    /*Disconnect from a given device*/
    [[IOSBluetooth Get] Disconnect:[NSString stringWithUTF8String:device]];
}
#endif
