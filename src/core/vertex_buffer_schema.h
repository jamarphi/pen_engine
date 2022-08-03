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
#pragma once
#include <vector>
#include "../state/config.h"
#ifndef __PEN_IOS__
#include "../../dependencies/glad/glad.h"
#endif

struct VertexBufferSchemaElement {
	/*This is used to contain metadata about each attribute being used for the vertex buffers*/
	unsigned int elementCount;
};

class VertexBufferSchema {
private:
	unsigned int vertexStride;
	std::vector<VertexBufferSchemaElement> vertexItems;
public:
	VertexBufferSchema() {
		vertexStride = 0;
	};

	inline unsigned int Stride() const {
		return vertexStride;
	}

	inline const std::vector<VertexBufferSchemaElement> Items() const {
		return vertexItems;
	}

	void Push(unsigned int count, unsigned int stride = 0) {
		/*Add float attributes to the layout*/
		VertexBufferSchemaElement ve = { count };
		vertexItems.push_back(ve);
		if (stride > 0){
			vertexStride += stride * 4;
		}
		else {
			vertexStride += count * 4;
		}

	}
};