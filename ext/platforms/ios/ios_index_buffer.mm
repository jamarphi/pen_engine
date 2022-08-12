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
#include "ios_index_buffer.h"

#ifdef __PEN_IOS__
static NSMutableDictionary* iosIndexBuffers;

@implementation IOSIndexBuffer
+ (void) IOSIndexBufferInit: (unsigned int) layerId :(int*) data :(unsigned int) count{
    /*Creates an ios index buffer*/
    IOSState* inst = [IOSState Get];
#ifndef TARGET_OS_IOS
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:count options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:count options:MTLResourceStorageModeShared];
#endif
    memcpy([iosIndexBuffer contents], data, sizeof(int) * count);
#ifndef TARGET_OS_IOS
    [iosIndexBuffer didModifyRange: NSRangeMake(0, [iosIndexBuffer length])];
#endif
    [iosIndexBuffers setObject:iosIndexBuffer forKey:[NSString stringWithFormat:@"%d", layerId]];
}

+ (void) IOSIndexBufferDestroy: (unsigned int) layerId{
	/*Removes buffer from GPU*/
    if([iosIndexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] != nil){
        [iosIndexBuffers removeObjectForKey:[NSString stringWithFormat:@"%d", layerId]];
    }
}

+ (NSMutableDictionary*) IOSIndexBuffersGet{
    /*Returns the index buffer list*/
    return iosIndexBuffers;
}
@end

void MapIOSIndexBufferInit(unsigned int layerId, int* data, unsigned int count){
    /*Creates an ios index buffer*/
    [IOSIndexBuffer IOSIndexBufferInit:layerId :data :count];
}

void MapIOSIndexBufferDestroy(unsigned int layerId){
    /*Removes buffer from GPU*/
    [IOSIndexBuffer IOSIndexBufferDestroy:layerId];
}
#endif
