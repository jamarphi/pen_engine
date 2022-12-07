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
#include "../../state/layer_manager.h"
#include "../position.h"
#include "../../objects/item_3d.h"
#include "../../ops/operations/3d.h"
#include "../../renderer/render.h"

namespace pen {
	struct Animation3DItem {
		pen::GraphicallyAcceleratedItem3D* item;
		unsigned int type;
		bool infinite;
		int frames;
		bool ran;
		float unitA;
		float unitB;
		float unitC;
	};

	class Animation3D {
	public:
		static std::vector<pen::Animation3DItem> animList;

	public:
		static void Add(pen::GraphicallyAcceleratedItem3D* item, const unsigned int& type, const long& ms, const bool& infinite, const float& unitA, const float& unitB = 0, const float& unitC = 0);
		static bool CheckStatus(const pen::Animation3DItem& item);
		static void Animate(pen::Animation3DItem item);
		static void Run();
	};
}
