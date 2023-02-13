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
#include "div.h"

namespace pen {
    namespace ui {
        Div::Div() { isUI = true; }

        Div::Div(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor, unsigned int objectOrientation,
            pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY,
            float itemTexCoordEndX, float itemTexCoordEndY) {
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectColor;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(objectPositions);
            SetSize(objectSize);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isClickable = true;
            orientation = objectOrientation;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            CheckActiveStatus();
        }

        void Div::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item*/
            childItems.push_back(item);
            UpdateContents();
        }

        void Div::Pop(int idx) {
            /*Removes the most recent child item or at a specified index*/
            if (childItems.size() > 0) {
                if (idx == -1) {
                    pen::ui::Item* tempItem = childItems[childItems.size() - 1];
                    childItems.pop_back();
                    RemoveItem(tempItem);
                }
                else {
                    pen::ui::Item* tempItem = childItems[idx];
                    std::vector<pen::ui::Item*> tempList = {};
                    for (int i = 0; i < childItems.size(); i++) {
                        if (i != idx) {
                            tempList.push_back(childItems[i]);
                        }
                    }
                    RemoveItem(tempItem);
                    childItems = tempList;
                }
                UpdateContents();
            }
        }

        void Div::UpdateContents() {
            /*Updates alignment for items inside div*/
            if (childItems.size() > 0) {
                float paddingLength = 0.0074f * pen::State::Get()->screenHeight;
                /*Integer division by 2 rounding down to whole number for childItems before doubling to get the total size*/
                float totalPaddingLength = paddingLength * (childItems.size() / 2) * 2.0f;
                if (orientation == 0) {
                    /*Div is horizontal*/
                    float itemWidth = (GetSize()->x - totalPaddingLength) / (float)childItems.size();
                    int tallestItemIdx = 0;
                    float maxHeight = 0.0f;

                    for (int i = 0; i < childItems.size(); i++) {
                        if (childItems[i]->GetSize()->y > maxHeight) {
                            tallestItemIdx = i;
                            maxHeight = childItems[i]->GetSize()->y;
                        }
                    }
                    GetSize()->y = maxHeight;
                    SetSize(pen::Vec2(GetSize()->x, maxHeight));
                    pen::ui::Scale(this, pen::Vec2(1.0f, itemWidth / childItems[tallestItemIdx]->GetSize()->x), true);

                    for (int j = 0; j < childItems.size(); j++) {
                        pen::ui::Scale(childItems[j], pen::Vec2(itemWidth / childItems[j]->GetSize()->x, itemWidth / childItems[j]->GetSize()->x));
                        float offsetDis = (float)j * (itemWidth + paddingLength);
                        pen::ui::Translate(childItems[j], pen::Vec2((GetPosition()->x + offsetDis) - childItems[j]->GetPosition()->x, (GetPosition()->y + (GetSize()->y / 2.0f)) - (childItems[j]->GetPosition()->y + (childItems[j]->GetSize()->y / 2.0f))));
                    }
                }
                else {
                    /*Div is vertical*/
                    float itemHeight = (GetSize()->y - totalPaddingLength) / childItems.size();
                    int longestItemIdx = 0;
                    float maxWidth = 0.0f;

                    for (int i = 0; i < childItems.size(); i++) {
                        if (childItems[i]->GetSize()->x > maxWidth) {
                            longestItemIdx = i;
                            maxWidth = childItems[i]->GetSize()->x;
                        }
                    }
                    SetSize(pen::Vec2(maxWidth, GetSize()->y));
                    pen::ui::Scale(this, pen::Vec2(itemHeight / childItems[longestItemIdx]->GetSize()->y, 1.0f), true);

                    for (int j = 0; j < childItems.size(); j++) {
                        pen::ui::Scale(childItems[j], pen::Vec2(itemHeight / childItems[j]->GetSize()->y, itemHeight / childItems[j]->GetSize()->y));
                        float offsetDis = ((GetPosition()->y + GetSize()->y) - ((float)j * (itemHeight + paddingLength)) - itemHeight) - GetPosition()->y;
                        pen::ui::Translate(childItems[j], pen::Vec2((GetPosition()->x + (GetSize()->x / 2.0f)) - (childItems[j]->GetPosition()->x + (childItems[j]->GetSize()->x / 2.0f)), (GetPosition()->y + offsetDis) - childItems[j]->GetPosition()->y));
                    }
                }
            }
        }
    }
}
