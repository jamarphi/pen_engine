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
#include "button.h"

namespace pen {
    namespace ui {
        Button::Button() {
            isUI = true;
            isClickable = true;
        }

        Button::Button(uint32_t objectId, std::string userText, pen::Vec3 objectPositions, int objectTextLength, pen::Vec4 objectColor, pen::Vec4 objectTextColor, 
            pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            /*Regular constructor*/
            isUI = true;
            id = objectId;
            SetPosition(objectPositions);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            origText = userText;
            isClickable = true;
            if (objectParent != nullptr) isListItem = objectParent->isList ? true : false;

            textColor = objectTextColor;
            textureName = objectTextureName;

            /*Sets the cycles variable for how many lines of text there is*/
            textLength = objectTextLength;
            GetTextCyclesNum(userText);
            float objectHeight = pen::State::Get()->textScaling * cycles + (cycles > 1 ? pen::State::Get()->textScaling : 0);
            data = pen::DrawRect(objectPositions.x, objectPositions.y, 300, objectHeight, objectColor);
            SetSize(pen::Vec2(300.0f, objectHeight));

            /*The width of the text box gets updated here*/
            SetTextCycles(userText);

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            /*CheckActiveStatus() occurs in CombineChildBuffers() as well*/
            CombineChildBuffers();
        }
    }
}
