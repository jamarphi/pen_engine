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
            isList = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            float* offset = new float[1];
            sliderOffset = offset;

            bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = objectColor;

            /*Initialize the list title and scroll bar*/
            Push(new pen::ui::TextBox(ID_ANY, objectTitle, pen::Vec3(positions.x, positions.y + size.y - (0.1f * size.y), 0.0f),
                objectTitle.size(), 0.1f * size.y, objectAccentColor, objectColor, nullptr, nullptr, true));
            childItems[0]->size.x = size.x;

            Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(positions.x + size.x - (0.05f * size.x), positions.y + size.y - (0.1f * size.y) - (0.9f * size.y), positions.z),
                pen::Vec2(0.05f * size.x, 0.9f * size.y), objectAccentColor, objectColor, 0, 1, this, nullptr));
            childItems[1]->AllowActive(false);

            if (childItems[0]->childItems.size() > 0) {
                if (childItems[0]->childItems[0]->positions.y < childItems[0]->positions.y) {
                    /*Hide the title*/
                    for (int i = 0; i < childItems[0]->childItems.size(); i++) childItems[0]->childItems[i]->AllowActive(false);
                }
            }
            
            CheckActiveStatus();
        }

        VerticalList::~VerticalList() {}

        void VerticalList::Push(pen::ui::Item* item) {
            /*Adds child items to be rendered after item, automatically updates child item positions to fit the nav bar*/
            if (shapeType == pen::ui::Shape::TRI || shapeType == pen::ui::Shape::QUAD) {
                if (item->shapeType != shapeType) {
                    std::cout << "You must use the same shape type for items that are children of this item" << std::endl;
                }
                else {
                    
                    itemCount += item->itemCount + 1;
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
            }
        }

        void VerticalList::Pop() {
            /*Removes the most recent child item*/
            if (childItems.size() > 2) {
                std::vector<pen::ui::Item*> tempList;
                itemCount -= childItems[childItems.size() - 2]->itemCount - 1;
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
                itemCount -= childItems[idx]->itemCount - 1;
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
                pen::Vec3 translation;
                if (i == 1) {
                    /*For first visible added item*/
                    translation.x = (childItems[i]->positions.x - positions.x) * -1.0f;                    
                    translation.y = (childItems[i]->positions.y - (childItems[0]->positions.y - childItems[i]->size.y)) * -1.0f;
                    translation.z = (childItems[i]->positions.z - positions.z) * -1.0f;

                    /*Translate all nested objects*/
                    pen::ui::Translate(childItems[i], translation, true);

                    childItems[i]->positions.x = positions.x;
                    childItems[i]->positions.y = childItems[0]->positions.y - childItems[i]->size.y;
                    childItems[i]->positions.z = positions.z;

                    UpdateChildScaling(childItems[i]);
                }
                else {
                    /*Go through the rest of the child items*/
                    translation.x = (childItems[i]->positions.x - positions.x) * -1.0f;
                    translation.y = (childItems[i]->positions.y - (childItems[i - 1]->positions.y - childItems[i]->size.y)) * -1.0f;
                    translation.z = (childItems[i]->positions.z - positions.z) * -1.0f;

                    /*Translate all nested objects*/
                    pen::ui::Translate(childItems[i], translation, true);
                    
                    childItems[i]->positions.x = positions.x;
                    childItems[i]->positions.y = childItems[i - 1]->positions.y - childItems[i]->size.y;
                    childItems[i]->positions.z = positions.z;

                    UpdateChildScaling(childItems[i]);
                }
            }

            /*Update the scroll bar size*/
            childItems[childItems.size() - 1]->childItems[2]->size.y = (childItems[childItems.size() - 1]->size.y 
                - childItems[childItems.size() - 1]->childItems[0]->size.y - childItems[childItems.size() - 1]->childItems[1]->size.y) 
                / (childItems.size() - 2);
            /*Update the scroll bar position*/
            childItems[childItems.size() - 1]->childItems[2]->positions.y = childItems[childItems.size() - 1]->positions.y + childItems[childItems.size() - 1]->size.y 
                - childItems[childItems.size() - 1]->childItems[0]->size.y - childItems[childItems.size() - 1]->childItems[2]->size.y;

            /*Reset item positions in vertical list*/
            if (reset) {
                for (int i = 1; i < childItems.size() - 1; i++) {
                    if (childItems[i]->positions.y >= positions.y &&
                        (childItems[i]->positions.y + childItems[i]->size.y <= positions.y + size.y - childItems[0]->size.y)) {
                        childItems[i]->AllowActive(true);
                    }
                    else {
                        childItems[i]->AllowActive(false);
                    }

                    /*Updates the scroll bar height to translate vertical list items*/
                    childItems[childItems.size() - 1]->heightOfScrollParentItems += childItems[i]->size.y;
                }

                if (childItems[childItems.size() - 1]->heightOfScrollParentItems > size.y - childItems[0]->size.y) {
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
            float tempFactor = item->size.x / size.x > 0.75f ? item->size.x / size.x : 0.75f;
            pen::ui::Scale(item, pen::Vec2(tempFactor, tempFactor), true);
            item->size.x = size.x;
        }
    }
}
