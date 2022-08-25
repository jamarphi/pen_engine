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

#include "../renderer/renderer.h"
#include "../core/texture.h"
#include "../core/vertex_buffer_schema.h"
#include "../ops/vectors/vec4.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec2.h"
#include "shape.h"
#include "item.h"
#include <iostream>
#include <vector>

#ifndef __PEN_MAC_IOS__
#define MAX_OBJECTS 10000
#define MAX_OBJECTS_SINGULAR 10000
#define RENDERER_VERTEX_SIZE sizeof(BatchVertexData)
#define RENDERER_BUFFER_SIZE 6 * RENDERER_VERTEX_SIZE * MAX_OBJECTS
#define RENDERER_INDICES_SIZE MAX_OBJECTS * 6
#define THREE_D_RENDERER_INDICES_SIZE RENDERER_INDICES_SIZE
#define BATCH_VERTEX_ELEMENTS 10
#endif

namespace pen {
	class Layer {
	public:
		uint16_t id;
#ifndef __PEN_MAC_IOS__
		int batchIndices[RENDERER_INDICES_SIZE];
        int indexCount;
#else
        static int* batchIndices;
        static bool buffersInitialized;
        static int indexCount;
#endif
		
		unsigned int itemCount = 0;
		VertexArray va;
		VertexBuffer vb;
		IndexBuffer ib;
#ifndef __PEN_MAC_IOS__
		float batchVertices[MAX_OBJECTS * BATCH_VERTEX_ELEMENTS];
#else
		static BatchVertexData* batchVertices;
#endif
		std::vector<pen::ui::Item*> layerItems;
		pen::Vec3 translation;
		unsigned int shapeType;
		bool isFixed = false;
		bool is3D = false;
		bool isWireFrame = false;
		pen::Mat4x4 model;
		bool isInstanced = false;
#ifndef __PEN_MAC_IOS__
		std::vector<pen::Vec3*> instancedDataList;
#else
        std::vector<pen::Vec3*>* instancedDataList = nullptr;
#endif

		/*Only one main item can be pushed to this layer*/
		bool isSingular = false;

		/*For pixel-by-pixel rendering*/
		unsigned char* pixels = nullptr;

		/*For offsetting into 3D models*/
		unsigned int itemOffset = 0;

		Layer();
		Layer(uint16_t generalId, unsigned int objectShapeType, bool objectIsFixed, bool objectIsSingular, bool objectIsWireFrame);
		~Layer();

		void Initialize();
#ifndef __PEN_MAC_IOS__
		void Update();
#else
		void Update(int layerIndex);
#endif
		virtual bool Push(pen::ui::Item* item, const unsigned int& offset = 0);
		void Pop();
		void Destroy();
#ifndef __PEN_MAC_IOS__
		virtual void CombineBuffers();
#else
		virtual void CombineBuffers(int layerIndex);
#endif
		virtual void InitializeIndices();

	private:
		void UpdateIndexCount(int* idxCount, const unsigned int& complexIdxCount);
	};
}
