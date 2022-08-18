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

#include "mac_ios_http.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSHttp* instance;
static NSURLSession* session;

@implementation PenMacIOSHttp
- (NSURLSessionDataTask *)dataTaskWithRequest:(NSURLRequest *)request
                            completionHandler:(void (^)(NSData *data, NSURLResponse *response, NSError *error))completionHandler{
    /*
     Handles a GET or POST http request
     Completion request gets handled in MapMacPenMacIOSHttpRequest
     */
    return nil;
}

+ (NSURLSessionDataTask*)Get{
    /*Returns an instance of PenMacIOSHttp*/
    if (!instance){
        instance = [[PenMacIOSHttp alloc] init];
        session = [NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.defaultSessionConfiguration delegate:instance delegateQueue:nil];
    }
    return (NSURLSessionDataTask*)instance;
}

+ (NSURLSession*)GetSession{
    /*Returns the http session*/
    return session;
}
@end

void MapMacPenMacIOSHttpRequest(const char* url, unsigned int type, pen::Map<std::string,std::string>* httpBody){
    /*Sends an http request*/
    NSURL* nsUrl = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
    NSMutableURLRequest* request;
    request = [request initWithURL:nsUrl];
    request.HTTPMethod = (type == 0) ? @"GET" : @"POST";
    if(type == 1){
        NSMutableDictionary<NSString*,NSString*>* nsHttpBody = [NSMutableDictionary dictionary];
        for(int i = 0; i < httpBody->Size(); i++){
            const char* key = httpBody->items[i].first.c_str();
            const char* value = httpBody->items[i].second.c_str();
            [nsHttpBody setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
        }
        NSData* bodyData = [NSJSONSerialization dataWithJSONObject:nsHttpBody options:NSJSONWritingPrettyPrinted error:nil];
        request.HTTPBody = bodyData;
        [request setValue:@"Content-Type" forHTTPHeaderField:@"application/json"];
    }
    
    void (^completionHandler)(NSData*, NSURLResponse*, NSError*) = ^(NSData *data, NSURLResponse *response, NSError *error){
        if(data){
            NSArray* responseData = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            if(responseData){
                pen::Map<std::string,std::string> responseMap = pen::Map<std::string,std::string>();
                for(NSDictionary* responseItem in responseData){
                    NSArray* keys = [responseItem allKeys];
                    NSArray* values = [responseItem allValues];
                    NSString* key = [keys objectAtIndex:0];
                    NSString* value = [values objectAtIndex:0];
                    const char* keyStr = [key UTF8String];
                    const char* valueStr = [value UTF8String];
                    responseMap.Insert(std::string(keyStr), std::string(valueStr));
                }
                if(pen::State::Get()->mobileOnHttpCallback != nullptr){
                    (*pen::State::Get()->mobileOnHttpCallback)(responseMap);
                }
            }
        }else{
            if(pen::State::Get()->mobileOnHttpCallback != nullptr){
                (*pen::State::Get()->mobileOnHttpCallback)(pen::Map<std::string,std::string>());
            }
        }
    };
    
    NSURLSession* httpSession = [PenMacIOSHttp GetSession];
    NSURLSessionDataTask* task = [httpSession dataTaskWithRequest:request completionHandler:completionHandler];
    [task resume];
}

#endif
