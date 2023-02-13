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
            bool (*onClickCallback)(pen::ui::Item*, int, int), pen::Vec4 objectColor, std::string objectTextureName,
            unsigned int row, unsigned int column) {
            /*Bitmap font objects constructor*/
            data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectColor);
            id = objectId;
            SetPosition(objectPositions);
            SetSize(objectSize);

            shapeType = pen::ui::Shape::QUAD;
            parent = objectParent;
            userOnClickCallback = onClickCallback;

            textureName = objectTextureName;

            isUI = true;
            isText = true;

            charRowIdx = row;
            charColumnIdx = column;

            UpdateTextCharacter(pen::State::Get()->asciiMap.Find(textureName)->second);

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

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

        void TextCharacterItem::UpdateTextCharacter(unsigned char characterIndex) {
            /*Replaces the text character*/
            std::vector<float> texCoordVector = pen::ui::Shape::UpdateBitmapFontPosition(characterIndex);
            pen::Animate(data, "fonts/bitmap.png", texCoordVector[0], texCoordVector[1], texCoordVector[2], texCoordVector[3]);
        }
	}
}