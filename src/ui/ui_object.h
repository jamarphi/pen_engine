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
#include "../core/color.h"
#include "position.h"
#include "text_character_item.h"

namespace pen {
    namespace ui {
        class UIObject : public pen::ui::Item {
            /*UI objects can be part of two groups: text boxes, and ui items that contain symbols instead of text*/
        public:
            UIObject();

            UIObject(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), 
                pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(pen::ui::Item*, int, int) = nullptr, std::string objectTextureName = "",
                float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);

        public:
            void GetTextCyclesNum(const std::string& userText);

            void SetTextCycles(std::string userText);

            int IndexOfPrevSpace(const std::string& userText, int idx, bool wordChecked = false);

            int IndexOfNextSpace(const std::string& userText, int idx, int startPoint);

            bool WordLongerThanLength(std::vector<std::string> words, int idx, int startPoint, int* wordIdx);

            std::vector<std::string> Split(std::string line, std::string delimiter);

            void GenerateText(const std::string& textStr, pen::Vec3 textPositions, bool firstLine, const unsigned int& rowPos);

            std::string ProcessText(std::string textStr);

            std::string StringExtend(char character, unsigned int count);
        };
    }
}