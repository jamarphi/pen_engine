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

#include "mac_ios_vertex_buffer.h"

#ifdef __PEN_MAC_IOS__
static NSMutableDictionary* iosVertexBuffers;

@implementation PenMacIOSVertexBuffer
+ (void) PenMacIOSVertexBufferInit: (unsigned int) layerId
   :(BatchVertexData*) data
   :(unsigned int) size{
    /*Creates a vertex buffer for a specific layer*/
    PenMacIOSState* inst = [PenMacIOSState Get];
#ifndef TARGET_OS_IOS
    id<MTLBuffer> iosVertexBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> iosVertexBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeShared];
#endif
    memcpy([iosVertexBuffer contents], data, size);
#ifndef TARGET_OS_IOS
    [PenMacIOSVertexBuffer didModifyRange: NSRangeMake(0, [iosVertexBuffer length])];
#endif
    [iosVertexBuffers setObject:iosVertexBuffer forKey:[NSString stringWithFormat:@"%d", layerId]];
}

+ (void) PenMacIOSVertexBufferDestroy: (unsigned int) layerId{
	/*Removes the buffer from the GPU*/
    if([iosVertexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] != nil){
        [iosVertexBuffers removeObjectForKey:[NSString stringWithFormat:@"%d", layerId]];
    }
}

+ (NSMutableDictionary*) PenMacIOSVertexBuffersGet{
    /*Returns the vertex buffer list*/
    return iosVertexBuffers;
}
@end

void MapMacPenMacIOSVertexBufferInit(unsigned int layerId, BatchVertexData* data, unsigned int size){
    /*Creates a vertex buffer for a specific layer*/
    [PenMacIOSVertexBuffer PenMacIOSVertexBufferInit :layerId :data :size];
}

void MapMacPenMacIOSVertexBufferDestroy(unsigned int layerId){
    /*Removes the buffer from the GPU*/
    [PenMacIOSVertexBuffer PenMacIOSVertexBufferDestroy:layerId];
}
#endif
