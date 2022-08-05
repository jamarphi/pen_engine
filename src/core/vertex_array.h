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
#include "../state/state.h"
#ifndef __PEN_IOS__
#include "../../dependencies/glad/glad.h"
#else
#include "../../ext/platforms/ios/ios_vertex_buffer.h"
#include "../../ext/platforms/ios/ios_argument_buffer.h"
#endif
#include "vertex_buffer.h"
#include "vertex_buffer_schema.h"
#include <iostream>

class VertexBufferLayout;

class VertexArray {
private:
	unsigned int rendererId;

public:
#ifdef __PEN_IOS__
	IOSArgumentBuffer* iosArgumentBuffer;
#endif

public:
	VertexArray();
	~VertexArray();
#ifndef __PEN_IOS__
	void AddBuffer(const VertexBufferSchema& schema);
	#else __PEN_IOS__
	void AddBuffer(IOSVertexBuffer* dataBuffer);
#endif
	void Bind() const;
	void Unbind() const;
	void Destroy();
};