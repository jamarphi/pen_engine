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
#include "../objects/item.h"
#include "../state/layer_manager.h"
#include "position.h"

namespace pen {
	namespace ui {
		enum AnimationType {
			TRANSLATE = 0,
			ROTATEX = 1,
			ROTATEY = 2,
			ROTATEZ = 3,
			SCALE = 4
		};

		struct AnimationItem {
			pen::ui::Item* item;
			unsigned int type;
			bool infinite;
			int frames;
			bool ran;
			float unitA;
			float unitB;
			float unitC;
		};

		class Animation {
		private:
			static std::vector<pen::ui::AnimationItem> animationList;

		public:
			static void Add(pen::ui::Item* item, const unsigned int& type, const long& ms, const bool& infinite, const float& unitA, const float& unitB = 0, const float& unitC = 0);
			static void Run();
		private:
			static bool CheckStatus(const pen::ui::AnimationItem& item);
			static void Animate(pen::ui::AnimationItem item);
		};
	}
}