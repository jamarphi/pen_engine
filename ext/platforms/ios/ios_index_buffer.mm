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
void IOSIndexBuffer::IOSIndexBuffer(int* data, unsigned int count){
	iosIndexBuffer = IOSState::Get()->iosDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
	std::memcpy(iosIndexBuffer->contents(), data, sizeof(int) * count);
	iosIndexBuffer->didModifyRange(NS::Range::Make(0, iosIndexBuffer->length()));
}

void IOSIndexBuffer::Destroy(){
	/*Removes buffer from GPU*/
	iosIndexBuffer->release();
}
#endif