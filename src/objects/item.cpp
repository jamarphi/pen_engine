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
#include "item.h"

namespace pen {
    namespace ui {
        std::vector<MtlData*>* mtlList = new std::vector<MtlData*>();

        Item::Item() {}

        Item::Item(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor, pen::ui::Item* objectParent, bool (*onClickCallback)(Item*, int, int),
            std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            /*Regular item constructor*/
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectColor;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectColor);
            }
            id = objectId;
            SetPosition(objectPositions);
            SetSize(objectSize);
            isUI = true;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

        Item::Item(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, unsigned int objectShapeType, pen::Vec4 objectColor,
           pen::ui::Item* objectParent, bool (*onClickCallback)(Item*, int, int), bool objectIsBackground, std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            /*Pixel buffer and background quad constructor*/
            if (objectIsBackground) {
                id = objectId;
                SetPosition(objectPositions);
                SetSize(objectSize);
                parent = objectParent;
                userOnClickCallback = onClickCallback;
                isBackground = objectIsBackground;

                isUI = true;
                shapeType = objectShapeType;
                color = objectColor;

                textureName = objectTextureName;

                texCoordStartX = itemTexCoordStartX;
                texCoordStartY = itemTexCoordStartY;
                texCoordEndX = itemTexCoordEndX;
                texCoordEndY = itemTexCoordEndY;

                bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, objectShapeType, objectColor, nullptr, GetAssetId(),
                    itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

                /*Checks to make sure the item is on the screen to be rendered*/
                CheckActiveStatus();
            }
        }

        Item::~Item() {
            if (id != PIXEL_DRAWING_ID) {
                /*Deletes the GUI pixel item*/
                pen::DeleteItem(data);
                data = nullptr;
            }
        }

        void Item::Push(Item* item) {
            /*Adds child items to be rendered after item*/
            childItems.push_back(item);
            if(item->isBackground) itemCount += item->itemCount + 1;
        }

        void Item::Pop() {
            /*Removes the most recent child item*/
            if (childItems.size() > 0) {
                itemCount -= childItems[childItems.size() - 1]->itemCount - 1;
               pen::ui::Item* tempItem = childItems[childItems.size() - 1];
                childItems.pop_back();
                RemoveItem(tempItem);
            }
        }

        void Item::Draw() {
            /*Draws the data to the pixel buffer*/
            data->Draw();
        }

        void Item::UpdateDisplayFunction(float (*displayFunction)(int, int, float)) {
            /*Updates the display function*/
            data->displayFunction = displayFunction;
        }

       pen::ui::Item* Item::FindItem(uint32_t id) {
            /*Use recursion until an item with this id is found*/
           pen::ui::Item* tempItem = nullptr;
            if (this->id == id) return this;
            for (int i = 0; i < childItems.size(); i++) {
                if (childItems[i]->childItems.size() > 0) {
                    tempItem = childItems[i]->FindItem(id);
                    if (tempItem != nullptr) return tempItem;
                }
                if (childItems[i]->id == id) return childItems[i];
            }
            return tempItem;
        }

        void Item::RemoveItemById(uint32_t id) {
            /*Remove all items and nested items with this id*/
            for (int i = 0; i < childItems.size(); i++) {
                if (childItems[i]->childItems.size() > 0) {
                    childItems[i]->RemoveItemById(id);
                }

                if (childItems[i]->id == id) {
                    std::vector<pen::ui::Item*> tempChildItems;
                    pen::ui::Item* itemToDelete = nullptr;
                    for (int j = 0; j < childItems.size(); j++) {
                        if (childItems[j]->id != id) {
                            tempChildItems.push_back(childItems[j]);
                        }
                        else {
                            itemToDelete = childItems[j];
                        }
                    }
                    childItems = tempChildItems;
                    RemoveNestedItems(itemToDelete);
                    if (itemToDelete != nullptr) delete itemToDelete;
                }
            }
        }

        bool Item::RemoveItem(Item* id) {
            /*Remove specific item by its pointer*/
            bool removed = false;
            for (int i = 0; i < childItems.size(); i++) {
                if (childItems[i] == id) {
                    std::vector<pen::ui::Item*> tempChildItems;
                    pen::ui::Item* itemToDelete = nullptr;
                    for (int j = 0; j < childItems.size(); j++) {
                        if (childItems[j] != id) {
                            tempChildItems.push_back(childItems[j]);
                        }
                        else {
                            itemToDelete = childItems[j];
                        }
                    }
                    childItems = tempChildItems;
                    RemoveNestedItems(itemToDelete);
                    if (itemToDelete != nullptr) delete itemToDelete;
                    return true;
                }

                if (childItems[i]->childItems.size() > 0) {
                   pen::ui::Item* tempItem = childItems[i];
                    removed = childItems[i]->RemoveItem(id);
                    if (removed) return true;
                }
            }
            return false;
        }

        void Item::RemoveNestedItems(Item* id) {
            /*Remove all nested items from a given item*/
            if (id->childItems.size() > 0) {
                for (int i = 0; i < id->childItems.size(); i++) {
                    if (id->childItems[i]->childItems.size() > 0) RemoveNestedItems(id->childItems[i]);
                    delete id->childItems[i];
                }
            }
        }

        void Item::CombineChildBuffers() {
            /*Combines all of the child item buffers after the buffer for this object*/

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();

            if (isActive && forceActive) {                
                /*Draws the gui objects to the pixel buffer in the order of parent items first then child items*/
                if (!isBackground) {
                    if (textureName != "") isText ? UpdateTextCharacter(pen::State::Get()->asciiMap.Find(textureName)->second)
                        : pen::Animate(data, textureName, texCoordStartX, texCoordStartY, texCoordEndX, texCoordEndY);
                    
                    this->Draw();
                }

                for (int i = 0; i < childItems.size(); i++) {
                    if (!childItems[i]->isActive || !childItems[i]->forceActive) continue;
                    childItems[i]->CombineChildBuffers();

                    if (isBackground) {
                        /*Adds the buffers to the parent item*/
                        for (int j = 0; j < childItems[i]->bufferPositions.size(); j++) {
                            bufferPositions.push_back(childItems[i]->bufferPositions[j]);
                        }
                    }
                }
            }
        }

        int Item::GetAssetId() {
            /*Grabs the asset id from the Asset class for a specific texture name*/
            if (textureName.length() > 0) {
                uint16_t tempId = 0;
                for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
                    if (pen::Asset::assetMap.items[i].second.name == textureName) {
                        tempId = pen::Asset::assetMap.items[i].second.id;
                        break;
                    }
                }
                return (float)(tempId % State::Get()->textureUnits);
            }
            else {
                /*If there is no texture name, the item will be a solid color*/
                return 0.0f;
            }
        }

        void Item::CheckActiveStatus() {
            /*Updates isActive flag if object needs to be rendered*/
            pen::State* inst = pen::State::Get();
            if (!forceActive) {
                /*User wants the object to not be rendered*/
                isActive = false;
            }
            else {
                if (isListItem) {
                    isActive = true;
                    if (isText) isActive = (positions.x > parent->parent->positions.x + parent->parent->size.x - 5.0f) ? false : true;
                }
                else {
                    if (positions.z > 1000.0f || positions.z < -1.0f) {
                        /*Item is too far into or outside the view of the screen to be rendered*/
                        isActive = false;
                    }
                    else {
                        if (positions.x >= pen::PixelBufferWidth() || positions.y >= pen::PixelBufferHeight()) {
                            /*Item is to the right or above the view of the screen*/
                            isActive = false;
                        }
                        else {
                            if (positions.x + size.x <= 0 || positions.y + size.y <= 0) {
                                /*Item is to the left or below the view of the screen*/
                                isActive = false;
                            }
                            else {
                                /*Item is within view*/
                                isActive = true;
                            }
                        }
                    }
                }
            }
        }

        void Item::UpdateChildOffsets(const pen::Vec2& scaling) {
            /*Go through and change the distance between child items based on the scaling of their container*/
            if (childItems.size() > 1) {
                float initialPosX = 0.0f;
                float initialPosY = 0.0f;
                float previousObjectX = 0.0f;
                float previousObjectY = 0.0f;
                float distanceX = 0.0f;
                float distanceY = 0.0f;
                float newDistanceX = 0.0f;
                float newDistanceY = 0.0f;

                for (int i = 0; i < childItems.size(); i++) {
                    initialPosX = (i == 0) ? data->x : previousObjectX;
                    initialPosY = (i == 0) ? data->y : previousObjectY;
                    distanceX = childItems[i]->data->x - initialPosX;
                    distanceY = childItems[i]->data->y - initialPosY;
                    newDistanceX = distanceX * scaling.x;
                    newDistanceY = distanceY * scaling.y;
                    previousObjectX = childItems[i]->data->x;
                    previousObjectY = childItems[i]->data->y;

                    /*Update x distance*/
                    childItems[i]->data->x = (i == 0 ? initialPosX : childItems[i - 1]->data->x) + newDistanceX;

                    /*Update y distance*/
                    childItems[i]->data->y = (i == 0 ? initialPosY : childItems[i - 1]->data->y) + newDistanceY;
                }
            }
        }

        bool Item::OnClick(Item* item, const int& button, const int& action) {
            /*Handles a click event based on the user's defined callback function*/
            return (userOnClickCallback != nullptr ? (*userOnClickCallback)(this, button, action) : false);
        }

        bool Item::OnDrag(Item* item, double* xPos, double* yPos) {
            /*Handles a drag event based on the user's defined drag function*/
            return (userOnDragCallback != nullptr ? (*userOnDragCallback)(this, xPos, yPos) : false);
        }

        bool Item::OnKey(Item* item, const int& key, const int& action) {
            /*Handles a key event based on the user's defined callback function*/
            return (userOnKeyCallback != nullptr ? (*userOnKeyCallback)(this, key, action) : false);
        }

        pen::Vec3* Item::GetPosition() {
            return &positions;
        }

        void Item::SetPosition(pen::Vec3 objectPos) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            if (data != nullptr) {
                data->x = objectPos.x;
                data->y = objectPos.y;
            }
            positions = objectPos;
        }

        pen::Vec2* Item::GetSize() {
            return &size;
        }

        void Item::SetSize(pen::Vec2 objectSize) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            if(data != nullptr && size.x > 0.0f && size.y > 0.0f) pen::Scale(data, objectSize.x / size.x, objectSize.y / size.y);
            size.x = objectSize.x;
            size.y = objectSize.y;
        }

        pen::Vec4* Item::GetColor() {
            if (data == nullptr) return &color;
            return &data->color;
        }

        void Item::SetColor(pen::Vec4 objectColor) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            if(data != nullptr) data->color = objectColor;
        }

        bool Item::GetActiveStatus() {
            return forceActive;
        }

        void Item::AllowActive(bool status) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            forceActive = status;
        }

        void Item::UpdateTexture(const std::string& texture, float itemTexCoordStartX, float itemTexCoordStartY,
            float itemTexCoordEndX, float itemTexCoordEndY) {
            /*Updates the texture of the item*/
            textureName = texture;
            if (itemTexCoordStartX != 0.0f) texCoordStartX = itemTexCoordStartX;
            if (itemTexCoordStartY != 0.0f) texCoordStartY = itemTexCoordStartY;
            if (itemTexCoordEndX != 1.0f) texCoordEndX = itemTexCoordEndX;
            if (itemTexCoordEndY != 0.0f) texCoordEndY = itemTexCoordEndY;
            if(data != nullptr) pen::Animate(data, texture, itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
        }
    }
}
