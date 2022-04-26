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
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            positions = objectPositions;
            size = objectSize;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = true;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = true;
            orientation = objectOrientation;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = objectColor;
            CheckActiveStatus();
        }

        Div::~Div() {}

        void Div::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item*/
            if (shapeType == pen::ui::Shape::TRI || shapeType == pen::ui::Shape::QUAD) {
                if (item->shapeType != shapeType) {
                    std::cout << "You must use the same shape type for items that are children of this item" << std::endl;
                }
                else {
                    childItems.push_back(item);
                    itemCount += item->itemCount + 1;
                    UpdateContents();
                }
            }
        }

        void Div::Pop(int idx) {
            /*Removes the most recent child item or at a specified index*/
            if (childItems.size() > 0) {
                if (idx == -1) {
                    itemCount -= childItems[childItems.size() - 1]->itemCount - 1;
                    pen::ui::Item* tempItem = childItems[childItems.size() - 1];
                    childItems.pop_back();
                    RemoveItem(tempItem);
                }
                else {
                    itemCount -= childItems[idx]->itemCount - 1;
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
                    float itemWidth = (size.x - totalPaddingLength) / (float)childItems.size();
                    int tallestItemIdx = 0;
                    float maxHeight = 0.0f;

                    for (int i = 0; i < childItems.size(); i++) {
                        if (childItems[i]->size.y > maxHeight) {
                            tallestItemIdx = i;
                            maxHeight = childItems[i]->size.y;
                        }
                    }
                    size.y = maxHeight;
                    pen::ui::Scale(this, pen::Vec2(1.0f, itemWidth / childItems[tallestItemIdx]->size.x), true, false);

                    for (int j = 0; j < childItems.size(); j++) {
                        pen::ui::Scale(childItems[j], pen::Vec2(itemWidth / childItems[j]->size.x, itemWidth / childItems[j]->size.x), true);
                        float offsetDis = (float)j * (itemWidth + paddingLength);
                        pen::ui::Translate(childItems[j], pen::Vec3((positions.x + offsetDis) - childItems[j]->positions.x, (positions.y + (size.y / 2.0f)) - (childItems[j]->positions.y + (childItems[j]->size.y / 2.0f)), 0.0f),true);
                    }
                }
                else {
                    /*Div is vertical*/
                    float itemHeight = (size.y - totalPaddingLength) / childItems.size();
                    int longestItemIdx = 0;
                    float maxWidth = 0.0f;

                    for (int i = 0; i < childItems.size(); i++) {
                        if (childItems[i]->size.x > maxWidth) {
                            longestItemIdx = i;
                            maxWidth = childItems[i]->size.x;
                        }
                    }
                    size.x = maxWidth;
                    pen::ui::Scale(this, pen::Vec2(itemHeight / childItems[longestItemIdx]->size.y, 1.0f), true, false);

                    for (int j = 0; j < childItems.size(); j++) {
                        pen::ui::Scale(childItems[j], pen::Vec2(itemHeight / childItems[j]->size.y, itemHeight / childItems[j]->size.y), true);
                        float offsetDis = ((positions.y + size.y) - ((float)j * (itemHeight + paddingLength)) - itemHeight) - positions.y;
                        pen::ui::Translate(childItems[j], pen::Vec3((positions.x + (size.x / 2.0f)) - (childItems[j]->positions.x + (childItems[j]->size.x / 2.0f)), (positions.y + offsetDis) - childItems[j]->positions.y, 0.0f),true);
                    }
                }
            }
        }
    }
}