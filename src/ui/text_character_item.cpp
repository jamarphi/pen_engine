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
#include "text_character_item.h"

namespace pen {
	namespace ui {
        TextCharacterItem::TextCharacterItem() {}

        TextCharacterItem::TextCharacterItem(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::ui::Item* objectParent, 
            bool (*onClickCallback)(pen::ui::Item*, int, int), pen::Vec4 objectColor, bool objectIsFixed, std::string objectTextureName,
            unsigned int row, unsigned int column) {
            /*Bitmap font objects constructor*/
            id = objectId;
            positions = objectPositions;
            size = objectSize;
            isFixed = objectIsFixed;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);

            shapeType = pen::ui::Shape::QUAD;
            color = objectColor;
            parent = objectParent;
            userOnClickCallback = onClickCallback;

            textureName = objectTextureName;

            isUI = true;
            isText = true;

            charRowIdx = row;
            charColumnIdx = column;

            bufferPositions = pen::ui::Shape::BitmapFontPosition(positions, size, color, pen::State::Get()->asciiMap.Find(textureName)->second, nullptr, 0.0f, 0.0f, 0.0f);

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

        TextCharacterItem::~TextCharacterItem() {}

        bool TextCharacterItem::OnClick(pen::ui::Item* item, const int& button, const int& action) {
            if (userOnClickCallback != nullptr) {
                return (*userOnClickCallback)(this, button, action);
            }
            else {
                /*Update the text cursor position of the text editor item*/
                if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
                    /*This sets the draggable for the text editor component*/
                    if (item->parent != nullptr) {
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->parent != nullptr && item->parent->parent->parent->isTextEditor) {
                                pen::State::SetDraggable((void*)item->parent->parent->parent);
                                pen::State::SetKeyable((void*)item->parent->parent->parent);
                                item->parent->parent->parent->UpdateTextCursor(item);
                            }
                        }
                    }
                }
                else if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::RELEASED) {
                    /*This sets the highlight active state to falsefor the text editor component*/
                    if (item->parent != nullptr) {
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->parent != nullptr && item->parent->parent->parent->isTextEditor) {
                                item->parent->parent->parent->textEditorHighlightActive = false;
                            }
                        }
                    }
                }
                return true;
            }
        }
	}
}