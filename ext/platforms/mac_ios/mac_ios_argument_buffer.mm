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
static PenMacIOSArgumentBuffer* instance;

@implementation PenMacIOSArgumentBuffer

- (void) PenMacIOSArgumentBufferInit: (unsigned int) layerId :(id<MTLBuffer>) dataBuffer{
    /*Creates an ios argument buffer for formatting vertex data*/
    PenMacIOSState* inst = [PenMacIOSState Get];
#ifndef TARGET_OS_IOS
    id<MTLBuffer> iosArgumentBuffer = [inst.iosDevice newBufferWithLength:[inst.iosArgEncoder encodedLength] options:MTLResourceStorageModeManaged];
#else
    //id<MTLBuffer> iosArgumentBuffer = [inst.iosDevice newBufferWithLength:[inst.iosArgEncoder encodedLength] options:MTLResourceStorageModeShared];
#endif
   // [inst.iosArgEncoder setArgumentBuffer:iosArgumentBuffer offset:0];
    //[inst.iosArgEncoder setBuffer:dataBuffer offset:0 atIndex:0];
#ifndef TARGET_OS_IOS
    //[PenMacIOSArgumentBuffer didModifyRange: NSMakeRange(0, [iosArgumentBuffer length])];
#endif
    //[iosArgumentBuffers setObject:iosArgumentBuffer forKey:[NSString stringWithFormat:@"%d", layerId]];
}

- (void) PenMacIOSArgumentBufferBind: (unsigned int) layerId{
	/*Binds the ios argument buffer*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    //[inst.iosArgEncoder setArgumentBuffer:[iosArgumentBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] offset:0];
}

- (void) PenMacIOSArgumentBufferDestroy: (unsigned int) layerId{
	/*Removes the buffer from the GPU*/
    if([self.iosArgumentBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] != nil){
        [self.iosArgumentBuffers removeObjectForKey:[NSString stringWithFormat:@"%d", layerId]];
    }
}

+ (PenMacIOSArgumentBuffer*) Get{
    /*Returns an instance of PenMacIOSArgumentBuffer*/
    if (!instance){
        instance = [[PenMacIOSArgumentBuffer alloc] init];
        instance.iosArgumentBuffers = [NSMutableDictionary dictionary];
    }
    return instance;
}
@end

void MapMacPenMacIOSArgumentBufferInit(unsigned int layerId){
    /*Creates an ios argument buffer for formatting vertex data*/
    NSMutableDictionary* vertexBuffers = [PenMacIOSVertexBuffer Get].iosVertexBuffers;
    [[PenMacIOSArgumentBuffer Get] PenMacIOSArgumentBufferInit:layerId :[vertexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]]];
}

void MapMacPenMacIOSArgumentBufferBind(unsigned int layerId){
    /*Binds the ios argument buffer*/
    [[PenMacIOSArgumentBuffer Get] PenMacIOSArgumentBufferBind:layerId];
}

void MapMacPenMacIOSArgumentBufferDestroy(unsigned int layerId){
    /*Removes the buffer from the GPU*/
    [[PenMacIOSArgumentBuffer Get] PenMacIOSArgumentBufferDestroy:layerId];
}
#endif
