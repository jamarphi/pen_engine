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
#include "mac_ios_argument_buffer.h"

#ifdef __PEN_MAC_IOS__
static NSMutableDictionary* iosArgumentBuffers;

@implementation IOSArgumentBuffer

+ (void) IOSArgumentBufferInit: (unsigned int) layerId :(id<MTLBuffer>) dataBuffer{
    /*Creates an ios argument buffer for formatting vertex data*/
    IOSState* inst = [IOSState Get];
#ifndef TARGET_OS_IOS
    id<MTLBuffer> iosArgumentBuffer = [inst.iosDevice newBufferWithLength:[inst.iosArgEncoder encodedLength] options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> iosArgumentBuffer = [inst.iosDevice newBufferWithLength:[inst.iosArgEncoder encodedLength] options:MTLResourceStorageModeShared];
#endif
    [inst.iosArgEncoder setArgumentBuffer:iosArgumentBuffer offset:0];
    [inst.iosArgEncoder setBuffer:dataBuffer offset:0 atIndex:0];
#ifndef TARGET_OS_IOS
    [IOSArgumentBuffer didModifyRange: NSRangeMake(0, [iosArgumentBuffer length])];
#endif
    [iosArgumentBuffers setObject:iosArgumentBuffer forKey:[NSString stringWithFormat:@"%d", layerId]];
}

+ (void) IOSArgumentBufferBind: (unsigned int) layerId{
	/*Binds the ios argument buffer*/
    IOSState* inst = [IOSState Get];
    [inst.iosArgEncoder setArgumentBuffer:[iosArgumentBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] offset:0];
}

+ (void) IOSArgumentBufferDestroy: (unsigned int) layerId{
	/*Removes the buffer from the GPU*/
    if([iosArgumentBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] != nil){
        [iosArgumentBuffers removeObjectForKey:[NSString stringWithFormat:@"%d", layerId]];
    }
}

+(NSMutableDictionary*) IOSArgumentBuffersGet{
    /*Returns the argument buffer list*/
    return iosArgumentBuffers;
}
@end

void MapMacIOSArgumentBufferInit(unsigned int layerId){
    /*Creates an ios argument buffer for formatting vertex data*/
    NSMutableDictionary* vertexBuffers = [IOSVertexBuffer IOSVertexBuffersGet];
    [IOSArgumentBuffer IOSArgumentBufferInit:layerId :[vertexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]]];
}

void MapMacIOSArgumentBufferBind(unsigned int layerId){
    /*Binds the ios argument buffer*/
    [IOSArgumentBuffer IOSArgumentBufferBind:layerId];
}

void MapMacIOSArgumentBufferDestroy(unsigned int layerId){
    /*Removes the buffer from the GPU*/
    [IOSArgumentBuffer IOSArgumentBufferDestroy:layerId];
}
#endif
