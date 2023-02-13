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
#include "vertical_list.h"

namespace pen {
    namespace ui {
        VerticalList::VerticalList() { isUI = true; }

        VerticalList::VerticalList(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
            pen::Vec4 objectAccentColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTitle, std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
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
            isList = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            float* offset = new float[1];
            sliderOffset = offset;

            shapeType = pen::ui::Shape::QUAD;

            /*Initialize the list title and scroll bar*/
            Push(new pen::ui::TextBox(ID_ANY, objectTitle, pen::Vec3(GetPosition()->x, GetPosition()->y + GetSize()->y - (0.1f * GetSize()->y), 0.0f),
                objectTitle.size(), 0.1f * GetSize()->y, objectAccentColor, objectColor, nullptr, nullptr));
            childItems[0]->SetSize(pen::Vec2(GetSize()->x, childItems[0]->GetSize()->y));

            Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(GetPosition()->x + GetSize()->x - (0.05f * GetSize()->x), GetPosition()->y + GetSize()->y - (0.1f * GetSize()->y) - (0.9f * GetSize()->y), GetPosition()->z),
                pen::Vec2(0.05f * GetSize()->x, 0.9f * GetSize()->y), objectAccentColor, objectColor, 0, 1, this, nullptr));
            childItems[1]->AllowActive(false);

            if (childItems[0]->childItems.size() > 0) {
                if (childItems[0]->childItems[0]->GetPosition()->y < childItems[0]->GetPosition()->y) {
                    /*Hide the title*/
                    for (int i = 0; i < childItems[0]->childItems.size(); i++) childItems[0]->childItems[i]->AllowActive(false);
                }
            }
            
            CheckActiveStatus();
        }

        void VerticalList::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item, automatically updates child item positions to fit the nav bar*/
            if (childItems.size() >= 2) {
                std::vector<pen::ui::Item*> tempList;
                tempList.push_back(childItems[0]);
                for (int i = 1; i < childItems.size() - 1; i++) {
                    tempList.push_back(childItems[i]);
                }
                item->isListItem = true;
                item->parent = this;
                tempList.push_back(item);
                tempList.push_back(childItems[childItems.size() - 1]);
                childItems = tempList;
                UpdateItemPositions();
            }
            else {
                childItems.push_back(item);
            }
        }

        void VerticalList::Pop() {
            /*Removes the most recent child item*/
            if (childItems.size() > 2) {
                std::vector<pen::ui::Item*> tempList;
               pen::ui::Item* tempItem = childItems[childItems.size() - 2];
                for (int i = 0; i < childItems.size(); i++) {
                    if (childItems[i] != tempItem) {
                        tempList.push_back(childItems[i]);
                    }
                }
                RemoveItem(tempItem);
                childItems = tempList;
                UpdateItemPositions();
            }
        }

        void VerticalList::RemoveListItem(unsigned int idx) {
            /*Remove a list item by index*/
            idx += 1;
            if (idx > 0 && idx < childItems.size() - 1 && childItems.size() > 2) {
                std::vector<pen::ui::Item*> tempList;
               pen::ui::Item* tempItem = childItems[idx];
                for (int i = 0; i < childItems.size(); i++) {
                    if (childItems[i] != tempItem) {
                        tempList.push_back(childItems[i]);
                    }
                }
                RemoveItem(tempItem);
                childItems = tempList;
                UpdateItemPositions();
            }
        }

        void VerticalList::ScrollBarUpdateItemPositions() {
            UpdateItemPositions(false);
        }

        void VerticalList::UpdateItemPositions(bool reset) {
            /*Updates the child item positions to fit the vertical list*/
            if(reset) childItems[childItems.size() - 1]->heightOfScrollParentItems = 0.0f;

            for (int i = 1; i < childItems.size() - 1; i++) {
                pen::Vec2 translation;
                if (i == 1) {
                    /*For first visible added item*/
                    translation.x = (childItems[i]->GetPosition()->x - GetPosition()->x) * -1.0f;                    
                    translation.y = (childItems[i]->GetPosition()->y - (childItems[0]->GetPosition()->y - childItems[i]->GetSize()->y)) * -1.0f;

                    /*Translate all nested objects*/
                    pen::ui::Translate(childItems[i], translation, true);

                    childItems[i]->SetPosition(pen::Vec3(GetPosition()->x, childItems[0]->GetPosition()->y - childItems[i]->GetSize()->y, 0.0f));

                    UpdateChildScaling(childItems[i]);
                }
                else {
                    /*Go through the rest of the child items*/
                    translation.x = (childItems[i]->GetPosition()->x - GetPosition()->x) * -1.0f;
                    translation.y = (childItems[i]->GetPosition()->y - (childItems[i - 1]->GetPosition()->y - childItems[i]->GetSize()->y)) * -1.0f;

                    /*Translate all nested objects*/
                    pen::ui::Translate(childItems[i], translation, true);
                    
                    childItems[i]->SetPosition(pen::Vec3(GetPosition()->x, childItems[i - 1]->GetPosition()->y - childItems[i]->GetSize()->y, 0.0f));

                    UpdateChildScaling(childItems[i]);
                }
            }
            
            /*Update the scroll bar size*/
            childItems[childItems.size() - 1]->childItems[2]->SetSize(pen::Vec2(childItems[childItems.size() - 1]->childItems[2]->GetSize()->x,
                (childItems[childItems.size() - 1]->GetSize()->y
                    - childItems[childItems.size() - 1]->childItems[0]->GetSize()->y - childItems[childItems.size() - 1]->childItems[1]->GetSize()->y)
                / (childItems.size() - 2)));
            /*Update the scroll bar position*/
            childItems[childItems.size() - 1]->childItems[2]->SetPosition(pen::Vec3(childItems[childItems.size() - 1]->childItems[2]->GetPosition()->x,
                childItems[childItems.size() - 1]->GetPosition()->y + childItems[childItems.size() - 1]->GetSize()->y
                - childItems[childItems.size() - 1]->childItems[0]->GetSize()->y - childItems[childItems.size() - 1]->childItems[2]->GetSize()->y,
                0.0f));

            /*Reset item positions in vertical list*/
            if (reset) {
                for (int i = 1; i < childItems.size() - 1; i++) {
                    if (childItems[i]->GetPosition()->y >= GetPosition()->y &&
                        (childItems[i]->GetPosition()->y + childItems[i]->GetSize()->y <= GetPosition()->y + GetSize()->y - childItems[0]->GetSize()->y)) {
                        childItems[i]->AllowActive(true);
                    }
                    else {
                        childItems[i]->AllowActive(false);
                    }

                    /*Updates the scroll bar height to translate vertical list items*/
                    childItems[childItems.size() - 1]->heightOfScrollParentItems += childItems[i]->GetSize()->y;
                }

                if (childItems[childItems.size() - 1]->heightOfScrollParentItems > GetSize()->y - childItems[0]->GetSize()->y) {
                    /*Shows scroll bar*/
                    childItems[childItems.size() - 1]->AllowActive(true);
                }
                else {
                    /*Hides scroll bar*/
                    childItems[childItems.size() - 1]->AllowActive(false);
                }
            }

            /*Make sure the first item is showing*/
            if (!reset) childItems[1]->AllowActive(true);
        }

        void VerticalList::UpdateChildScaling(pen::ui::Item* item) {
            /*The aspect ratio is saved for any non-ui objects*/
            float tempFactor = item->GetSize()->x / GetSize()->x > 0.75f ? item->GetSize()->x / GetSize()->x : 0.75f;
            pen::ui::Scale(item, pen::Vec2(tempFactor, tempFactor), true);
            item->GetSize()->x = GetSize()->x;
        }
    }
}
