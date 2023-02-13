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
    GraphicallyAcceleratedItem3D::GraphicallyAcceleratedItem3D() {}

    GraphicallyAcceleratedItem3D::GraphicallyAcceleratedItem3D(bool childItem, pen::Vec3 objectPositions, unsigned int objectShapeType, pen::Vec4 objectColor,
        float* objectBufferPositions, const std::string& objectTexture) {
        /*3D child item constructor*/
        SetPosition(objectPositions);
        SetSize(pen::Vec2(50.0f, 50.0f));
        isSingular = true;
        shapeType = objectShapeType;
        color = objectColor;
        model = pen::Mat4x4(1.0f);
        textureName = objectTexture;

        bufferPositions = pen::ui::Shape::GetItemBatchData(*GetPosition(), *GetSize(), pen::ui::Shape::OBJ_3D, objectColor, objectBufferPositions, 0.0f, 0.0f, 0.0f, GetAssetId());

        /*Checks to make sure the item is on the screen to be rendered*/
        CheckActiveStatus();
    }

#ifndef __PEN_MAC_IOS__
    GraphicallyAcceleratedItem3D::GraphicallyAcceleratedItem3D(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
        std::string objectTextureName,
        float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
        /*Regular constructor*/
        id = objectId;
        SetPosition(objectPositions);
        /*Size does not matter for the main item*/
        SetSize(objectSize);
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
    GraphicallyAcceleratedItem3D::GraphicallyAcceleratedItem3D(int* layerIndices, uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
        std::string objectTextureName,
        float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
        /*Regular constructor*/
        id = objectId;
        SetPosition(objectPositions);
        /*Size does not matter for the main item*/
        SetSize(objectSize);
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

    GraphicallyAcceleratedItem3D::~GraphicallyAcceleratedItem3D() {}

    void GraphicallyAcceleratedItem3D::Push(pen::ui::Item* item) {
        /*Adds child items to be rendered after item*/
        childItems.push_back(item);
        itemCount++;
    }

    void GraphicallyAcceleratedItem3D::CombineChildBuffers() {
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

    pen::Vec3* GraphicallyAcceleratedItem3D::GetPosition() {
        /*The get position method for 3d items should do nothing, this is done by translating the model matrix with the first triangle manually*/
        return nullptr;
    }

    void GraphicallyAcceleratedItem3D::SetPosition(pen::Vec3 objectPos) {
        /*The set position method for 3d items should do nothing*/
    }

    pen::Vec2* GraphicallyAcceleratedItem3D::GetSize() {
        /*The get size method for 3d items should do nothing*/
        return nullptr;
    }

    void GraphicallyAcceleratedItem3D::SetSize(pen::Vec2 objectSize) {
        /*The set size method for 3d items should do nothing*/
    }

    void GraphicallyAcceleratedItem3D::SetColor(pen::Vec4 objectColor) {
        /*The set color function should do nothing*/
    }

    void GraphicallyAcceleratedItem3D::UpdateTexture(const std::string& path) {
        /*The update texture method for 3d items should do nothing*/
    }
}
