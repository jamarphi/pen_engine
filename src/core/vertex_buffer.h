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
#include "../state/config.h"
#ifndef __PEN_MAC_IOS__
#include "../../dependencies/glad/glad.h"
#endif
#include "../../ext/platforms/mac_ios/mac_ios_cpp_objective_c_mapping.h"
#include <iostream>

class VertexBuffer {
private:
	unsigned int rendererId;
public:
	VertexBuffer();
	/*Regular buffer constructor*/
#ifndef __PEN_MAC_IOS__
	VertexBuffer(const void* data, unsigned int size);
    void Destroy();
#else
	VertexBuffer(unsigned int layerId, BatchVertexData* data, unsigned int size);
    void Destroy(unsigned int layerId);
#endif
	/*For text rendering*/
	VertexBuffer(unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};
