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
#include "context_window.h"

namespace pen {
	namespace ui {
        ContextWindow::ContextWindow() { isUI = true; }

        ContextWindow::ContextWindow(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
            pen::Vec4 objectAccentColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTitle, std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
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

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = objectAccentColor;

            /*Initialize the title*/
            Push(new pen::ui::TextBox(ID_ANY, objectTitle, pen::Vec3(positions.x, positions.y + size.y - (0.1f * size.y), 0.0f),
                objectTitle.size(), 0.1f * size.y, objectAccentColor, objectColor, nullptr, nullptr, true));
            childItems[0]->size.x = size.x;

            Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x + (0.03f * size.x), positions.y + (0.03f * size.y), positions.z),
                pen::Vec2(0.94f * size.x, size.y - childItems[0]->size.y - (0.03f * size.y)), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));
            container = childItems[1];

            if (childItems[0]->childItems.size() > 0) {
                if (childItems[0]->childItems[0]->positions.y < childItems[0]->positions.y) {
                    /*Hide the title*/
                    for (int i = 0; i < childItems[0]->childItems.size(); i++) childItems[0]->childItems[i]->AllowActive(false);
                }
            }

            CheckActiveStatus();
        }

        ContextWindow::~ContextWindow() {}

        void ContextWindow::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item*/
            if (shapeType == pen::ui::Shape::TRI || shapeType == pen::ui::Shape::QUAD) {
                if (item->shapeType != shapeType) {
                    std::cout << "You must use the same shape type for items that are children of this item" << std::endl;
                }
                else {
                    if (childItems.size() > 1) {
                        container->childItems.push_back(item);
                        container->itemCount += item->itemCount + 1;
                    }
                    else {
                        /*For the title and container window*/
                        childItems.push_back(item);
                        itemCount += item->itemCount + 1;
                    }
                }
            }
        }

        void ContextWindow::Pop() {
            /*Removes the most recent child item*/
            if (container->childItems.size() > 0) {
                container->itemCount -= container->childItems[container->childItems.size() - 1]->itemCount - 1;
                pen::ui::Item* tempItem = container->childItems[container->childItems.size() - 1];
                container->childItems.pop_back();
                container->RemoveItem(tempItem);
            }
        }
	}
}