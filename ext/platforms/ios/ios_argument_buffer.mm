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
#include "ios_argument_buffer.h"

#ifdef __PEN_IOS__
static MTL::Buffer* iosArgumentBuffers[30];

@implementation IOSArgumentBuffer

void IOS_CPPObjectCMapping::IOSArgumentBufferInit(unsigned int layerId){
    /*Creates an ios argument buffer for formatting vertex data*/
    MTL::Buffer** vertexBuffers = [IOSVertexBuffer IOSVertexBuffersGet];
    [IOSArgumentBuffer IOSArgumentBufferInit:layerId :vertexBuffers[layerId]];
}

+ (void) IOSArgumentBufferInit: (unsigned int) layerId :(MTL::Buffer*) dataBuffer{
    /*Creates an ios argument buffer for formatting vertex data*/
    IOSState* inst = [IOSState Get];
	MTL::Buffer* iosArgumentBuffer = inst.iosDevice->newBuffer(inst.iosArgEncoder->encodedLength(), MTL::ResourceStorageModeManaged);
	inst.iosArgEncoder->setArgumentBuffer(iosArgumentBuffer, 0);
	inst.iosArgEncoder->setBuffer(dataBuffer, 0, 0);
	iosArgumentBuffer->didModifyRange(NS::Range::Make(0, iosArgumentBuffer->length()));
    iosArgumentBuffers[layerId] = iosArgumentBuffer;
}

void IOS_CPPObjectCMapping::IOSArgumentBufferBind(unsigned int layerId){
    /*Binds the ios argument buffer*/
    [IOSArgumentBuffer IOSArgumentBufferBind:layerId];
}

+ (void) IOSArgumentBufferBind: (unsigned int) layerId{
	/*Binds the ios argument buffer*/
    IOSState* inst = [IOSState Get];
	inst.iosArgEncoder->setArgumentBuffer(iosArgumentBuffers[layerId], 0);
}

void IOS_CPPObjectCMapping::IOSArgumentBufferDestroy(unsigned int layerId){
    /*Removes the buffer from the GPU*/
    [IOSArgumentBuffer IOSArgumentBufferDestroy:layerId];
}

+ (void) IOSArgumentBufferDestroy: (unsigned int) layerId{
	/*Removes the buffer from the GPU*/
	iosArgumentBuffers[layerId]->release();
}

+(MTL::Buffer**) IOSArgumentBuffersGet{
    /*Returns the argument buffer list*/
    return iosArgumentBuffers;
}
@end
#endif
