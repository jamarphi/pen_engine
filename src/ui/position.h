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

    enum AnimationType {
        ROTATE_X = 0,
        ROTATE_Y = 1,
        ROTATE_Z = 2,
        TRANSLATE = 3,
        SCALE = 4,
        PAN = 5,
        LOOK = 6,
        ZOOM = 7,
        COLOR = 8,
        CUSTOM = 9
    };

    namespace ui{
        enum AXIS {
            X = 0,
            Y = 1,
            Z = 2
        };

        static void Translate(pen::ui::Item* object, const pen::Vec2& translationCoord, const bool& nested = true) {
            /*Translates GUI items*/
            if (object->shapeType != 5) {
                /*
                The translation is done by setting the x and y directly rather than with the pixel item's matrix due to
                an out of bounds drawing error when the translation is very large
                */
                pen::State* inst = pen::State::Get();
                pen::Vec3* pos = object->GetPosition();
                pos->x += translationCoord.x;
                object->data->x += translationCoord.x;
                pos->y += translationCoord.y;
                object->data->y += translationCoord.y;

                if (nested) {
                    /*Translates all nested items as well*/
                    for (auto& item : object->childItems) {
                        Translate(item, translationCoord, nested);
                    }
                }
            }
        }

        static void Scale(pen::ui::Item* object, const pen::Vec2& scaling, const bool& nested = true) {
            /*Scales GUI items*/
            if (object->shapeType != 5) {
                /*The size for the pixel sprite and the size variable of the GUI item are both updated in SetSize*/
                pen::State* inst = pen::State::Get();
                pen::Vec2 tempScaling = scaling;
                pen::Vec2* size = object->GetSize();

                object->SetSize(pen::Vec2(size->x * scaling.x, size->y * scaling.y));
                object->UpdateChildOffsets(tempScaling);

                for (auto& item : object->childItems) {
                    /*Scales all nested items as well*/
                    Scale(item, scaling, nested);
                }
            }
        }

        static void Rotate(pen::ui::Item* object, float& angle, const bool& nested = true) {
            /*Rotates an item either clockwise or counter-clockwise along the specified axis*/
            if (object->shapeType != 5) {
                if ((angle > 0.0f || angle < 0.0f) && angle < 2.0f * 3.14159f && angle > -2.0f * 3.14159f) {

                    /*If angle is exactly pi offset it by a little bit*/
                    if (angle == 3.14159f || angle == -3.14159f) angle -= 0.01f;

                    /*It's offset the other way to get an obtuse angle if angle is greater than pi*/
                    if (angle > 3.14159f) angle = -1.0f * (3.14159f - (angle - 3.14159f));
                    if (angle < -3.14159f) angle = 3.14159f + (angle + 3.14159f);

                    pen::Rotate(object->data, angle);

                    /*Run through any child items to do the same thing*/
                    if (nested) {
                        /*Rotates all nested items as well*/
                        for (auto& item : object->childItems) {
                            Rotate(item, angle, nested);
                        }
                    }
                }
            }
        }
    }
}
