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
#include "button.h"
#include "div.h"

namespace pen {
	namespace ui {
		class KeyBoard : public pen::ui::Item {
		private:
			void (*userOnKeyReturn)(char key);
		public:
			KeyBoard();
			KeyBoard(uint32_t objectId, void (*onKeyReturn)(char key), pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
				pen::Vec4 objectAccentColor = pen::Vec4(0.8f, 0.8f, 0.8f, 1.0f), pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(pen::ui::Item*, int, int) = nullptr, std::string objectTextureName = "", float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f,
				float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);

			void KeyBoardReturnKey(pen::ui::Item* item);
		private:
			void InitializeKeys(pen::Vec4 accentColor);
		};
	}
}