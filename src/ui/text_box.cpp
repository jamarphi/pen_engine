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
#include "text_box.h"

namespace pen {
    namespace ui {
        TextBox::TextBox() { isUI = true; }

        TextBox::TextBox(uint32_t objectId, std::string userText, pen::Vec3 objectPositions, int objectTextLength, float objectHeight, pen::Vec4 objectColor, pen::Vec4 objectTextColor, 
            pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), bool objectIsFixed, std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY, bool textEditor) {
            /*Constructor for user supplied height*/
            isUI = true;
            id = objectId;
            positions = objectPositions; 
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = objectIsFixed;
            origText = userText;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = false;
            if (objectParent != nullptr) isListItem = objectParent->isList ? true : false;

            color = objectColor;
            textColor = objectTextColor;
            textureName = objectTextureName;
            isTextEditor = textEditor;

            /*Sets the cycles variable for how many lines of text there is*/
            textLength = objectTextLength;
            size.x = 0.0f;
            GetTextCyclesNum(userText);
            size.y = objectHeight;

            /*The width of the text box gets updated here*/
            SetTextCycles(userText);

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            /*For empty text boxes*/
            if (size.x == 0.0f) size.x = 50.0f;
            if (size.y == 0.0f) size.y = pen::State::Get()->textScaling;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            /*CheckActiveStatus() occurs in CombineChildBuffers() as well*/
            CombineChildBuffers();
        }

        TextBox::TextBox(uint32_t objectId, std::string userText, pen::Vec3 objectPositions, int objectTextLength, pen::Vec4 objectColor, pen::Vec4 objectTextColor, 
            pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), bool objectIsFixed, std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY, bool textEditor) {
            /*Regular constructor*/
            isUI = true;
            id = objectId;
            positions = objectPositions; 
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = objectIsFixed;
            origText = userText;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = false;

            color = objectColor;
            textColor = objectTextColor;
            textureName = objectTextureName;
            isTextEditor = textEditor;

            /*Sets the cycles variable for how many lines of text there is*/
            textLength = objectTextLength;
            size.x = 0.0f;
            GetTextCyclesNum(userText);
            size.y = pen::State::Get()->textScaling * cycles + (cycles > 1 ? pen::State::Get()->textScaling : 0);

            /*The width of the text box gets updated here*/
            SetTextCycles(userText);

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            /*For empty text boxes*/
            if (size.x == 0.0f) size.x = 50.0f;
            if (size.y == 0.0f) size.y = pen::State::Get()->textScaling;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            /*CheckActiveStatus() occurs in CombineChildBuffers() as well*/
            CombineChildBuffers();
        }

        TextBox::~TextBox() {
            bufferPositions.clear();
            textureName = "";
        }

        void TextBox::SetTextLength() {
            /*Sets the max amount of text characters per line*/
            textLength = (unsigned int)((size.x / pen::State::Get()->textScaling - 1) * 2);
            if (textLength <= 0) textLength = 1;
        }

        void TextBox::SetTextColor(pen::Vec4 itemTextColor, int pos) {
            if (pos == -1) {
                /*Update color for all child items, all child items are text*/
                for (auto& item : childItems) item->SetColor(itemTextColor);
            }
            else {
                /*Update color for a specific child item*/
                childItems[pos]->SetColor(itemTextColor);
            }
        }

        void TextBox::UpdateText(const std::string& userText) {
            /*Remove any previous text*/
            for (int i = 0; i < childItems.size(); i++) {
                delete childItems[i];
            }
            childItems.clear();
            itemCount = 0;
            
            origText = userText;

            /*Updates the cycles variable for how many lines of text there is*/
            GetTextCyclesNum(userText);
            size.y = (pen::State::Get()->textScaling * cycles + (cycles > 1 ? pen::State::Get()->textScaling : 0)) * itemScaling;

            size.x = 0.0f;
            /*Updates the text, the width of the text box gets updated here*/
            SetTextCycles(userText);

            /*For empty text boxes*/
            if (size.x == 0.0f) size.x = 50.0f;
            if (size.y == 0.0f) size.y = pen::State::Get()->textScaling;
        }
    }
}