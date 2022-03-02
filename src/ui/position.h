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
#include "../objects/item.h"
#include "../state/layer_manager.h"
#include "../state/state.h"

namespace pen {
    namespace ui{
        enum AXIS {
            X = 0,
            Y = 1,
            Z = 2
        };

        static void Translate(pen::ui::Item* object, const pen::Vec3& translationCoord, const bool& staticTransform = false, const bool& nested = true, const bool& firstIter = true) {
            /*param translationCoord: {x, y, z} units from current position*/
            if (object->shapeType != 5) {
                pen::State* inst = pen::State::Get();
                pen::Vec3 pos = object->GetPosition();
                float deltaTime = inst->deltaTime;
                pos.x += translationCoord.x * (staticTransform ? 1.0f : deltaTime);
                pos.y += translationCoord.y * (staticTransform ? 1.0f : deltaTime);
                pos.z += translationCoord.z * (staticTransform ? 1.0f : deltaTime);
                object->SetPosition(pos);

                /*Sets the buffer positions in here just in case object is rotated*/
                if (object->angles.x != 0.0f || object->angles.y != 0.0f || object->angles.z != 0.0f) {
                    for (int i = 0; i < object->bufferPositions.size(); i += SHAPE_BATCH_VERTEX_SIZE) {
                        object->bufferPositions[i] += translationCoord.x * (staticTransform ? 1.0f : inst->deltaTime);
                        object->bufferPositions[i + 1] += translationCoord.y * (staticTransform ? 1.0f : inst->deltaTime);
                        object->bufferPositions[i + 2] += translationCoord.z * (staticTransform ? 1.0f : inst->deltaTime);
                    }
                }

                if (nested) {
                    /*Translates all nested items as well*/
                    for (auto& item : object->childItems) {
                        Translate(item, translationCoord, staticTransform, nested, false);
                    }
                }

                if (firstIter) pen::ui::Submit();
            }
        }

        static pen::Vec2 ScaleByAngle(pen::ui::Item* object, const pen::Vec3& angles, const pen::Vec2& initialScaling) {
            /*Scales an object by its z angle*/
            pen::Vec2 scaled;

            if (angles.z == 0.0f || (initialScaling.x == 1.0f && initialScaling.y == 1.0f) || object->shapeType == pen::ui::Shape::COMPLEX) {
                /*Object is is horizontal*/
                return initialScaling;
            }
            else {
                /*This is only being scaled 2D for primitive objects*/

                if (initialScaling.x > 1.0f) {
                    scaled.x = 1.0f + pen::op::Abs((initialScaling.x - 1.0f) * pen::op::Cos(angles.z));
                }
                else {
                    scaled.x = 1.0f - pen::op::Abs((1.0f - initialScaling.x) * pen::op::Cos(angles.z));
                }

                if (angles.z < 0.0f) {
                    /*Accounts for moving positions down if angle is aiming downward*/
                    if (initialScaling.y > 1.0f) {
                        scaled.y = 1.0f - pen::op::Abs((initialScaling.y - 1.0f) * pen::op::Sin(angles.z));
                    }
                    else {
                        scaled.y = 1.0f + pen::op::Abs((1.0f - initialScaling.y) * pen::op::Cos(angles.z));
                    }
                }
                else {
                    if (initialScaling.y > 1.0f) {
                        scaled.y = 1.0f + pen::op::Abs((initialScaling.y - 1.0f) * pen::op::Sin(angles.z));
                    }
                    else {
                        scaled.y = 1.0f - pen::op::Abs((1.0f - initialScaling.y) * pen::op::Cos(angles.z));
                    }
                }

                return scaled;
            }
        }

        static void Scale(pen::ui::Item* object, const pen::Vec2& scaling, const bool& staticTransform = false, const bool& nested = true, const bool& firstIter = true) {
            /*param translationCoord: {x, y, z} units from current position*/
            if (object->shapeType != 5) {
                pen::State* inst = pen::State::Get();
                pen::Vec2 tempScaling = scaling;

                if (!(tempScaling.x == 0.0f || tempScaling.y == 0.0f || (tempScaling.x == 1.0f && tempScaling.y == 1.0f))) {
                    if (firstIter || nested) {
                        if (tempScaling.x > 1.0f) {
                            tempScaling.x = (1.0f + ((tempScaling.x - 1.0f) * (staticTransform ? 1.0f : inst->deltaTime)));
                        }
                        else {
                            tempScaling.x = (1.0f - ((1.0f - tempScaling.x) * (staticTransform ? 1.0f : inst->deltaTime)));
                        }
                        object->size.x *= tempScaling.x;

                        if (tempScaling.y > 1.0f) {
                            tempScaling.y = (1.0f + ((tempScaling.y - 1.0f) * (staticTransform ? 1.0f : inst->deltaTime)));

                        }
                        else {
                            tempScaling.y = (1.0f - ((1.0f - tempScaling.y) * (staticTransform ? 1.0f : inst->deltaTime)));
                        }
                        object->size.y *= tempScaling.y;
                        object->itemScaling *= tempScaling.x;

                        int vertexNum = object->bufferPositions.size() / SHAPE_BATCH_VERTEX_SIZE;
                        pen::Vec2 pointScaling = ScaleByAngle(object, object->angles, tempScaling);

                        switch (vertexNum) {
                        case 2:
                            /*Lines*/

                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE] *= pointScaling.x;
                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE + 1] *= pointScaling.y;
                            break;
                        case 3:
                            /*Triangles*/
                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE] *= pointScaling.x;
                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE + 1] *= pointScaling.y;

                            /*The third point of the triangle is going at a reciprocal angle to the second*/
                            if (pointScaling.x > 1.0f) {
                                object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE] *= (1.0f - (pointScaling.x - 1.0f));
                            }
                            else {
                                object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE] *= (1.0f + (1.0f - pointScaling.x));
                            }

                            if (pointScaling.y > 1.0f) {
                                object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1] *= (1.0f - (pointScaling.y - 1.0f));
                            }
                            else {
                                object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1] *= (1.0f + (1.0f - pointScaling.y));
                            }
                            break;
                        case 4:
                            /*Quads*/
                            if (object->angles.z != 0.0f && tempScaling.y != 1.0f) {
                                /*This changes the fourth vertex for quads if the object needs to be scaled in the y axis*/
                                if (pointScaling.x > 1.0f) {
                                    object->bufferPositions[3 * SHAPE_BATCH_VERTEX_SIZE] *= (1.0f - (pointScaling.x - 1.0f));
                                }
                                else {
                                    object->bufferPositions[3 * SHAPE_BATCH_VERTEX_SIZE] *= (1.0f + (1.0f - pointScaling.x));
                                }

                                if (pointScaling.y > 1.0f) {
                                    object->bufferPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 1] *= (1.0f - (pointScaling.y - 1.0f));
                                }
                                else {
                                    object->bufferPositions[3 * SHAPE_BATCH_VERTEX_SIZE + 1] *= (1.0f + (1.0f - pointScaling.y));
                                }
                            }

                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE] *= pointScaling.x;
                            object->bufferPositions[SHAPE_BATCH_VERTEX_SIZE + 1] *= pointScaling.y;

                            object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE] *= pointScaling.x;
                            object->bufferPositions[2 * SHAPE_BATCH_VERTEX_SIZE + 1] *= pointScaling.y;

                            break;
                        case 6:
                            /*Complex shapes*/
                            for (int i = 0; i < object->bufferPositions.size(); i += SHAPE_BATCH_VERTEX_SIZE) {
                                object->bufferPositions[i] *= pointScaling.x;
                                object->bufferPositions[i + 1] *= pointScaling.x;
                                object->bufferPositions[i + 2] *= pointScaling.x;
                            }
                        default:
                            break;
                        }
                    }

                    object->UpdateChildOffsets(tempScaling);

                    for (auto& item : object->childItems) {
                        /*Scales all nested items as well*/
                        Scale(item, scaling, staticTransform, nested, false);
                    }

                    if (firstIter) pen::ui::Submit();
                }
            }
        }

        static pen::Vec2 GetPointOfOrigin(pen::ui::Item* object, float* positions, const int& axis) {
            /*Grab point of origin for given angle*/
            switch (axis) {
            case 0:
                return pen::Vec2(positions[1] + object->size.y / 2.0f, positions[2]);
            case 1:
                return pen::Vec2(positions[0] + object->size.x / 2.0f, positions[2]);
            case 2:
                if (object->shapeType == 1) {
                    /*Line*/
                    return pen::Vec2(positions[0] + object->size.x / 2.0f, positions[1]);
                }
                else {
                    return pen::Vec2(positions[0] + object->size.x / 2.0f, positions[1] + object->size.y / 2.0f);
                }
            default:
                return pen::Vec2(0.0f, 0.0f);
            }
        }

        static void Rotate(pen::ui::Item* object, const float& degrees, const int& axis, const bool& staticTransform = false, 
            const pen::Vec2& objectPointOfOrigin = pen::Vec2(0.0f, 0.0f), const bool& nested = true, 
            const bool& firstIter = true, const bool& calculatePointOfOrigin = true) {
            /*Rotates an item either clockwise or counter-clockwise along the specified axis*/
            if (object->shapeType != 5) {
                float objectAngle = degrees * 3.14159f / 180.0f;
                if ((objectAngle > 0.0f || objectAngle < 0.0f) && objectAngle < 2.0f * 3.14159f && objectAngle > -2.0f * 3.14159f) {
                    float angle = objectAngle * (staticTransform ? 1.0f : pen::State::Get()->deltaTime);

                    /*If angle is exactly pi offset it by a little bit*/
                    if (angle == 3.14159f || angle == -3.14159f) angle -= 0.01f;

                    /*It's offset the other way to get an obtuse angle if angle is greater than pi*/
                    if (angle > 3.14159f) angle = -1.0f * (3.14159f - (angle - 3.14159f));
                    if (angle < -3.14159f) angle = 3.14159f + (angle + 3.14159f);

                    if (axis == 0) object->angles.x += angle;
                    if (axis == 1) object->angles.y += angle;
                    if (axis == 2) object->angles.z += angle;

                    if (object->angles.x >= 2.0f * 3.14159f || object->angles.x <= -2.0f * 3.14159f) object->angles.x = 0.0f;
                    if (object->angles.y >= 2.0f * 3.14159f || object->angles.y <= -2.0f * 3.14159f) object->angles.y = 0.0f;
                    if (object->angles.z >= 2.0f * 3.14159f || object->angles.z <= -2.0f * 3.14159f) object->angles.z = 0.0f;

                    /*Resets the buffer positions before rotating*/
                    std::vector<float> tempPositions;
                    if (object->shapeType == pen::ui::Shape::COMPLEX) {
                        tempPositions = object->bufferPositions;
                    }
                    else {
                        tempPositions = (object->isText) ? pen::ui::Shape::BitmapFontPosition(object->positions, object->size, object->color, pen::State::Get()->asciiMap.Find(object->textureName)->second, &object->bufferPositions[0], 0.0f, 0.0f, 0.0f)
                            : pen::ui::Shape::GetBatchPosition(object->positions, object->size, object->shapeType, object->color, &object->bufferPositions[0], 0.0f, 0.0f, 0.0f, object->GetAssetId(), object->texCoordStartX, object->texCoordStartY, object->texCoordEndX, object->texCoordEndY);
                    }

                    pen::Vec2 pointOfOrigin = (calculatePointOfOrigin && object->shapeType != pen::ui::Shape::COMPLEX) ? GetPointOfOrigin(object, &tempPositions[0], axis) : objectPointOfOrigin;

                    int point1 = 0, point2 = 0;
                    float chosenAngle = 0.0f;
                    switch (axis) {
                    case 0:
                        /*X axis*/
                        point1 = 1;
                        point2 = 2;
                        chosenAngle = object->angles.x;
                        break;
                    case 1:
                        /*Y axis*/
                        point1 = 0;
                        point2 = 2;
                        chosenAngle = object->angles.y;
                        break;
                    case 2:
                        /*Z axis*/
                        point1 = 0;
                        point2 = 1;
                        chosenAngle = object->angles.z;
                        break;
                    default:
                        break;
                    }

                    for (int i = 0; i < object->bufferPositions.size(); i += SHAPE_BATCH_VERTEX_SIZE) {

                        int xShift = tempPositions[i + point1] - pointOfOrigin.x;
                        int yShift = tempPositions[i + point2] - pointOfOrigin.y;

                        /*Calculate the rotated coordinates, has to be normalized to fit -1.0f to 1.0f z range*/
                        object->bufferPositions[i + point1] = (pointOfOrigin.x + (xShift * pen::op::Cos(chosenAngle) - yShift * pen::op::Sin(chosenAngle))) / (point1 == 2 ? 100.0f : 1.0f);
                        object->bufferPositions[i + point2] = (pointOfOrigin.y + (xShift * pen::op::Sin(chosenAngle) + yShift * pen::op::Cos(chosenAngle))) / (point2 == 2 ? 100.0f : 1.0f);
                    }

                    /*Run through any child items to do the same thing*/
                    if (nested) {
                        /*Rotates all nested items as well*/
                        for (auto& item : object->childItems) {
                            Rotate(item, objectAngle, axis, staticTransform, pointOfOrigin, nested, false, calculatePointOfOrigin);
                        }
                    }

                    if (firstIter) pen::ui::Submit();
                }
            }
        }
    }
}