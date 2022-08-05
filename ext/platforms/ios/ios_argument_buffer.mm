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
void IOSArgumentBuffer::IOSArgumentBuffer(IOSVertexBuffer* dataBuffer){
	iosArgumentBuffer = IOSState::Get()->iosDevice->newBuffer(IOSState::Get()->iosArgEncoder->encodedLength(), MTL::ResourceStorageModeManaged);
	IOSState::Get()->iosArgEncoder->setArgumentBuffer(iosArgumentBuffer, 0);
	IOSState::Get()->iosArgEncoder->setBuffer(dataBuffer, 0, 0);
	iosArgumentBuffer->didModifyRange(NS::Range::Make(0, iosArgumentBuffer->length()));
}

void IOSArgumentBuffer::Bind(){
	/*Binds the ios argument buffer*/
	IOSState::Get()->iosArgEncoder->setArgumentBuffer(iosArgumentBuffer, 0);
}

void IOSArgumentBuffer::Destroy(){
	/*Removes the buffer from the GPU*/
	iosArgumentBuffer->release();
}
#endif