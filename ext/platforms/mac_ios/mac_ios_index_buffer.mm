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
#include "mac_ios_index_buffer.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSIndexBuffer* instance;

@implementation PenMacIOSIndexBuffer
- (void) PenMacIOSIndexBufferInit: (unsigned int) layerId :(int*) data :(unsigned int) count{
    /*Creates an ios index buffer*/
    PenMacIOSState* inst = [PenMacIOSState Get];
#ifndef TARGET_OS_IOS
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:sizeof(int) * count options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:sizeof(int) * count options:MTLResourceStorageModeShared];
#endif
    memcpy([iosIndexBuffer contents], data, sizeof(int) * count);
#ifndef TARGET_OS_IOS
    [self.iosIndexBuffer didModifyRange: NSMakeRange(0, [iosIndexBuffer length])];
#endif
    [self.iosIndexBuffers setObject:iosIndexBuffer forKey:[NSString stringWithFormat:@"%d", layerId]];
}

- (void) PenMacIOSIndexBufferDestroy: (unsigned int) layerId{
	/*Removes buffer from GPU*/
    if([self.iosIndexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] != nil){
        [self.iosIndexBuffers removeObjectForKey:[NSString stringWithFormat:@"%d", layerId]];
    }
}

+ (PenMacIOSIndexBuffer*) Get{
    /*Returns an instance of PenMacIOSIndexBuffer*/
    if (!instance){
        instance = [[PenMacIOSIndexBuffer alloc] init];
        instance.iosIndexBuffers = [NSMutableDictionary dictionary];
    }
    return instance;
}
@end

void MapMacPenMacIOSIndexBufferInit(unsigned int layerId, int* data, unsigned int count){
    /*Creates an ios index buffer*/
    [[PenMacIOSIndexBuffer Get] PenMacIOSIndexBufferInit:layerId :data :count];
}

void MapMacPenMacIOSIndexBufferDestroy(unsigned int layerId){
    /*Removes buffer from GPU*/
    [[PenMacIOSIndexBuffer Get] PenMacIOSIndexBufferDestroy:layerId];
}
#endif
