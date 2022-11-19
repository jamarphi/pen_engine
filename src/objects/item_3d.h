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
#pragma once
#include "item.h"
#include "../ops/operations/operations.h"
#include "../state/asset.h"
#include <fstream>
#include <sstream>

#define GENERAL_MODEL_SOURCE "pen_engine/res/models/"

namespace pen {
	class GraphicallyAcceleratedItem3D : public pen::ui::Item {
    public:
        GraphicallyAcceleratedItem3D();

        GraphicallyAcceleratedItem3D(bool childItem, pen::Vec3 objectPositions, unsigned int objectShapeType, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            bool objectIsFixed = false, float* objectBufferPositions = nullptr, const std::string& objectTexture = "");

#ifndef __PEN_MAC_IOS__
        GraphicallyAcceleratedItem3D(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            bool objectIsFixed = false, std::string objectTextureName = "",
            float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);
#else
        GraphicallyAcceleratedItem3D(int* layerIndices, uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            bool objectIsFixed = false, std::string objectTextureName = "",
            float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);
#endif

        ~GraphicallyAcceleratedItem3D();

        void Push(pen::ui::Item* item);
        void CombineChildBuffers();
        pen::Vec3 GetPosition();
        void SetPosition(pen::Vec3 objectPos);
        pen::Vec2 GetSize();
        void SetSize(pen::Vec2 objectSize);
        void SetColor(pen::Vec4 objectColor);
        void UpdateTexture(const std::string& path);
	};
}
