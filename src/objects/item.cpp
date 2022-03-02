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
        Item::Item() {}

        Item::Item(uint32_t objectId, float* positions, unsigned int objectVertexCount, int* indices, unsigned int objectIndexCount, pen::Vec4 objectColor, bool objectIsFixed,
            bool objectIsWireFrame) {
            /*Complex shape item constructor*/
            id = objectId;
            isFixed = objectIsFixed;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            shapeType = pen::ui::Shape::COMPLEX;
            color = objectColor;
            isWireFrame = objectIsWireFrame;

            for (int i = 0; i < objectVertexCount; i++) {
                /*----Position data----*/
                bufferPositions.push_back(positions[(i * 3)]);
                bufferPositions.push_back(positions[(i * 3) + 1]);
                bufferPositions.push_back(positions[(i * 3) + 2]);
                /*----Position data----*/

                /*----Color data----*/
                bufferPositions.push_back(color.x);
                bufferPositions.push_back(color.y);
                bufferPositions.push_back(color.z);
                bufferPositions.push_back(color.w);
                /*----Color data----*/

                /*----Texture coordinates----*/
                bufferPositions.push_back(0.0f);
                bufferPositions.push_back(0.0f);
                /*----Texture coordinates----*/

                /*Texture asset id*/
                bufferPositions.push_back(0.0f);
            }

            /*Minus 1 in place since 1 is added when item is pushed to another item or layer*/
            itemCount = objectVertexCount - 1;

            for (int j = 0; j < objectIndexCount; j++) {
                complexIndices.push_back(indices[j]);
            }
            complexIndexCount = objectIndexCount;
            originalComplexIndexCount = objectIndexCount;

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

        Item::Item(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, unsigned int objectShapeType, pen::Vec4 objectColor,
           pen::ui::Item* objectParent, bool (*onClickCallback)(Item*, int, int), bool objectIsFixed, std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            /*Regular constructor for items*/
            id = objectId;
            positions = objectPositions;
            size = objectSize;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = objectIsFixed;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);

            shapeType = objectShapeType;
            color = objectColor;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, objectShapeType, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

        Item::~Item() {
            bufferPositions.clear();
            textureName = "";
        }

        void Item::Push(Item* item) {
            /*Adds child items to be rendered after item*/
            if (shapeType == pen::ui::Shape::TRI || shapeType == pen::ui::Shape::QUAD) {
                if (item->shapeType != shapeType) {
                    std::cout << "You must use the same shape type for items that are children of this item" << std::endl;
                }
                else {
                    childItems.push_back(item);
                    itemCount += item->itemCount + 1;
                }
            }
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

            complexIndexCount = originalComplexIndexCount;

            if (isActive && forceActive) {
                /*If this object uses a text character as the texture, it gets bufferPositions from the bitmap function since it uses certain sections of the bitmap*/
                if (shapeType != pen::ui::Shape::COMPLEX) bufferPositions = isText ? pen::ui::Shape::BitmapFontPosition(positions, size, color, pen::State::Get()->asciiMap.Find(textureName)->second, &bufferPositions[0], angles.x, angles.y, angles.z)
                    : pen::ui::Shape::GetBatchPosition(positions, size, shapeType, color, &bufferPositions[0], angles.x, angles.y, angles.z, GetAssetId(), texCoordStartX, texCoordStartY, texCoordEndX, texCoordEndY);

                for (int i = 0; i < childItems.size(); i++) {
                    if (!childItems[i]->isActive || !childItems[i]->forceActive) continue;
                    /*All of the child item buffers are first combined*/
                    childItems[i]->CombineChildBuffers();
                    complexIndexCount += childItems[i]->complexIndexCount;

                    /*Adds the buffers to the parent item*/
                    for (int j = 0; j < childItems[i]->bufferPositions.size(); j++) {
                        bufferPositions.push_back(childItems[i]->bufferPositions[j]);
                    }
                }
            }
        }

        int Item::GetAssetId() {
            /*Grabs the asset id from the Asset class for a specific texture name*/
            if (textureName.length() > 0) {
                uint16_t tempId = 0;
                for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
                    if (pen::Asset::assetMap.items[i].second.path == textureName) {
                        tempId = pen::Asset::assetMap.items[i].second.personalId;
                        break;
                    }
                }
                pen::State* inst = pen::State::Get();

                /*This gives us the asset grouping id based on the number of allowed texture slots for a given batch*/
                assetGroupingId = (float)(tempId / inst->textureUnits);

                return (float)(tempId % inst->textureUnits);
            }
            else {
                /*If there is no texture name, the item will be a solid color*/
                assetGroupingId = 0;
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
                if (shapeType != pen::ui::Shape::COMPLEX) {
                    if (isListItem) {
                        isActive = true;
                        if (isText) isActive = (positions.x > parent->parent->positions.x + parent->parent->size.x - 5.0f) ? false : true;
                    }
                    else {
                        if (positions.z > 1.0f || positions.z < -1.0f) {
                            /*Item is too far into or outside the view of the screen to be rendered*/
                            isActive = false;
                        }
                        else {
                            if (positions.x >= inst->screenWidth || positions.y >= inst->screenHeight) {
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
                else {
                    isActive = true;
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
                float angleX = 0.0f;
                float angleY = 0.0f;
                float angleZ = 0.0f;

                for (int i = 0; i < childItems.size(); i++) {
                    initialPosX = (i == 0) ? positions.x : previousObjectX;
                    initialPosY = (i == 0) ? positions.y : previousObjectY;
                    distanceX = childItems[i]->positions.x - initialPosX;
                    distanceY = childItems[i]->positions.y - initialPosY;
                    newDistanceX = distanceX * scaling.x;
                    newDistanceY = distanceY * scaling.y;
                    previousObjectX = childItems[i]->positions.x;
                    previousObjectY = childItems[i]->positions.y;

                    angleX = childItems[i]->angles.x;
                    angleY = childItems[i]->angles.y;
                    angleZ = childItems[i]->angles.z;

                    childItems[i]->angles.x = 0.0f;
                    childItems[i]->angles.y = 0.0f;
                    childItems[i]->angles.z = 0.0f;

                    /*Update x distance*/
                    childItems[i]->positions.x = (i == 0 ? initialPosX : childItems[i - 1]->positions.x) + newDistanceX;

                    /*Update y distance*/
                    childItems[i]->positions.y = (i == 0 ? initialPosY : childItems[i - 1]->positions.y) + newDistanceY;

                    childItems[i]->bufferPositions = childItems[i]->isText ? pen::ui::Shape::BitmapFontPosition(childItems[i]->positions, childItems[i]->size, childItems[i]->color, pen::State::Get()->asciiMap.Find(childItems[i]->textureName)->second, &childItems[i]->bufferPositions[0], childItems[i]->angles.x, childItems[i]->angles.y, childItems[i]->angles.z)
                        : pen::ui::Shape::GetBatchPosition(childItems[i]->positions, childItems[i]->size, childItems[i]->shapeType, childItems[i]->color, &childItems[i]->bufferPositions[0], childItems[i]->angles.x, childItems[i]->angles.y, childItems[i]->angles.z, childItems[i]->GetAssetId(), childItems[i]->texCoordStartX, childItems[i]->texCoordStartY, childItems[i]->texCoordEndX, childItems[i]->texCoordEndY);

                    if (angleX != 0.0f) childItems[i]->ItemRotate(angleX, 0, true);
                    if (angleY != 0.0f) childItems[i]->ItemRotate(angleY, 1, true);
                    if (angleZ != 0.0f) childItems[i]->ItemRotate(angleZ, 2, true);
                }
            }
        }

        pen::Vec2 Item::ItemGetPointOfOrigin(float* positions, const int& axis) {
            /*Grab point of origin for given angle*/
            switch (axis) {
            case 0:
                return pen::Vec2(positions[1] + size.y / 2.0f, positions[2]);
            case 1:
                return pen::Vec2(positions[0] + size.x / 2.0f, positions[2]);
            case 2:
                if (shapeType == 1) {
                    /*Line*/
                    return pen::Vec2(positions[0] + size.x / 2.0f, positions[1]);
                }
                else {
                    return pen::Vec2(positions[0] + size.x / 2.0f, positions[1] + size.y / 2.0f);
                }
            default:
                return pen::Vec2(0.0f, 0.0f);
            }
        }

        void Item::ItemRotate(const float& objectAngle, const int& axis, bool staticTransform, const pen::Vec2& objectPointOfOrigin, const bool& calculatePointOfOrigin) {
            /*Added specifically for UpdateChildOffsets, rotates an item either clockwise or counter-clockwise along the specified axis*/
            if ((objectAngle > 0.0f || objectAngle < 0.0f) && objectAngle < 2.0f * 3.14159f && objectAngle > -2.0f * 3.14159f) {
                float angle = objectAngle * (staticTransform ? 1.0f : pen::State::Get()->deltaTime);

                /*If angle is exactly pi offset it by a little bit*/
                if (angle == 3.14159f || angle == -3.14159f) angle -= 0.01f;

                /*It's offset the other way to get an obtuse angle if angle is greater than pi*/
                if (angle > 3.14159f) angle = -1.0f * (3.14159f - (angle - 3.14159f));
                if (angle < -3.14159f) angle = 3.14159f + (angle + 3.14159f);

                if (axis == 0) angles.x += angle;
                if (axis == 1) angles.y += angle;
                if (axis == 2) angles.z += angle;

                if (angles.x >= 2.0f * 3.14159f || angles.x <= -2.0f * 3.14159f) angles.x = 0.0f;
                if (angles.y >= 2.0f * 3.14159f || angles.y <= -2.0f * 3.14159f) angles.y = 0.0f;
                if (angles.z >= 2.0f * 3.14159f || angles.z <= -2.0f * 3.14159f) angles.z = 0.0f;

                /*Resets the buffer positions before rotating*/
                std::vector<float> tempPositions = (isText) ? pen::ui::Shape::BitmapFontPosition(positions, size, color, pen::State::Get()->asciiMap.Find(textureName)->second, &bufferPositions[0], 0.0f, 0.0f, 0.0f)
                    : pen::ui::Shape::GetBatchPosition(positions, size, shapeType, color, &bufferPositions[0], 0.0f, 0.0f, 0.0f, GetAssetId(), texCoordStartX, texCoordStartY, texCoordEndX, texCoordEndY);

                pen::Vec2 pointOfOrigin = calculatePointOfOrigin ? this->parent->ItemGetPointOfOrigin(&tempPositions[0], axis) : objectPointOfOrigin;

                int point1 = 0, point2 = 0;
                float chosenAngle = 0.0f;
                switch (axis) {
                case 0:
                    /*X axis*/
                    point1 = 1;
                    point2 = 2;
                    chosenAngle = angles.x;
                    break;
                case 1:
                    /*Y axis*/
                    point1 = 0;
                    point2 = 2;
                    chosenAngle = angles.y;
                    break;
                case 2:
                    /*Z axis*/
                    point1 = 0;
                    point2 = 1;
                    chosenAngle = angles.z;
                    break;
                default:
                    break;
                }

                if (shapeType != 5) {
                    /*Main 3D model items are meta items that have no actual data themselves*/
                    for (int i = 0; i < bufferPositions.size(); i += SHAPE_BATCH_VERTEX_SIZE) {
                        int xShift = tempPositions[i + point1] - pointOfOrigin.x;
                        int yShift = tempPositions[i + point2] - pointOfOrigin.y;

                        /*Calculate the rotated coordinates, has to be normalized to fit -1.0f to 1.0f z range*/
                        bufferPositions[i + point1] = (pointOfOrigin.x + (xShift * pen::op::Cos(chosenAngle) - yShift * pen::op::Sin(chosenAngle))) / (point1 == 2 ? 100.0f : 1.0f);
                        bufferPositions[i + point2] = (pointOfOrigin.y + (xShift * pen::op::Sin(chosenAngle) + yShift * pen::op::Cos(chosenAngle))) / (point2 == 2 ? 100.0f : 1.0f);
                    }
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

        pen::Vec3 Item::GetPosition() {
            return positions;
        }

        void Item::SetPosition(pen::Vec3 objectPos) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            positions = objectPos;
        }

        pen::Vec2 Item::GetSize() {
            return size;
        }

        void Item::SetSize(pen::Vec2 objectSize) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            size = objectSize;
        }

        pen::Vec4 Item::GetColor() {
            return color;
        }

        void Item::SetColor(pen::Vec4 objectColor) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            color = objectColor;
        }

        bool Item::GetActiveStatus() {
            return forceActive;
        }

        void Item::AllowActive(bool status) {
            /*When used directly, pen::ui::Submit() must be called afterwards to update*/
            forceActive = status;
        }
    }
}