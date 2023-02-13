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

namespace pen {
	namespace ui {
		class TextCharacterItem : public pen::ui::Item {
		public:
			TextCharacterItem();
			TextCharacterItem(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::ui::Item* objectParent, 
				bool (*onClickCallback)(pen::ui::Item*, int, int), pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), 
				std::string objectTextureName = "", unsigned int row = 0, unsigned int column = 0);
			bool OnClick(pen::ui::Item* item, const int& button, const int& action);
			void UpdateTextCharacter(unsigned char characterIndex);
		};
	}
}