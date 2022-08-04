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
#include "index_buffer.h"

IndexBuffer::IndexBuffer() {

}

IndexBuffer::IndexBuffer(int* data, unsigned int count) : indexCount(count) {
	/*Using static draw for the index buffer since it won't change*/
#ifndef __PEN_IOS__
	glGenBuffers(1, &rendererId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
#endif

}

#ifdef __PEN_IOS__
IndexBuffer::IndexBuffer(int* data, unsigned int count, MTL::Device* iosGPU) {
	/*For IOS Metal buffers*/
	iosDevice = iosGPU->retain();
	iosBuffer = iosDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
	std::memcpy(iosBuffer->contents(), data, size);
	iosBuffer->didModifyRange(NS::Range::Make(0, iosBuffer->length()));
}
#endif

IndexBuffer::~IndexBuffer() {
	

}

void IndexBuffer::Bind() const {
	/*Binds the index buffer specific to the layer it is a part of*/
#ifndef __PEN_IOS__
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererId);
#endif

}

void IndexBuffer::Unbind() const {
	/*Unbinds the index buffer*/
#ifndef __PEN_IOS__
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

}

void IndexBuffer::Destroy() {
	/*Removes the index buffer from memory on the GPU*/
#ifndef __PEN_IOS__
	glDeleteBuffers(1, &rendererId);
#endif
}