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

#include "mac_ios_util.h"

#ifdef __PEN_MAC_IOS__
@implementation PenMacIOSUtil
+ (const char*) IOSLoadAsset:(const char*) path :(const char*) mimeType {
    /*Loads in an asset*/
    const char* file;
    NSBundle* main = [NSBundle mainBundle];
    NSString* resourcePath = [main pathForResource:[NSString stringWithUTF8String:path] ofType:[NSString stringWithUTF8String:mimeType]];
    NSString* data = [NSString stringWithContentsOfFile:resourcePath encoding:NSUTF8StringEncoding error:nil];
    file = [data UTF8String];
    return file;
}
@end

const char* MapMacIOSLoadAsset(const char* path, const char* mimeType){
    /*Loads in an asset*/
    return [PenMacIOSUtil IOSLoadAsset:path :mimeType];
}
#endif
