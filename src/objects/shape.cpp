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
#include "shape.h"

namespace pen {
    namespace ui {
        unsigned int Shape::POINT = 0;
        unsigned int Shape::LINE = 1;
        unsigned int Shape::TRI = 2;
        unsigned int Shape::QUAD = 3;
        unsigned int Shape::BUFFER = 4;
        unsigned int Shape::OBJ_3D = 5;
        unsigned int Shape::COMPLEX = 6;

        std::vector<float> Shape::GetItemBatchData(pen::Vec3 pos, pen::Vec2 size, unsigned int shapeType, pen::Vec4 color, float* oldPositions, const float& angleX, const float& angleY, const float& angleZ, float texId,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            /*
            Pos position is for the bottom left corner of your sprite.
            Size is essentially the length and width, or base and height of your shapes.
            If the angle in radians is not 0, the original buffer position attribute will be copied since it was rotated.

            Currently the attributes go:
                Position (x, y, z)
                Color (r, g, b, a)
                Texture Coordinates (coefficients from 0 to 1 starting from the bottom left of the texture)
                Texture Id (this is determined based on which asset you decide to use for a given object)
            */
            std::vector<float> positionsVect;
            switch (shapeType) {
            case 0:
                //Point
                positionsVect = { pos.x, pos.y, pos.z, color.x, color.y, color.z, color.w,0.0f, 0.0f,0.0f };
                break;
            case 1:
                //Line
                if (angleX != 0.0f || angleY != 0.0f || angleZ != 0.0) {
                    positionsVect = {
                        oldPositions[0], oldPositions[1], oldPositions[2], color.x, color.y, color.z, color.w,0.0f, 0.0f,0.0f,
                        oldPositions[SHAPE_BATCH_VERTEX_SIZE], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,0.0f, 0.0f,0.0f
                    };
                }
                else {
                    positionsVect = {
                        pos.x, pos.y, pos.z, color.x, color.y, color.z, color.w,0.0f, 0.0f,0.0f,
                        pos.x + size.x, pos.y, pos.z, color.x, color.y, color.z, color.w,0.0f, 0.0f,0.0f
                    };
                }
                break;
            case 2:
                //Triangle
                if (angleX != 0.0f || angleY != 0.0f || angleZ != 0.0) {
                    positionsVect = {
                            oldPositions[0], oldPositions[1], oldPositions[2], color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 0.0f + itemTexCoordStartY,texId,
                            oldPositions[SHAPE_BATCH_VERTEX_SIZE], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 0.0f + itemTexCoordStartY,texId,
                            oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w, 0.5f, 1.0f * itemTexCoordEndY,texId
                    };
                }
                else {
                    positionsVect = {
                            pos.x, pos.y, pos.z, color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 0.0f + itemTexCoordStartY,texId,
                            pos.x + size.x, pos.y, pos.z, color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 0.0f + itemTexCoordStartY,texId,
                            (pos.x + (size.x / 2.0f)), pos.y + size.x, pos.z, color.x, color.y, color.z, color.w, 0.5f, 1.0f * itemTexCoordEndY,texId
                    };
                }
                break;
            case 3:
            case 4:
                //Quad    
                if (angleX != 0.0f || angleY != 0.0f || angleZ != 0.0) {
                    positionsVect = {
                            oldPositions[0], oldPositions[1], oldPositions[2], color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 0.0f + itemTexCoordStartY,texId,
                            oldPositions[SHAPE_BATCH_VERTEX_SIZE], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 0.0f + itemTexCoordStartY,texId,
                            oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 1.0f * itemTexCoordEndY,texId,
                            oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE], oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 1.0f * itemTexCoordEndY,texId
                    };
                }
                else {
                    positionsVect = {
                            pos.x, pos.y, pos.z, color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 0.0f + itemTexCoordStartY,texId,
                            pos.x + size.x, pos.y, pos.z, color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 0.0f + itemTexCoordStartY,texId,
                            pos.x + size.x, pos.y + size.y, pos.z, color.x, color.y, color.z, color.w,1.0f * itemTexCoordEndX, 1.0f * itemTexCoordEndY,texId,
                            pos.x, pos.y + size.y, pos.z, color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 1.0f * itemTexCoordEndY,texId
                    };
                }
                break;
            case 5:
                //Obj file 3D model triangle/quad primitives
                positionsVect = {
                            oldPositions[0], oldPositions[1], oldPositions[2], color.x, color.y, color.z, color.w,0.0f + itemTexCoordStartX, 0.0f + itemTexCoordStartY,texId
                };
                break;
            default:
                break;
            }

            return positionsVect;
        }

        std::vector<float> Shape::BitmapFontPosition(pen::Vec3 pos, pen::Vec2 size, pen::Vec4 color, unsigned char characterIndex, float* oldPositions, const float& angleX, const float& angleY, const float& angleZ) {
            /*Returns buffer positions for objects using the bitmap font*/

            /*
            Add the texture coordinates for this particular object using the bitmap font.
            In the bitmap being used, there are 18 letters per row and 7 rows in an 8 px x 8 px png.
            This bitmap starts with the 32 index character in the ASCII table.

            Currently the attributes go:
                Position (x, y, z)
                Color (r, g, b, a)
                Texture Coordinates (coefficients from 0 to 1 starting from the bottom left of the texture)
                Texture Id (this is determined based on which asset you decide to use for a given object)
            */
            float charUnit = 1.0f / 18.0f;
            float rowHeight = 1.0f / 7.0f;

            float xPos = (characterIndex % 18) * charUnit - (charUnit / 6.0f);
            float yPos = (6 - characterIndex / 18) * rowHeight;

            if (xPos < 0.0f) xPos = 0.0f;

            std::vector<float> positionsVect;

            if (angleX != 0.0f || angleY != 0.0f || angleZ != 0.0) {
                positionsVect = {
                            oldPositions[0], oldPositions[1], oldPositions[2], color.y, color.z, color.w,xPos, yPos,BITMAP_FONT_ID,
                            oldPositions[SHAPE_BATCH_VERTEX_SIZE], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,xPos + charUnit, yPos,BITMAP_FONT_ID,
                            oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,xPos + charUnit, yPos + rowHeight,BITMAP_FONT_ID,
                            oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE], oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 1], oldPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 2], color.x, color.y, color.z, color.w,xPos, yPos + rowHeight,BITMAP_FONT_ID
                };
            }
            else {
                positionsVect = {
                            pos.x, pos.y, pos.z, color.x, color.y, color.z, color.w,xPos, yPos,BITMAP_FONT_ID,
                            pos.x + size.x, pos.y, pos.z, color.x, color.y, color.z, color.w,xPos + charUnit, yPos,BITMAP_FONT_ID,
                            pos.x + size.x, pos.y + size.y, pos.z, color.x, color.y, color.z, color.w,xPos + charUnit, yPos + rowHeight,BITMAP_FONT_ID,
                            pos.x, pos.y + size.y, pos.z, color.x, color.y, color.z, color.w,xPos, yPos + rowHeight,BITMAP_FONT_ID
                };
            }

            return positionsVect;
        }
    }
}
