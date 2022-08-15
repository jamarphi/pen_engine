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

#include "mac_ios_socket.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSSocket* instance;
//static NSURLSession* session;
static NSURLSessionWebSocketTask* task;

@implementation PenMacIOSSocket
- (void)URLSession:(NSURLSession *)socketSession
     webSocketTask:(NSURLSessionWebSocketTask *)webSocketTask
didOpenWithProtocol:(NSString *)protocol{
    /*Handshake successful*/
    (*pen::State::Get()->mobileOnSocketCallback)(nullptr, 0);
}

- (void)URLSession:(NSURLSession *)session
     webSocketTask:(NSURLSessionWebSocketTask *)webSocketTask
  didCloseWithCode:(NSURLSessionWebSocketCloseCode)closeCode
            reason:(NSData *)reason{
    /*Socket connection has closed*/
    (*pen::State::Get()->mobileOnSocketCallback)(nullptr, 2);
}

- (void) Connect:(NSString*)url {
    /*Connects to a server*/
    NSURL* nsUrl = [NSURL URLWithString:url];
    task = [self.session webSocketTaskWithURL:nsUrl];
}

- (void) Send:(NSData*) data{
    /*Sends data to the server*/
    NSURLSessionWebSocketMessage* message;
    message = [message initWithData:data];
    
    void (^completionHandler)(NSError*) = ^(NSError *error){
        if(error){
            NSLog(@"%@", error);
        }
    };
    [task sendMessage:message completionHandler:completionHandler];
}

- (void) Receive{
    /*Receives data from the server*/
    void (^completionHandler)(NSURLSessionWebSocketMessage*, NSError*) = ^(NSURLSessionWebSocketMessage* message, NSError *error){
        if(error){
            NSLog(@"%@", error);
        }else {
            NSData* data = message.data;
            const void* dataBuffer = [data bytes];
            (*pen::State::Get()->mobileOnSocketCallback)((char*)dataBuffer, 1);
        }
    };
    [task receiveMessageWithCompletionHandler:completionHandler];
}


+ (PenMacIOSSocket*)Get{
    /*Returns an instance of PenMacIOSHttp*/
    if (!instance){
        instance = [[PenMacIOSSocket alloc] init];
        instance.session = [NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.defaultSessionConfiguration delegate:instance delegateQueue:nil];
    }
    return (PenMacIOSSocket*)instance;
}

+ (NSURLSession*)GetSession{
    /*Returns the current session in use*/
    return [PenMacIOSSocket Get].session;
}

+ (NSURLSessionWebSocketTask*)GetTask{
    /*Returns the current socket task in use*/
    return task;
}
@end

void MapMacPenMacIOSSocketConnect(const char* url){
    /*Connects to a server*/
    [[PenMacIOSSocket Get] Connect:[NSString stringWithUTF8String:url]];
}

void MapMacPenMacIOSSocketSend(char* data, long length){
    /*Sends data to the server*/
    NSData* dataBuffer = [NSData dataWithBytes:(void*)data length:length];
    [[PenMacIOSSocket Get] Send:dataBuffer];
}

void MapMacPenMacIOSSocketReceive(){
    /*Receives data from the server*/
    [[PenMacIOSSocket Get] Receive];
}

#endif
