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
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectAccentColor;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectAccentColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(objectPositions);
            SetSize(objectSize);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isClickable = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            /*Initialize the title*/
            Push(new pen::ui::TextBox(ID_ANY, objectTitle, pen::Vec3(GetPosition()->x, GetPosition()->y + GetSize()->y - (0.1f * GetSize()->y), 0.0f),
                objectTitle.size(), 0.1f * GetSize()->y, objectAccentColor, objectColor, nullptr, nullptr));
            childItems[0]->SetSize(pen::Vec2(GetSize()->x, childItems[0]->GetSize()->y));

            Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x + (0.03f * GetSize()->x), GetPosition()->y + (0.03f * GetSize()->y), GetPosition()->z),
                pen::Vec2(0.94f * GetSize()->x, GetSize()->y - childItems[0]->GetSize()->y - (0.03f * GetSize()->y)), objectColor, this, onClickCallback));
            container = childItems[1];

            if (childItems[0]->childItems.size() > 0) {
                if (childItems[0]->childItems[0]->GetPosition()->y < childItems[0]->GetPosition()->y) {
                    /*Hide the title*/
                    for (int i = 0; i < childItems[0]->childItems.size(); i++) childItems[0]->childItems[i]->AllowActive(false);
                }
            }

            CheckActiveStatus();
        }

        void ContextWindow::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item*/
            if (childItems.size() > 1) {
                container->childItems.push_back(item);
            }
            else {
                /*For the title and container window*/
                childItems.push_back(item);
            }
        }

        void ContextWindow::Pop() {
            /*Removes the most recent child item*/
            if (container->childItems.size() > 0) {
                pen::ui::Item* tempItem = container->childItems[container->childItems.size() - 1];
                container->childItems.pop_back();
                container->RemoveItem(tempItem);
            }
        }
	}
}
