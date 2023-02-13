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
#include "slider.h"

namespace pen {
    namespace ui {
        Slider::Slider() { 
            isUI = true;
            sliderBarColor = pen::PEN_GRAY;
            sliderSquareColor = pen::PEN_RED;
        }

        Slider::Slider(uint32_t objectId, unsigned int objectSliderOrder, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectSliderSquareColor,
            pen::Vec4 objectSliderBarColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY,
            float itemTexCoordEndX, float itemTexCoordEndY) {
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = pen::PEN_TRANSPARENT;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, pen::PEN_TRANSPARENT);
            }
            id = objectId;
            sliderOrder = objectSliderOrder;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(objectPositions);
            SetSize(objectSize);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isClickable = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            sliderBarColor = objectSliderBarColor;
            sliderSquareColor = objectSliderSquareColor;

            sliderMode = (GetSize()->x > GetSize()->y) ? 0 : 1;

            InitializeSlider();

            CheckActiveStatus();
        }

        bool Slider::OnClick(pen::ui::Item* item, const int& button, const int& action) {
            /*Handles on click events for slider bars*/
            if (userOnClickCallback != nullptr) {
                return (*userOnClickCallback)(item, button, action);
            }
            else {
                if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
                    pen::State::SetDraggable((void*)item);
                }
                return true;
            }
        }

        bool Slider::OnDrag(pen::ui::Item* item, double* xPos, double* yPos) {
            /*Handles on drag events for slider bars*/

            /*The entire slider bar handles this drag event instead of its children*/
            pen::State* inst = pen::State::Get();

            /*Slide the slider square over and update the offset*/
            if (sliderMode == 0) {
                /*Slider is horizontal*/
                item->childItems[1]->SetPosition(pen::Vec3(*xPos - (item->childItems[1]->GetSize()->x / 2.0f), item->childItems[1]->GetPosition()->y, 0.0f));
                if(item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = (item->childItems[1]->GetPosition()->x + (item->childItems[1]->GetSize()->x / 2.0f)
                    - item->GetPosition()->x) / item->GetSize()->x;

                if (item->childItems[1]->GetPosition()->x + (item->childItems[1]->GetSize()->x / 2.0f) < item->GetPosition()->x
                    || *xPos < 0.0f) {
                    /*Set slider square back to beginning*/
                    item->childItems[1]->SetPosition(pen::Vec3(item->GetPosition()->x - (item->childItems[1]->GetSize()->x / 2.0f), item->childItems[1]->GetPosition()->y, 0.0f));
                    if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 0.0f;
                }
                else if (item->childItems[1]->GetPosition()->x + (item->childItems[1]->GetSize()->x / 2.0f) > item->GetPosition()->x + item->GetSize()->x
                    || *xPos > inst->screenWidth) {
                    /*Set slider square back to end*/
                    item->childItems[1]->SetPosition(pen::Vec3(item->GetPosition()->x + item->GetSize()->x - (item->childItems[1]->GetSize()->x / 2.0f), item->childItems[1]->GetPosition()->y, 0.0f));
                    if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 1.0f;
                }
            }
            else {
                /*Slider is vertical*/
                item->childItems[1]->SetPosition(pen::Vec3(item->childItems[1]->GetPosition()->x, *yPos - (item->childItems[1]->GetSize()->y / 2.0f), 0.0f));
                if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = (item->childItems[1]->GetPosition()->y + (item->childItems[1]->GetSize()->y / 2.0f)
                    - item->GetPosition()->y) / item->GetSize()->y;

                if (item->childItems[1]->GetPosition()->y + (item->childItems[1]->GetSize()->y / 2.0f) < item->GetPosition()->y
                    || *yPos < 0.0f) {
                    /*Set slider square back to beginning*/
                    item->childItems[1]->SetPosition(pen::Vec3(item->childItems[1]->GetPosition()->x, item->GetPosition()->y - (item->childItems[1]->GetSize()->y / 2.0f), 0.0f));
                    if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 0.0f;
                }
                else if (item->childItems[1]->GetPosition()->y + (item->childItems[1]->GetSize()->y / 2.0f) > item->GetPosition()->y + item->GetSize()->y
                    || *yPos > inst->screenHeight) {
                    /*Set slider square back to end*/
                    item->childItems[1]->SetPosition(pen::Vec3(item->childItems[1]->GetPosition()->x, item->GetPosition()->y + item->GetSize()->y - (item->childItems[1]->GetSize()->y / 2.0f), 0.0f));
                    if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 1.0f;
                }  
            }

            pen::Flush();
            pen::ui::Submit();

            if (userOnDragCallback != nullptr) {
                return (*userOnDragCallback)(item, xPos, yPos);
            }
            else {
                if (item->parent != nullptr) {
                    item->parent->SliderUpdateItemPositions();

                    /*For text editors*/
                    if (item->parent->parent != nullptr) {
                        if (item->parent->parent->isTextEditor) item->parent->parent->SliderUpdateItemPositions();
                    }
                }
                return true;
            }       
        }

        void Slider::UpdateSliderOrientation(unsigned int mode) {
            /*Updates the orientation of a given slider*/
            if (mode != sliderMode) {
                sliderMode = mode;

                /*Remove previous child items*/
                delete childItems[0];
                delete childItems[1];

                childItems.clear();

                InitializeSlider();
            }
        }

        void Slider::InitializeSlider() {
            if (sliderMode == 0) {
                /*Slider is horizontal*/

                /*Static slider bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y + (GetSize()->y / 2.0f) - (0.1 * GetSize()->y / 2.0f), GetPosition()->z),
                    pen::Vec2(GetSize()->x, 0.1f * GetSize()->y), sliderBarColor, this, userOnClickCallback));

                /*Slider square*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x - (GetSize()->y / 2.0f), GetPosition()->y, GetPosition()->z),
                    pen::Vec2(GetSize()->y, GetSize()->y), sliderSquareColor, this, userOnClickCallback));

                initialSliderPoint = GetPosition()->x;
            }
            else {
                /*Slider is vertical*/

                /*Static slider bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x + (GetSize()->x / 2.0f) - (0.1 * GetSize()->x / 2.0f), GetPosition()->y, GetPosition()->z),
                    pen::Vec2(0.1 * GetSize()->x, GetSize()->y), sliderBarColor, this, userOnClickCallback));

                /*Slider square*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y + GetSize()->y - (GetSize()->x / 2.0f), GetPosition()->z),
                    pen::Vec2(GetSize()->x, GetSize()->x), sliderSquareColor, this, userOnClickCallback));

                initialSliderPoint = GetPosition()->y;
            }

            childItems[0]->isClickable = false;
            childItems[1]->isClickable = false;
        }
    }
}
