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
#include <vector>
#include <string>
#include "../ops/vectors/vec4.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec2.h"
#include "../state/config.h"

#define BITMAP_FONT_ID 1.0f
#define SHAPE_BATCH_VERTEX_SIZE 10

namespace pen {
    namespace ui {
        class Shape {
        public:
            static unsigned int POINT;
            static unsigned int LINE;
            static unsigned int TRI;
            static unsigned int QUAD;
            static unsigned int BUFFER;
            static unsigned int OBJ_3D;
            static unsigned int COMPLEX;
            static std::vector<float> GetItemBatchData(pen::Vec3 pos, pen::Vec2 size, unsigned int shapeType, pen::Vec4 color, float* oldPositions, const float& angleX, const float& angleY, const float& angleZ,
                float texId = 0.0f,
                float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);
            static std::vector<float> BitmapFontPosition(pen::Vec3 pos, pen::Vec2 size, pen::Vec4 color, unsigned char characterIndex, float* oldPositions, const float& angleX, const float& angleY, const float& angleZ);
        };
    }
}
