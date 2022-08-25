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
- (void) PenMacIOSIndexBufferInit:(int*) data :(unsigned int) count{
    /*Creates an ios index buffer*/
    PenMacIOSState* inst = [PenMacIOSState Get];
#if TARGET_OS_OSX
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:sizeof(int) * count options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> iosIndexBuffer = [inst.iosDevice newBufferWithLength:sizeof(int) * count options:MTLResourceStorageModeShared];
#endif
    memcpy([iosIndexBuffer contents], data, sizeof(int) * count);
#if TARGET_OS_OSX
    [self.iosIndexBuffer didModifyRange: NSMakeRange(0, [iosIndexBuffer length])];
#endif
    self.iosIndexBuffer = iosIndexBuffer;
}

-(void)UpdateIndices:(int*) data :(unsigned int) count{
    /*Updates the index buffer*/
     memcpy([self.iosIndexBuffer contents], data, sizeof(int) * count);
#if TARGET_OS_OSX
    [self.iosIndexBuffer didModifyRange: NSMakeRange(0, [self.iosIndexBuffer length])];
#endif
}

+ (PenMacIOSIndexBuffer*) Get{
    /*Returns an instance of PenMacIOSIndexBuffer*/
    if (!instance){
        instance = [[PenMacIOSIndexBuffer alloc] init];
    }
    return instance;
}
@end

void MapMacPenMacIOSIndexBufferInit(int* data, unsigned int count){
    /*Creates an ios index buffer*/
    [[PenMacIOSIndexBuffer Get] PenMacIOSIndexBufferInit:data :count];
}

void MapMacPenMacIOSUpdateIndices(int* data, unsigned int count){
    /*Updates the index buffer*/
    [[PenMacIOSIndexBuffer Get] UpdateIndices:data :count];
}
#endif
