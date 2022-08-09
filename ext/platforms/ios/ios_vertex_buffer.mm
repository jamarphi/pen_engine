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

#include "ios_vertex_buffer.h"

#ifdef __PEN_IOS__
static MTL::Buffer* iosVertexBuffers[30];

@implementation IOSVertexBuffer
void IOS_CPPObjectCMapping::IOSVertexBufferInit(unsigned int layerId, BatchVertexData* data, unsigned int size){
    /*Creates a vertex buffer for a specific layer*/
    [IOSVertexBuffer IOSVertexBufferInit :layerId :data :size];
}

+ (void) IOSVertexBufferInit: (unsigned int) layerId
   :(BatchVertexData*) data
   :(unsigned int) size{
    /*Creates a vertex buffer for a specific layer*/
    IOSState* inst = [IOSState Get];
	MTL::Buffer* iosVertexBuffer = inst.iosDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
	std::memcpy(iosVertexBuffer->contents(), data, size);
	iosVertexBuffer->didModifyRange(NS::Range::Make(0, iosVertexBuffer->length()));
    iosVertexBuffers[layerId] = iosVertexBuffer;
}

void IOS_CPPObjectCMapping::IOSVertexBufferDestroy(unsigned int layerId){
    /*Removes the buffer from the GPU*/
    [IOSVertexBuffer IOSVertexBufferDestroy:layerId];
}

+ (void) IOSVertexBufferDestroy: (unsigned int) layerId{
	/*Removes the buffer from the GPU*/
    iosVertexBuffers[layerId]->release();
}

+ (MTL::Buffer**) IOSVertexBuffersGet{
    /*Returns the vertex buffer list*/
    return iosVertexBuffers;
}
@end
#endif
