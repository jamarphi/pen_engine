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
#include "item_3d.h"

namespace pen {
    Item3D::Item3D() {}

    Item3D::Item3D(bool childItem, pen::Vec3 objectPositions, unsigned int objectShapeType, pen::Vec4 objectColor,
        bool objectIsFixed, float* objectBufferPositions, const std::string& objectTexture) {
        /*3D child item constructor*/
        positions = objectPositions;
        size = pen::Vec2(50.0f, 50.0f);
        isFixed = objectIsFixed;
        angles = pen::Vec3(0.0f, 0.0f, 0.0f);
        isSingular = true;
        shapeType = objectShapeType;
        color = objectColor;
        model = pen::Mat4x4(1.0f);
        textureName = objectTexture;

        bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, pen::ui::Shape::OBJ_3D, objectColor, objectBufferPositions, 0.0f, 0.0f, 0.0f, GetAssetId());

        /*Checks to make sure the item is on the screen to be rendered*/
        CheckActiveStatus();
    }

#ifndef __PEN_MAC_IOS__
    Item3D::Item3D(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
        bool objectIsFixed, std::string objectTextureName,
        float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
        /*Regular constructor*/
        id = objectId;
        positions = objectPositions;
        /*Size does not matter for the main item*/
        size = objectSize;
        isFixed = objectIsFixed;
        angles = pen::Vec3(0.0f, 0.0f, 0.0f);
        isSingular = true;

        shapeType = pen::ui::Shape::OBJ_3D;
        color = objectColor;

        textureName = objectTextureName;

        texCoordStartX = itemTexCoordStartX;
        texCoordStartY = itemTexCoordStartY;
        texCoordEndX = itemTexCoordEndX;
        texCoordEndY = itemTexCoordEndY;

        int* tempIndices = new int[60000];
        indices3D = tempIndices;

        /*Checks to make sure the item is on the screen to be rendered*/
        CheckActiveStatus();
    }
#else
    Item3D::Item3D(int* layerIndices, uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
        bool objectIsFixed, std::string objectTextureName,
        float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
        /*Regular constructor*/
        id = objectId;
        positions = objectPositions;
        /*Size does not matter for the main item*/
        size = objectSize;
        isFixed = objectIsFixed;
        angles = pen::Vec3(0.0f, 0.0f, 0.0f);
        isSingular = true;

        shapeType = pen::ui::Shape::OBJ_3D;
        color = objectColor;

        textureName = objectTextureName;

        texCoordStartX = itemTexCoordStartX;
        texCoordStartY = itemTexCoordStartY;
        texCoordEndX = itemTexCoordEndX;
        texCoordEndY = itemTexCoordEndY;

        indices3D = layerIndices;

        /*Checks to make sure the item is on the screen to be rendered*/
        CheckActiveStatus();
    }
#endif

    Item3D::~Item3D() {
        bufferPositions.clear();
        textureName = "";
    }

    void Item3D::Push(pen::ui::Item* item) {
        /*Adds child items to be rendered after item*/
        childItems.push_back(item);
        itemCount++;
    }

    void Item3D::CombineChildBuffers() {
        /*Combines all of the child item buffers after the buffer for this object*/

        /*Checks to make sure the item is on the screen to be rendered*/
        CheckActiveStatus();

        if (childItems.size() > 0) {
            /*Only combine child buffers for the main item*/

            /*This buffer is cleared in case there are any items added to it since it is just a meta object*/
            bufferPositions.clear();

            for (int i = 0; i < childItems.size(); i++) {
                if (!childItems[i]->isActive || !childItems[i]->forceActive) continue;
                /*Adds the buffers to the parent item*/
                for (int j = 0; j < childItems[i]->bufferPositions.size(); j++) {
                    bufferPositions.push_back(childItems[i]->bufferPositions[j]);
                }
            }
        }
    }

    pen::Vec3 Item3D::GetPosition() {
        /*Returns the position of the first triangle*/
        return pen::op::Mat4x4MultVec3(model, childItems[0]->positions, false);
    }

    void Item3D::SetPosition(pen::Vec3 objectPos) {
        /*The set position method for 3d items should do nothing*/
    }

    pen::Vec2 Item3D::GetSize() {
        /*The get size method for 3d items should do nothing*/
        return pen::Vec2(0.0f, 0.0f);
    }

    void Item3D::SetSize(pen::Vec2 objectSize) {
        /*The set size method for 3d items should do nothing*/
    }

    void Item3D::SetColor(pen::Vec4 objectColor) {
        /*Updates the color of the 3d item and its children*/
        color = objectColor;
        for (int i = 0; i < childItems.size(); i++) {
            childItems[i]->color = objectColor;
        }
    }

    void Item3D::UpdateTexture(const std::string& path) {
        /*The update texture method for 3d items should do nothing*/
    }
}
