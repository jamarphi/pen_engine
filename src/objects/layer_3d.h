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
#include "layer.h"
#include "item_3d.h"

namespace pen {
	class Layer3D : public pen::Layer {
	public:
		Layer3D();
		Layer3D(uint16_t generalId, unsigned int objectShapeType, bool objectIsFixed, bool objectIsSingular, bool objectIsWireFrame, bool objectIsInstanced, std::vector<pen::Vec3*> dataList = {});
		~Layer3D();

		bool Push(pen::ui::Item* item, const unsigned int& offset = 0);

#ifndef __PEN_MAC_IOS__
		void CombineBuffers();
#else
		void CombineBuffers(int layerIndex);
#endif
		void InitializeIndices();
	};
}
