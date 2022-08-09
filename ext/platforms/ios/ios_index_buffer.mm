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
static MTL::Buffer* iosIndexBuffers[30];

@implementation IOSIndexBuffer
void IOS_CPPObjectCMapping::IOSIndexBufferInit(unsigned int layerId, int* data, unsigned int count){
    /*Creates an ios index buffer*/
    [IOSIndexBuffer IOSIndexBufferInit:layerId :data :count];
}

+ (void) IOSIndexBufferInit: (unsigned int) layerId :(int*) data :(unsigned int) count{
    /*Creates an ios index buffer*/
    IOSState* inst = [IOSState Get];
	MTL::Buffer* iosIndexBuffer = inst.iosDevice->newBuffer(count, MTL::ResourceStorageModeManaged);
	std::memcpy(iosIndexBuffer->contents(), data, sizeof(int) * count);
	iosIndexBuffer->didModifyRange(NS::Range::Make(0, iosIndexBuffer->length()));
    iosIndexBuffers[layerId] = iosIndexBuffer;
}

void IOS_CPPObjectCMapping::IOSIndexBufferDestroy(unsigned int layerId){
    /*Removes buffer from GPU*/
    [IOSIndexBuffer IOSIndexBufferDestroy:layerId];
}

+ (void) IOSIndexBufferDestroy: (unsigned int) layerId{
	/*Removes buffer from GPU*/
	iosIndexBuffers[layerId]->release();
}

+(MTL::Buffer**) IOSIndexBuffersGet{
    /*Returns the index buffer list*/
    return iosIndexBuffers;
}
@end
#endif
