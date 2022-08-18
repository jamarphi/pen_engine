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

#include "mac_ios_central_bluetooth.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSCentralBluetooth* instance;
static NSMutableArray<CBPeripheral*>* peripherals;
static NSMutableArray<CBUUID*>* preferredDevices;
static NSMutableArray<CBPeripheral*>* receivers;
static NSMutableArray<CBCharacteristic*>* characteristics;

@implementation PenMacIOSCentralBluetooth

- (void)centralManagerDidUpdateState:(nonnull CBCentralManager *)central {
    /*Update made to the central bluetooth state*/
    peripherals = [[NSMutableArray alloc] init];
    preferredDevices = [[NSMutableArray alloc] init];
    receivers = [[NSMutableArray alloc] init];
    characteristics = [[NSMutableArray alloc] init];
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
    if(pen::State::Get()->mobileMacIosOnBluetoothCallback != nullptr){
        (*pen::State::Get()->mobileMacIosOnBluetoothCallback)(dataBuffer, length, 0);
    }
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
    if(pen::State::Get()->mobileMacIosOnBluetoothCallback != nullptr){
        (*pen::State::Get()->mobileMacIosOnBluetoothCallback)(dataBuffer, length, 0);
    }
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
    if(!self.centralManager){
        self.centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:nil];
    }
    [self.centralManager scanForPeripheralsWithServices:preferredDevices options:nil];
}

-(void) Stop {
    /*Stops the scanning of peripherals*/
    [self.centralManager stopScan];
}

-(void) Connect:(CBPeripheral*) peripheral{
    /*Connects to a peripheral device*/
    [self.centralManager connectPeripheral:peripheral options:nil];
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

+(PenMacIOSCentralBluetooth*) Get{
    /*Returns an instance of PenMacIOSCentralBluetooth*/
    if (!instance)
        instance = [[PenMacIOSCentralBluetooth alloc] init];
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

void MapMacPenMacIOSCentralBluetoothAddDevice(const char* device){
    /*Add a device to preferred devices*/
    NSString* deviceStr = [NSString stringWithUTF8String:device];
    CBUUID* uuid = [CBUUID UUIDWithString:deviceStr];
    [[[PenMacIOSCentralBluetooth Get] GetPreferredDevices] addObject:uuid];
}

void MapMacPenMacIOSCentralBluetoothScan(){
    /*Start scanning for peripheral devices*/
    [[PenMacIOSCentralBluetooth Get] Init];
}

void MapMacPenMacIOSCentralBluetoothStop(){
    /*Stops scanning for peripheral devices*/
    [[PenMacIOSCentralBluetooth Get] Stop];
}

unsigned int MapMacPenMacIOSCentralBluetoothGetCountOfPeripherals(){
    /*Returns the number of peripherals*/
    return [[[PenMacIOSCentralBluetooth Get] GetPeripherals] count];
}

const char* MapMacPenMacIOSCentralBluetoothGetPeripheral(unsigned int index){
    /*Returns a peripheral device*/
    CBPeripheral* peripheral = [[[PenMacIOSCentralBluetooth Get] GetPeripherals] objectAtIndex:index];
    return [peripheral.name UTF8String];
}

void MapMacPenMacIOSCentralBluetoothConnect(const char* device, const char* deviceCharacteristicDescriptor){
    /*Connect to a peripheral device*/
    NSString* deviceStr = [NSString stringWithUTF8String:device];
    NSMutableArray<CBPeripheral*>* devices = [[PenMacIOSCentralBluetooth Get] GetPeripherals];
    for(CBPeripheral* peripheral in devices){
        if([peripheral.name isEqualToString:deviceStr]){
            PenMacIOSCentralBluetooth* inst = [PenMacIOSCentralBluetooth Get];
            inst.characteristicDescriptor = [NSString stringWithUTF8String:deviceCharacteristicDescriptor];
            [inst Connect:peripheral];
            break;
        }
    }
}

void MapMacPenMacIOSCentralBluetoothRead(const char* device){
    /*Reads the value from the current characteristic of a given device*/
    [[PenMacIOSCentralBluetooth Get] Read:[NSString stringWithUTF8String:device]];
}

void MapMacPenMacIOSCentralBluetoothWrite(char* data, long length){
    /*Reads the value of the current characteristic*/
    [[PenMacIOSCentralBluetooth Get] Write:(const void*)data :length];
}

void MapMacPenMacIOSCentralBluetoothDisconnect(const char* device){
    /*Disconnect from a given device*/
    [[PenMacIOSCentralBluetooth Get] Disconnect:[NSString stringWithUTF8String:device]];
}
#endif
