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
#include "vertex_array.h"

VertexArray::VertexArray() {
}

VertexArray::~VertexArray() {
	
}

void VertexArray::AddBuffer(const VertexBufferSchema& schema) {
	/*Creates a vertex array object that defines the attributes used with the vertex buffer*/
	glGenVertexArrays(1, &rendererId);
	Bind();

	/*The schema is used to determine the stride of each attribute*/
	const auto& elements = schema.Items();
	unsigned int offset = 0;
	
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		glVertexAttribPointer(i, element.elementCount, GL_FLOAT, GL_FALSE,
			schema.Stride(), (const void*)offset);
		glEnableVertexAttribArray(i);
		offset += element.elementCount * 4;
	}
}

void VertexArray::Bind() const {
	/*Binds the vertex array for the layer it is a part of*/
	glBindVertexArray(rendererId);
}

void VertexArray::Unbind() const {
	/*Unbinds the vertex array*/
	glBindVertexArray(0);

}

void VertexArray::Destroy() {
	/*Removes the vertex array from memory on the GPU*/
	glDeleteVertexArrays(1, &rendererId);
}