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
#include "ui_object.h"

namespace pen {
    namespace ui {
        UIObject::UIObject() { isUI = true; }

        UIObject::UIObject(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, unsigned int objectShapeType, pen::Vec4 objectColor,
            pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), bool objectIsFixed, std::string objectTextureName,
            float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            id = objectId;
            isUI = true;
            positions = objectPositions;
            size = objectSize;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            isFixed = objectIsFixed;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            bufferPositions = pen::ui::Shape::GetItemBatchData(positions, size, objectShapeType, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = objectShapeType;

            color = objectColor;

            /*Checks to make sure the item is on the screen to be rendered*/
            CheckActiveStatus();
        }

        UIObject::~UIObject() {
            bufferPositions.clear();
            textureName = "";
        }

        void UIObject::GetTextCyclesNum(const std::string& userText) {
            /*Grabs the number of cycles based on the amount of text lines there will be for a given text box*/
            unsigned int userTextSize = userText.length();
            if (userTextSize > 0) {
                cycles = (userTextSize / textLength > 0 ? userTextSize / textLength : 1) + (userTextSize > textLength && userTextSize % textLength > 0 ? 1 : 0);
            }
        }

        void UIObject::GenerateText(const std::string& textStr, pen::Vec3 textPositions, bool firstLine, const unsigned int& rowPos) {
            /*Creates each text character quad for a given line of text*/
            std::string::const_iterator c;
            float scale = 1.0f;
            float startPos = textPositions.x;
            std::string tempStr = "";
            unsigned int counter = 0;
            pen::State* inst = pen::State::Get();
            for (c = textStr.begin(); c != textStr.end(); c++)
            {
                Character ch = pen::State::Get()->characters.Find(*c)->second;

                float xpos = startPos + (itemScaling * ch.bearing.x) * scale;
                float ypos = textPositions.y - (ch.size.y - (itemScaling * ch.bearing.y)) * scale;
                float zpos = textPositions.z;

                float w = ch.size.x * scale;
                float h = ch.size.y * scale;

                /*Set the width of the text box*/
                if (firstLine) size.x += (w + (itemScaling * ch.bearing.x) + 0.5f);

                ch.advance *= itemScaling;
                startPos += (ch.advance >> 6) * scale;
                tempStr = ch.character;
                if (!(itemCount == 9999 || (itemCount >= 9980 && isTextEditor))) {
                    if (isTextEditor) {
                        childItems.push_back(new pen::ui::Item(ID_ANY, pen::Vec3(xpos, ypos, zpos),
                            pen::Vec2(w + (5.0f * itemScaling), inst->textScaling), pen::ui::Shape::QUAD, pen::PEN_BLUE, this, nullptr, isFixed));
                        childItems[childItems.size() - 1]->isListItem = isListItem ? true : false;
                        childItems[childItems.size() - 1]->AllowActive(false);
                    }
                        childItems.push_back(new pen::ui::TextCharacterItem(ID_ANY, pen::Vec3(xpos, ypos, zpos), pen::Vec2(w, h), this, userOnClickCallback, textColor, isFixed, tempStr, rowPos, counter));
                        childItems[childItems.size() - 1]->isListItem = isListItem ? true : false;
                        itemCount++;

                        /*Scale text if necessary*/
                        pen::ui::Scale(childItems[childItems.size() - 1], pen::Vec2(itemScaling, itemScaling), true);
                        counter++;
                }
                else {
                    break;
                    break;
                }
            }
        }

        void UIObject::SetTextCycles(std::string userText) {
            userText = ProcessText(userText);
            unsigned int userTextSize = userText.length();
            int startingPoint = 0;
            bool prevWordChecked = false;
            bool firstLine = true;
            int counter = 0;
            pen::State* stateInst = pen::State::Get();

            while (startingPoint < userTextSize) {
                /*Grabs the position for a given line of text based on the previous line position and the text box container positions*/
                pen::Vec3 position = Vec3(positions.x,
                    positions.y + size.y - (2.4f * textPadding) - (counter * stateInst->textScaling * 1.3f * itemScaling), positions.z);

                if (userTextSize <= textLength) { 
                    /*If there is only one text line for this text box, it generates it right away*/
                    GenerateText(userText, position, firstLine, counter);
                    firstLine = false;
                    break;
                }

                if (userText.at(startingPoint) == ' ') startingPoint++;


                int wordIdx = 0;
                
                /*Grabs the index used for splitting text line into separate words*/
                int splitIdx = startingPoint + textLength > userTextSize ? (startingPoint + (userTextSize % textLength) - 3) : (startingPoint + textLength - 1);
                if (splitIdx > userTextSize) break;

                /*Splits text line into separate words*/
                std::vector<std::string> words = Split(userText.at(splitIdx) == ' ' ? userText.substr(startingPoint, textLength)
                    : (userText.substr(startingPoint, IndexOfNextSpace(userText, splitIdx, startingPoint) - startingPoint)), " ");
                
                if (WordLongerThanLength(words, IndexOfNextSpace(userText, startingPoint, startingPoint), startingPoint, &wordIdx)) {
                    /*For words longer than the max length of the row*/
                    startingPoint--;
                    userText.insert(startingPoint + textLength + 1, "-");
                    GenerateText(userText.substr(startingPoint + wordIdx, startingPoint + textLength - wordIdx), position, firstLine, counter);
                    firstLine = false;
                    startingPoint += textLength + 1;
                    prevWordChecked = false;

                }
                else {
                    int lastCharOfRowIdx = startingPoint + textLength > userTextSize ? (startingPoint + userTextSize % textLength - 2) : (startingPoint + textLength - 1);
                    /*If the last character of the row is not a space character*/
                    if (userText.at(lastCharOfRowIdx) != ' ') {
                        /*If the last word of this row goes past max length of the row*/
                        if (IndexOfNextSpace(userText, lastCharOfRowIdx, startingPoint) > lastCharOfRowIdx + 1) {
                            /*Place whole word that goes past the text length on a new line*/
                            int prevSpaceIdx = IndexOfPrevSpace(userText, startingPoint + textLength);
                            GenerateText(userText.substr(startingPoint, textLength - (textLength - (prevSpaceIdx % textLength)) + 1), position, firstLine, counter);
                            firstLine = false;
                            startingPoint += (prevSpaceIdx % textLength == 0 ? prevSpaceIdx % textLength + 1 : prevSpaceIdx % textLength);
                            prevWordChecked = false;
                        }
                        else {
                            /*The last word of this row fits nicely within the max length of this row*/
                            GenerateText(userText.substr(startingPoint, textLength), position, firstLine, counter);
                            firstLine = false;
                            startingPoint += textLength;
                            prevWordChecked = false;
                        }
                    }
                    else {
                        /*If the last character of the row is a space, we just move to the next row*/
                        GenerateText(userText.substr(startingPoint, textLength), position, firstLine, counter);
                        firstLine = false;
                        startingPoint += textLength;
                        prevWordChecked = false;
                    }
                }
                counter++;
            }
        }

        int UIObject::IndexOfPrevSpace(const std::string& userText, int idx, bool wordChecked) {
            /*Grabs the index of the previous space character from a given position*/
            if (wordChecked) return idx;

            int prevSpace = userText.substr(0, idx).find_last_of(" ");
            return (prevSpace < 0 ? 0 : prevSpace);
        }

        int UIObject::IndexOfNextSpace(const std::string& userText, int idx, int startPoint) {
            /*Grabs the index of the next space character from a given position*/
            int nextSpace = userText.substr(idx).find_first_of(" ");
            if (nextSpace < 0) {
                return userText.length() - 1;
            }
            else {
                return startPoint + (idx - startPoint) + nextSpace;
            };
        }

        bool UIObject::WordLongerThanLength(std::vector<std::string> words, int idx, int startPoint, int* wordIdx) {
            /*Checks if a word is longer than the max number of text characters per line*/
            int* counter = 0;
            for (auto& word : words) {
                if (word.length() > textLength) {
                    wordIdx = (startPoint / textLength) + counter;
                    return true;
                }
                counter += word.length() + 1;
            }
            return false;
        }

        std::vector<std::string> UIObject::Split(std::string line, std::string delimiter) {
            /*Splits a line of text into separate words*/
            int start = 0;
            int end = line.find(delimiter);
            std::vector<std::string> wordList;
            while (end != -1) {
                wordList.push_back(line.substr(start, end - start));
                start = end + delimiter.size();
                end = line.find(delimiter, start);
            }
            return wordList;
        }

        std::string UIObject::ProcessText(std::string textStr) {
            /*Removes characters that are not part of the character set*/
            bool keepGoing = true;
            while (keepGoing) {
                keepGoing = false;
                std::string::const_iterator c;
                int counter = 0;
                for (c = textStr.begin(); c != textStr.end(); c++)
                {
                    
                    if (*c < 32 && *c != 9 && *c != 10) {
                        /*The character is not a normal character and is not a tab or enter*/
                        if (counter == 0) {
                            textStr = textStr.substr(1);
                        }
                        else {
                            textStr = textStr.substr(0, counter) + textStr.substr(counter + 1, textStr.size() - (counter + 1));
                        }
                        keepGoing = true;
                        break;
                    }
                    else if (*c == 9) {
                        /*Replace tab with four spaces*/
                        if (counter == 0) {
                            textStr = "    " + textStr.substr(1);
                        }
                        else {
                            textStr = textStr.substr(0, counter) + "    " + textStr.substr(counter + 1, textStr.size() - (counter + 1));
                        }
                        keepGoing = true;
                        break;
                    }
                    else if (*c == 10) {
                        /*Replace enter with enough spaces to reach end of line*/
                        unsigned int spaces = textLength - (counter % textLength);
                        if (counter == 0) {
                            textStr = StringExtend(' ', spaces) + textStr.substr(1);
                        }
                        else {
                            textStr = textStr.substr(0, counter) + StringExtend(' ', spaces) + textStr.substr(counter + 1, textStr.size() - (counter + 1));
                        }
                        keepGoing = true;
                        break;
                    }
                    counter++;
                }
            }
            return textStr;
        }

        std::string UIObject::StringExtend(char character, unsigned int count) {
            std::string tempStr;
            for (int i = 0; i < count; i++) {
                tempStr += character;
            }
            return tempStr;
        }
    }
}
