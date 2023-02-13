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
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectColor;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(objectPositions);
            SetSize(objectSize);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isClickable = true;
            sliderOrder = order;
            sliderMode = mode;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            SetColor(pen::PEN_TRANSPARENT);

            if (sliderMode == 0) {
                /*Left button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y, GetPosition()->z),
                    pen::Vec2(0.1f * GetSize()->x, GetSize()->y), objectColor, this, onClickCallback));

                /*Right button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x + GetSize()->x - (0.1f * GetSize()->x), GetPosition()->y, GetPosition()->z),
                    pen::Vec2(0.1f * GetSize()->x, GetSize()->y), objectColor, this, onClickCallback));

                /*Bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x + (0.1f * GetSize()->x), GetPosition()->y, GetPosition()->z),
                    pen::Vec2(0.2f * GetSize()->x, GetSize()->y), objectColor, this, onClickCallback));
            }
            else {
                /*Up button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y + GetSize()->y - (0.1f * GetSize()->y), GetPosition()->z),
                    pen::Vec2(GetSize()->x, 0.1f * GetSize()->y), objectColor, this, onClickCallback));

                /*Down button*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y, GetPosition()->z),
                    pen::Vec2(GetSize()->x, 0.1f * GetSize()->y), objectColor, this, onClickCallback));

                /*Bar*/
                Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x, GetPosition()->y + GetSize()->y - (0.1f * GetSize()->y) - (0.2f * GetSize()->y), GetPosition()->z),
                    pen::Vec2(GetSize()->x, 0.2f * GetSize()->y), objectColor, this, onClickCallback));
            }

            childItems[0]->isClickable = false;
            childItems[1]->isClickable = false;
            childItems[2]->isClickable = false;

            CheckActiveStatus();
        }

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
                    scrollTranslation = (*xPos - (item->childItems[2]->GetPosition()->x + (item->childItems[2]->GetSize()->x / 2.0f)));

                    item->childItems[2]->SetPosition(pen::Vec3(*xPos - (item->childItems[2]->GetSize()->x / 2.0f), item->childItems[2]->GetPosition()->y, 0.0f));
                    if (item->parent != nullptr) item->childItems[2]->SetPosition(pen::Vec3(item->childItems[2]->GetPosition()->x,
                        1.0f - (item->parent->sliderOffset[sliderOrder] = ((item->childItems[2]->GetPosition()->x + (item->childItems[2]->GetSize()->x / 2.0f)
                            - item->GetPosition()->x) / (item->GetSize()->x - item->childItems[0]->GetSize()->x - item->childItems[1]->GetSize()->x)))
                        , 0.0f));
                     
                    
                    if (item->parent != nullptr) {
                        if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                        /*For text editors*/
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                        }
                    }

                    if (item->childItems[2]->GetPosition()->x + item->childItems[2]->GetSize()->x > item->childItems[1]->GetPosition()->x
                        || *xPos > inst->screenWidth) {
                        /*Set scroll bar back to end*/
                        item->childItems[2]->SetPosition(pen::Vec3(item->childItems[1]->GetPosition()->x - item->childItems[2]->GetSize()->x, item->childItems[2]->GetPosition()->y, 0.0f));
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
                    else if (item->childItems[2]->GetPosition()->x < item->childItems[0]->GetPosition()->x + item->childItems[0]->GetSize()->x
                        || *xPos < 0.0f) {
                        /*Set scroll bar back to beginning*/
                        item->childItems[2]->SetPosition(pen::Vec3(item->childItems[0]->GetPosition()->x + item->childItems[0]->GetSize()->x, item->childItems[2]->GetPosition()->y, 0.0f));
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
                    scrollTranslation = (*yPos - (item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)));

                    item->childItems[2]->SetPosition(pen::Vec3(item->childItems[2]->GetPosition()->x, (*yPos - (item->childItems[2]->GetSize()->y / 2.0f)), 0.0f));
                    if (item->parent != nullptr) item->childItems[2]->SetPosition(pen::Vec3(item->childItems[2]->GetPosition()->x,
                        1.0f - (item->parent->sliderOffset[sliderOrder] = (1 - ((item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)
                            - item->GetPosition()->y) / (item->GetSize()->y - item->childItems[0]->GetSize()->y - item->childItems[1]->GetSize()->y))))
                        , 0.0f));

                    if (item->parent != nullptr) {
                        if (!item->parent->isList) item->parent->ScrollBarUpdateItemPositions();

                        /*For text editors*/
                        if (item->parent->parent != nullptr) {
                            if (item->parent->parent->isTextEditor) item->parent->parent->ScrollBarUpdateItemPositions();
                        }
                    }

                    if (item->childItems[2]->GetPosition()->y < item->childItems[1]->GetPosition()->y + item->childItems[1]->GetSize()->y
                        || *yPos < 0.0f) {
                        /*Set scroll bar back to end*/
                        item->childItems[2]->SetPosition(pen::Vec3(item->childItems[2]->GetPosition()->x, item->GetPosition()->y + item->childItems[1]->GetSize()->y, 0.0f));
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
                    else if (item->childItems[2]->GetPosition()->y + item->childItems[2]->GetSize()->y > item->childItems[0]->GetPosition()->y
                        || *yPos > inst->screenHeight) {
                        /*Set scroll bar back to beginning*/
                        item->childItems[2]->SetPosition(pen::Vec3(item->childItems[2]->GetPosition()->x, item->GetPosition()->y + item->GetSize()->y - item->childItems[0]->GetSize()->y - item->childItems[2]->GetSize()->y, 0.0f));
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
            pen::Vec2 translation = pen::Vec2(0.0f, 0.0f);
            
            if (scrollTranslation + (item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)) < item->GetPosition()->y + item->childItems[1]->GetSize()->y + (item->childItems[2]->GetSize()->y / 2.0f)) {
                /*Cap it at the bottom*/
                translation.y = -1.0f * (item->GetPosition()->y + item->childItems[1]->GetSize()->y - (item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)))
                    * heightOfScrollParentItems / item->GetSize()->y;
            }
            else if (scrollTranslation + (item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)) > item->GetPosition()->y + item->GetSize()->y - item->childItems[0]->GetSize()->y) {
                /*Cap it at the top*/
                translation.y = -1.0f * (item->GetPosition()->y + item->GetSize()->y - item->childItems[0]->GetSize()->y - (item->childItems[2]->GetPosition()->y + (item->childItems[2]->GetSize()->y / 2.0f)))
                    * heightOfScrollParentItems / item->GetSize()->y;
            }
            else {
                translation.y = -1.0f * scrollTranslation * heightOfScrollParentItems / item->GetSize()->y;
            }

            for (int i = 1; i < item->parent->childItems.size() - 1; i++) {
                pen::ui::Translate(item->parent->childItems[i], translation, true);

                /*Check to make sure each item is not outside boundaries*/
                if (item->parent->childItems[i]->GetPosition()->y < item->parent->GetPosition()->y
                    || (item->parent->childItems[i]->GetPosition()->y + item->parent->childItems[i]->GetSize()->y > item->parent->childItems[0]->GetPosition()->y)) {
                    item->parent->childItems[i]->AllowActive(false);
                }
                else {
                    item->parent->childItems[i]->AllowActive(true);
                }
            }

            pen::Flush();
            pen::ui::Submit();
        }
    }
}
