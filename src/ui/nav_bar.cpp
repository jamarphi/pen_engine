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
#include "nav_bar.h"

namespace pen {
    namespace ui {
        NavBar::NavBar() { isUI = true; }

        NavBar::NavBar(uint32_t objectId, float height, pen::Vec4 objectColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            positions = pen::Vec3(0.0f, inst->screenHeight - height, 0.0f);
            size = pen::Vec2(inst->screenWidth, height);
            if (size.y >= inst->textScaling) size.y = inst->textScaling - 0.1f;
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

            color = objectColor;

            CheckActiveStatus();
        }

        NavBar::~NavBar() {}

        void NavBar::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item, automatically updates child item positions to fit the nav bar*/
            if (shapeType == pen::ui::Shape::TRI || shapeType == pen::ui::Shape::QUAD) {
                if (item->shapeType != shapeType) {
                    std::cout << "You must use the same shape type for items that are children of this item" << std::endl;
                }
                else {
                    childItems.push_back(item);
                    itemCount += item->itemCount + 1;
                    UpdateItemPositions();
                }
            }
        }

        void NavBar::UpdateItemPositions() {
            /*Updates the child item positions to fit the nav bar*/
            pen::State* inst = pen::State::Get();
            for (int i = 0; i < childItems.size(); i++) {
                if (i == 0) {
                    /*Set first button relative to the nav bar*/
                    childItems[i]->positions.x = positions.x;
                    childItems[i]->positions.y = positions.y;
                    childItems[i]->positions.z = positions.z;

                    /*This happens in order to update the positions of the text characters*/
                    childItems[i]->UpdateText(childItems[i]->origText);

                    pen::ui::Scale(childItems[i], pen::Vec2(size.y / childItems[i]->size.y, size.y / childItems[i]->size.y));

                    /*Updates the text to be level with its container*/
                    for (int j = 0; j < childItems[i]->childItems.size(); j++) {
                        childItems[i]->childItems[j]->positions.y = childItems[i]->positions.y;
                    }
                }
                else {
                    /*Set the rest of the buttons relative to the previous ones*/
                    childItems[i]->positions.x = childItems[i - 1]->positions.x + childItems[i - 1]->size.x;
                    childItems[i]->positions.y = positions.y;
                    childItems[i]->positions.z = positions.z;

                    /*This happens in order to update the positions of the text characters*/
                    childItems[i]->UpdateText(childItems[i]->origText);

                    pen::ui::Scale(childItems[i], pen::Vec2(size.y / childItems[i]->size.y, size.y / childItems[i]->size.y));

                    /*Updates the text to be level with its container*/
                    for (int k = 0; k < childItems[i]->childItems.size(); k++) {
                        childItems[i]->childItems[k]->positions.y = childItems[i]->positions.y;
                    }
                }
            }
        }
    }
}