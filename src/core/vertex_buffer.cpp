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
#include "vertex_buffer.h"

VertexBuffer::VertexBuffer() {
	/*This constructor is used for declaring a VertexBuffer variable as a member variable of another class*/
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
	/*For static rendering*/
	glGenBuffers(1, &rendererId);
	glBindBuffer(GL_ARRAY_BUFFER, rendererId);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

}

VertexBuffer::VertexBuffer(unsigned int size) {
	/*For dynamic rendering*/
	glGenBuffers(1, &rendererId);
	glBindBuffer(GL_ARRAY_BUFFER, rendererId);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	
}

void VertexBuffer::Bind() const {
	/*Binds the vertex buffer for the layer that it is a part of*/
	glBindBuffer(GL_ARRAY_BUFFER, rendererId);

}

void VertexBuffer::Unbind() const {
	/*Unbinds the vertex buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Destroy() {
	/*Removes the vertex buffer from memory on the GPU*/
	glDeleteBuffers(1, &rendererId);
}