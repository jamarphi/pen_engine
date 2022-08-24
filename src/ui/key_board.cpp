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
#include "key_board.h"

bool OnKeyBoardKeyPressed(pen::ui::Item* item, int button, int action) {
#ifndef __PEN_MOBILE__
    if (action == pen::in::KEYS::PRESSED) {
        if (item->origText == "") {
            item->parent->KeyBoardReturnKey(item);
        }
        else {
            item->parent->parent->KeyBoardReturnKey(item);
        }
    }
#else
    if (item->origText == "") {
        item->parent->KeyBoardReturnKey(item);
    }
    else {
        item->parent->parent->KeyBoardReturnKey(item);
    }
#endif
    return true;
}

namespace pen {
    namespace ui {
        KeyBoard::KeyBoard() { isUI = true; }

        KeyBoard::KeyBoard(uint32_t objectId, void (*onKeyReturn)(char key), pen::Vec4 objectColor,
            pen::Vec4 objectAccentColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            id = objectId;
            isUI = true;
            userOnKeyReturn = onKeyReturn;
            positions = pen::Vec3(0.0f, 0.0f, 0.0f);
            size = pen::Vec2(pen::State::Get()->screenWidth, pen::State::Get()->screenHeight / 2.0f);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = true;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = objectColor;

            InitializeKeys(objectAccentColor);

            CheckActiveStatus();
        }

        KeyBoard::~KeyBoard() {}

        void KeyBoard::InitializeKeys(pen::Vec4 accentColor) {
            /*Initializes the keys for the key board*/
            float keyHeight = 0.0925f * size.y;
            pen::ui::Item* key = nullptr;
            pen::ui::Item* topRow = new pen::ui::Div(ID_ANY, pen::Vec3(0.0f, positions.y + size.y - (2.0f * keyHeight), 0.0f), pen::Vec2(size.x, keyHeight),
                pen::PEN_TRANSPARENT, 0, this);
            key = new pen::ui::Button(ID_ANY, "1", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.x += (0.5f * keyHeight);
            key->childItems[0]->positions.y -= (0.6f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "2", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (1.4f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "3", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (1.1f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "4", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (1.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "5", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.8f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "6", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.675f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "7", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.66f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "8", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.6f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "9", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.x -= (0.2f * keyHeight);
            key->childItems[0]->positions.y -= (0.6f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "0", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.5f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "-", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (1.7f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "=", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.33f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "<", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, topRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.33f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            topRow->Push(key);
            pen::ui::Scale(topRow, pen::Vec2(1.0f, (size.y / 5.0f) / topRow->size.y), true);

            pen::ui::Item* secondTopRow = new pen::ui::Div(ID_ANY, pen::Vec3(3.0f * keyHeight / 2.0f, positions.y + size.y - topRow->size.y - keyHeight - (0.0296f * size.y), 0.0f), pen::Vec2(size.x - (3.0f * keyHeight), keyHeight),
                pen::PEN_TRANSPARENT, 0, this);
            key = new pen::ui::Button(ID_ANY, "q", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (2.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "w", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (2.4f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "e", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (0.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "r", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (1.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "t", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (1.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "y", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.66f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "u", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.66f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "i", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (0.726f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "o", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.66f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "p", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondTopRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.4f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondTopRow->Push(key);
            pen::ui::Scale(secondTopRow, pen::Vec2(1.0f, (size.y / 5.0f) / secondTopRow->size.y), true);

            pen::ui::Item* middleRow = new pen::ui::Div(ID_ANY, pen::Vec3(3.0f * keyHeight / 2.0f, positions.y + size.y - topRow->size.y - secondTopRow->size.y - keyHeight - (0.0444f * size.y), 0.0f), pen::Vec2(size.x - (3.0f * keyHeight), keyHeight),
                pen::PEN_TRANSPARENT, 0, this);
            key = new pen::ui::Button(ID_ANY, "Caps", pen::Vec3(0.0f, 0.0f, 0.0f), 4, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (4.0f * keyHeight);
            key->childItems[0]->size.y *= 2.0f;
            key->childItems[1]->positions.y -= (4.0f * keyHeight);
            key->childItems[1]->size.y *= 2.0f;
            key->childItems[2]->positions.y -= (4.0f * keyHeight);
            key->childItems[2]->size.y *= 2.0f;
            key->childItems[3]->positions.y -= (4.0f * keyHeight);
            key->childItems[3]->size.y *= 2.0f;
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "a", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (1.8f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "s", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (1.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "d", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (2.5f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "f", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (2.0f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "g", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (3.0f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "h", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (3.5f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "j", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.x -= keyHeight;
            key->childItems[0]->positions.y += (0.8f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "k", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (3.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "l", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, middleRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (1.5f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            middleRow->Push(key);
            pen::ui::Scale(middleRow, pen::Vec2(1.0f, (size.y / 5.0f) / middleRow->size.y), true);

            pen::ui::Item* secondBottomRow = new pen::ui::Div(ID_ANY, pen::Vec3(6.0f * keyHeight / 2.0f, positions.y + size.y - topRow->size.y - secondTopRow->size.y - middleRow->size.y - keyHeight - (0.0444f * size.y), 0.0f), pen::Vec2(size.x - (6.0f * keyHeight), keyHeight),
                pen::PEN_TRANSPARENT, 0, this);
            key = new pen::ui::Button(ID_ANY, "z", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y += (0.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "x", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.5f *keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "c", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.6f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "v", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "b", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "n", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= (0.2f * keyHeight);
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            key = new pen::ui::Button(ID_ANY, "m", pen::Vec3(0.0f, 0.0f, 0.0f), 1, pen::PEN_TRANSPARENT, accentColor, secondBottomRow, &OnKeyBoardKeyPressed);
            pen::ui::Scale(key, pen::Vec2(1.0f, keyHeight / key->size.y), true);
            key->childItems[0]->positions.y -= keyHeight;
            key->itemScaling /= (keyHeight / key->size.y);
            secondBottomRow->Push(key);
            pen::ui::Scale(secondBottomRow, pen::Vec2(1.0f, (size.y / 5.0f) / secondBottomRow->size.y), true);

            pen::ui::Item* spaceBar = new pen::ui::Item(ID_ANY, pen::Vec3(size.x / 2.0f - ((size.x / 3.0f) / 2.0f), 
                positions.y + size.y - topRow->size.y - secondTopRow->size.y - middleRow->size.y - secondBottomRow->size.y - keyHeight - (0.0296f * size.y), 0.0f),
                pen::Vec2(size.x / 3.0f, keyHeight), 3, accentColor, this, &OnKeyBoardKeyPressed, true);
            spaceBar->isUI = true;
            spaceBar->isClickable = true;

            Push(topRow);
            Push(secondTopRow);
            Push(middleRow);
            Push(secondBottomRow);
            Push(spaceBar);
        }

        void KeyBoard::KeyBoardReturnKey(pen::ui::Item* item) {
            /*Returns the key that was pressed*/
            if (item->origText == "Caps") {
                (*userOnKeyReturn)('^');
            }
            else if (item->origText == "") {
                (*userOnKeyReturn)(' ');
            }
            else {
                (*userOnKeyReturn)(item->origText[0]);
            }
        }
    }
}
