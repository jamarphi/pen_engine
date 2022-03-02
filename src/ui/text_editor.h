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
#include "text_box.h"
#include "scroll_bar.h"

#define TEXT_EDITOR_LINE_LENGTH 100

namespace pen {
	namespace ui {
		class TextEditor : public pen::ui::Item {
		public:
			/*Window that contains the text box, text, and scroll bar*/
			pen::ui::Item* containerWindow;

			/*Every even child item in this text box will be a highlighting quad and every odd child item will be the text character*/
			pen::ui::Item* windowTextBox;
			pen::ui::Item* verticalScrollBar;
			pen::ui::Item* horizontalScrollBar;
			pen::ui::Item* textCursor;
			int cursorIdx = 0;

			bool textHighlighted = false;
		public:
			TextEditor();
			TextEditor(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
				pen::Vec4 objectAccentColor = pen::Vec4(0.8f, 0.8f, 0.8f, 1.0f), pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(pen::ui::Item*, int, int) = nullptr, bool (*onKeyCallback)(pen::ui::Item*, int, int) = nullptr, 
				std::string initialText = "", std::string objectTextureName = "", float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f,
				float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);
			~TextEditor();

			void UpdateTextCursor(pen::ui::Item* textCharacter);
			void ScrollBarUpdateItemPositions();
			bool OnClick(pen::ui::Item* item, const int& button, const int& action);
			bool OnDrag(pen::ui::Item* item, double* xPos, double* yPos);
			bool OnKey(pen::ui::Item* item, const int& key, const int& action);
			std::string ConvertChar(const int& character);
		};
	}
}