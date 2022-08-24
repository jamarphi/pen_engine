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
#include "scroll_bar.h"

namespace pen {
    namespace ui {
        ScrollBar::ScrollBar() { isUI = true; }

        ScrollBar::ScrollBar(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
            pen::Vec4 objectAccentColor, unsigned int order, unsigned int mode, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY,
            float itemTexCoordEndX, float itemTexCoordEndY) {
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            positions = objectPositions;
            size = objectSize;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = true;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = true;
            sliderOrder = order;
            sliderMode = mode;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, pen::ui::Shape::QUAD, pen::PEN_TRANSPARENT, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = pen::PEN_TRANSPARENT;

            if (sliderMode == 0) {
                /*Left button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x, positions.y, positions.z),
                    pen::Vec2(0.1f * size.x, size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));

                /*Right button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x + size.x - (0.1f * size.x), positions.y, positions.z),
                    pen::Vec2(0.1f * size.x, size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));

                /*Bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x + (0.1f * size.x), positions.y, positions.z),
                    pen::Vec2(0.2f * size.x, size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));
            }
            else {
                /*Up button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x, positions.y + size.y - (0.1f * size.y), positions.z),
                    pen::Vec2(size.x, 0.1f * size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));

                /*Down button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x, positions.y, positions.z),
                    pen::Vec2(size.x, 0.1f * size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));

                /*Bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x, positions.y + size.y - (0.1f * size.y) - (0.2f * size.y), positions.z),
                    pen::Vec2(size.x, 0.2f * size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));
            }

            childItems[0]->isClickable = false;
            childItems[1]->isClickable = false;
            childItems[2]->isClickable = false;

            CheckActiveStatus();
        }

        ScrollBar::~ScrollBar() {}

        bool ScrollBar::OnClick(pen::ui::Item* item, const int& button, const int& action) {
            /*Handles on click events for scroll bars*/
            if (userOnClickCallback != nullptr) {
                return (*userOnClickCallback)(this, button, action);
            }
            else {
                /*The entire scroll bar handles this click event instead of its children*/
                if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
                    pen::State::SetDraggable((void*)item);
                }
                return true;
            }
        }

        bool ScrollBar::OnDrag(pen::ui::Item* item, double* xPos, double* yPos) {
            /*Handles on drag events for slider bars*/
            if (userOnDragCallback != nullptr) {
                return (*userOnDragCallback)(item, xPos, yPos);
            }
            else {
                /*The entire scroll bar handles this drag event instead of its children*/
                pen::State* inst = pen::State::Get();

                float scrollTranslation = 0.0f;
                bool scrollItems = true;

                if (sliderMode == 0) {
                    /*Scroll bar is horizontal*/
                    scrollTranslation = (*xPos - (item->childItems[2]->positions.x + (item->childItems[2]->size.x / 2.0f)));

                    item->childItems[2]->positions.x = (*xPos - (item->childItems[2]->size.x / 2.0f));
                    if (item->parent != nullptr) 1.0f - (item->parent->sliderOffset[sliderOrder] = ((item->childItems[2]->positions.x + (item->childItems[2]->size.x / 2.0f)
                        - item->positions.x) / (item->size.x - item->childItems[0]->size.x - item->childItems[1]->size.x)));

                    if (item->parent != nullptr) {
                        if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                        /*For text editors*/
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                        }
                    }

                    if (item->childItems[2]->positions.x + item->childItems[2]->size.x > item->childItems[1]->positions.x
                        || *xPos > inst->screenWidth) {
                        /*Set scroll bar back to end*/
                        item->childItems[2]->positions.x = item->childItems[1]->positions.x - item->childItems[2]->size.x;
                        if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 1.0f;
                        scrollItems = false;
                        if (item->parent != nullptr) {
                            if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                            /*For text editors*/
                            if (item->parent->parent != nullptr) {
                                if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                            }
                        }
                    }
                    else if (item->childItems[2]->positions.x < item->childItems[0]->positions.x + item->childItems[0]->size.x
                        || *xPos < 0.0f) {
                        /*Set scroll bar back to beginning*/
                        item->childItems[2]->positions.x = item->childItems[0]->positions.x + item->childItems[0]->size.x;
                        if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 0.0f;
                        scrollItems = false;
                        if (item->parent != nullptr) {
                            item->parent->ScrollBarUpdateItemPositions();

                            /*For text editors*/
                            if (item->parent->parent != nullptr) {
                                if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                            }
                        }
                    }
                }
                else {
                    /*Scroll bar is vertical*/
                    scrollTranslation = (*yPos - (item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)));

                    item->childItems[2]->positions.y = (*yPos - (item->childItems[2]->size.y / 2.0f));
                    if (item->parent != nullptr) 1.0f - (item->parent->sliderOffset[sliderOrder] = (1 - ((item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)
                        - item->positions.y) / (item->size.y - item->childItems[0]->size.y - item->childItems[1]->size.y))));

                    if (item->parent != nullptr) {
                        if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                        /*For text editors*/
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                        }
                    }

                    if (item->childItems[2]->positions.y < item->childItems[1]->positions.y + item->childItems[1]->size.y
                        || *yPos < 0.0f) {
                        /*Set scroll bar back to end*/
                        item->childItems[2]->positions.y = item->positions.y + item->childItems[1]->size.y;
                        if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 1.0f;
                        scrollItems = false;
                        if (item->parent != nullptr) {
                            if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                            /*For text editors*/
                            if (item->parent->parent != nullptr) {
                                if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                            }
                        }
                    }
                    else if (item->childItems[2]->positions.y + item->childItems[2]->size.y > item->childItems[0]->positions.y
                        || *yPos > inst->screenHeight) {
                        /*Set scroll bar back to beginning*/
                        item->childItems[2]->positions.y = item->positions.y + item->size.y - item->childItems[0]->size.y - item->childItems[2]->size.y;
                        if (item->parent != nullptr) item->parent->sliderOffset[sliderOrder] = 0.0f;
                        scrollItems = false;
                        if (item->parent != nullptr) {
                            item->parent->ScrollBarUpdateItemPositions();

                            /*For text editors*/
                            if (item->parent->parent != nullptr) {
                                if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                            }
                        }
                    }
                }

                if (item->parent != nullptr && scrollItems && sliderMode == 1) {
                    VerticalListDrag(item, xPos, yPos, scrollTranslation);
                }
                else {
                    pen::ui::Submit();
                }

                return true;
            }
        }

        void ScrollBar::VerticalListDrag(pen::ui::Item* item, double* xPos, double* yPos, const float& scrollTranslation) {
            /*Updates the list items of the vertical list component*/
            pen::Vec3 translation = pen::Vec3(0.0f, 0.0f, 0.0f);
            
            if (scrollTranslation + (item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)) < item->positions.y + item->childItems[1]->size.y + (item->childItems[2]->size.y / 2.0f)) {
                /*Cap it at the bottom*/
                translation.y = -1.0f * (item->positions.y + item->childItems[1]->size.y - (item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)))
                    * heightOfScrollParentItems / item->size.y;
            }
            else if (scrollTranslation + (item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)) > item->positions.y + item->size.y - item->childItems[0]->size.y) {
                /*Cap it at the top*/
                translation.y = -1.0f * (item->positions.y + item->size.y - item->childItems[0]->size.y - (item->childItems[2]->positions.y + (item->childItems[2]->size.y / 2.0f)))
                    * heightOfScrollParentItems / item->size.y;
            }
            else {
                translation.y = -1.0f * scrollTranslation * heightOfScrollParentItems / item->size.y;
            }

            for (int i = 1; i < item->parent->childItems.size() - 1; i++) {
                pen::ui::Translate(item->parent->childItems[i], translation, true, true, false);

                /*Check to make sure each item is not outside boundaries*/
                if (item->parent->childItems[i]->positions.y < item->parent->positions.y
                    || (item->parent->childItems[i]->positions.y + item->parent->childItems[i]->size.y > item->parent->childItems[0]->positions.y)) {
                    item->parent->childItems[i]->AllowActive(false);
                }
                else {
                    item->parent->childItems[i]->AllowActive(true);
                }
            }

            pen::ui::Submit();
        }
    }
}
