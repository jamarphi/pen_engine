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
            float positionY = pen::PixelBufferHeight() - height;
            if (objectTextureName != "") {
                data = pen::CreateSprite(0, positionY, pen::PixelBufferWidth(), height, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectColor;
            }
            else {
                data = pen::DrawRect(0, positionY, pen::PixelBufferWidth(), height, objectColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(pen::Vec3(0.0f, positionY, 0.0f));
            SetSize(pen::Vec2(pen::PixelBufferWidth(), height));
            if (GetSize()->y >= inst->textScaling) SetSize(pen::Vec2(GetSize()->x, inst->textScaling - 0.1f));
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isClickable = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            CheckActiveStatus();
        }

        void NavBar::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item, automatically updates child item positions to fit the nav bar*/
            childItems.push_back(item);
            UpdateItemPositions();
        }

        void NavBar::UpdateItemPositions() {
            /*Updates the child item positions to fit the nav bar*/
            pen::State* inst = pen::State::Get();
            for (int i = 0; i < childItems.size(); i++) {
                if (i == 0) {
                    /*Set first button relative to the nav bar*/
                    childItems[i]->SetPosition(*GetPosition());

                    /*This happens in order to update the positions of the text characters*/
                    childItems[i]->UpdateText(childItems[i]->origText);

                    pen::ui::Scale(childItems[i], pen::Vec2(GetSize()->y / childItems[i]->GetSize()->y, GetSize()->y / childItems[i]->GetSize()->y));

                    /*Updates the text to be level with its container*/
                    for (int j = 0; j < childItems[i]->childItems.size(); j++) {
                        childItems[i]->childItems[j]->SetPosition(pen::Vec3(childItems[i]->childItems[j]->GetPosition()->x, childItems[i]->GetPosition()->y, 0.0f));
                    }
                }
                else {
                    /*Set the rest of the buttons relative to the previous ones*/
                    childItems[i]->SetPosition(pen::Vec3(childItems[i - 1]->GetPosition()->x + childItems[i - 1]->GetSize()->x, GetPosition()->y, 0.0f));

                    /*This happens in order to update the positions of the text characters*/
                    childItems[i]->UpdateText(childItems[i]->origText);

                    pen::ui::Scale(childItems[i], pen::Vec2(GetSize()->y / childItems[i]->GetSize()->y, GetSize()->y / childItems[i]->GetSize()->y));

                    /*Updates the text to be level with its container*/
                    for (int k = 0; k < childItems[i]->childItems.size(); k++) {
                        childItems[i]->childItems[k]->SetPosition(pen::Vec3(childItems[i]->childItems[k]->GetPosition()->x, childItems[i]->GetPosition()->y, 0.0f));
                    }
                }
            }
        }
    }
}
