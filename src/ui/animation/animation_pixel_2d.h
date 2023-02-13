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
#include "../../state/pixel.h"
#include "../../renderer/render.h"

namespace pen {
		struct AnimationPixelItem {
			pen::Item* item;
			unsigned int type;
			bool infinite;
			int frames;
			bool ran;
			void (*onAnimationEnd)(pen::Item*, unsigned int);
			void (*customAnimationCallback)(pen::Item*);
			float unitA;
			float unitB;
			float unitC;
			float unitD;
		};

		class AnimationPixel {
		public:
		static std::vector<pen::AnimationPixelItem> animationList;

		public:
		static void Add(pen::Item* item, const unsigned int& type, const long& ms, const bool& infinite, void (*onAnimationEndEvent)(pen::Item*, unsigned int), void (*onCustomAnimationCallback)(pen::Item*), const float& unitA, const float& unitB = 0.0f, const float& unitC = 0.0f, const float& unitD = 0.0f);
		static bool CheckStatus(const pen::AnimationPixelItem& item);
		static void Animate(pen::AnimationPixelItem item);
		static void Run();
	};
}