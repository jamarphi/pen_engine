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
#include "ui_object.h"

namespace pen {
	namespace ui {
		class TextBox : public pen::ui::UIObject {
			/*The textbox is the most primitive ui element that contains text.  Other text-containing ui items include this class.*/
		public:
			TextBox();
			/*This is the regular constructor*/
			TextBox(uint32_t objectId, std::string userText, pen::Vec3 objectPositions, int objectTextLength, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), pen::Vec4 objectTextColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
				pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(pen::ui::Item*, int, int) = nullptr, bool objectIsFixed = true, std::string objectTextureName = "",
				float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f, bool textEditor = false);

			/*This is for an initial textbox height from the user*/
			TextBox(uint32_t objectId, std::string userText, pen::Vec3 objectPositions, int objectTextLength, float objectHeight, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), pen::Vec4 objectTextColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
				pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(pen::ui::Item*, int, int) = nullptr, bool objectIsFixed = true,std::string objectTextureName = "",
				float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f, bool textEditor = false);
			~TextBox();
			void SetTextLength();
			void SetTextColor(pen::Vec4 itemTextColor, int pos = -1);
			void UpdateText(const std::string& userText);
		};
	}
}