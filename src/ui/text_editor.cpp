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
#include "text_editor.h"

namespace pen {
	namespace ui {
        TextEditor::TextEditor() { isUI = true; }

        TextEditor::TextEditor(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, pen::Vec4 objectColor,
            pen::Vec4 objectAccentColor, pen::ui::Item* objectParent, bool (*onClickCallback)(pen::ui::Item*, int, int), bool (*onKeyCallback)(pen::ui::Item*, int, int), 
            std::string initialText, std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY, float itemTexCoordEndX, float itemTexCoordEndY) {
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            positions = objectPositions;
            size = objectSize;
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            userOnKeyCallback = onKeyCallback;
            isFixed = true;
            angles = pen::Vec3(0.0f, 0.0f, 0.0f);
            isClickable = true;
            isSingular = true;
            isTextEditor = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            origText = initialText;

            bufferPositions = pen::ui::Shape::GetBatchPosition(positions, size, pen::ui::Shape::QUAD, objectColor, nullptr, 0.0f, 0.0f, 0.0f, GetAssetId(),
                itemTexCoordStartX, itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);

            shapeType = pen::ui::Shape::QUAD;

            color = objectAccentColor;

            /*----First child item is the container window----*/
            Push(new pen::ui::Item(ID_ANY, pen::Vec3(positions.x + (0.03f * size.x), positions.y + (0.03f * size.y), positions.z), 
                pen::Vec2(0.94f * size.x, 0.94f * size.y), pen::ui::Shape::QUAD, objectColor, this, onClickCallback, true));
            containerWindow = childItems[0];

            float* offset = new float[2];
            containerWindow->sliderOffset = offset;
            /*----First child item is the container window----*/

            /*----Initialize the text box within the container window----*/
            containerWindow->Push(new pen::ui::TextBox(ID_ANY, initialText, pen::Vec3(containerWindow->positions.x,
                containerWindow->positions.y + containerWindow->size.y - pen::State::Get()->textScaling, containerWindow->positions.z),
                100, pen::PEN_TRANSPARENT, objectAccentColor, containerWindow, onClickCallback, true, "", 0.0f, 0.0f, 1.0f, 1.0f, true));
            windowTextBox = containerWindow->childItems[0];
            float textBoxScaling = (windowTextBox->size.x / containerWindow->size.x > 1.0f || windowTextBox->size.x / containerWindow->size.x < 0.5f ? 0.6f : windowTextBox->size.x / containerWindow->size.x);
            pen::ui::Scale(windowTextBox, pen::Vec2(textBoxScaling, textBoxScaling), true);
            /*Scale the textbox by itself without updating its itemScaling variable*/
            windowTextBox->size.x = containerWindow->size.x;
            for (int i = 0; i < windowTextBox->childItems.size(); i+=2) windowTextBox->childItems[i + 1]->isClickable = true;
            /*----Initialize the text box within the container window----*/

            /*----Create text cursor----*/
            containerWindow->Push(new pen::ui::Item(ID_ANY,
                pen::Vec3(containerWindow->positions.x, containerWindow->positions.y + containerWindow->size.y - 20.0f, containerWindow->positions.z),
                pen::Vec2(inst->textScaling * windowTextBox->itemScaling / 2.0f, 3.0f), pen::ui::Shape::QUAD, objectAccentColor, containerWindow, nullptr, true));
            textCursor = containerWindow->childItems[1];
            textCursor->AllowActive(false);
            /*----Create text cursor----*/

            /*----Create the vertical scroll bar for this window----*/
            containerWindow->Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(containerWindow->positions.x + containerWindow->size.x - (0.05f * containerWindow->size.x),
                containerWindow->positions.y, containerWindow->positions.z),
                pen::Vec2(0.05f * containerWindow->size.x, containerWindow->size.y), objectAccentColor, objectColor, 0, 1, containerWindow, nullptr));
            verticalScrollBar = containerWindow->childItems[2];
            verticalScrollBar->AllowActive(false);
            /*----Create the vertical scroll bar for this window----*/

            /*----Create the horizontal scroll bar for this window----*/
            containerWindow->Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(containerWindow->positions.x,
                containerWindow->positions.y, containerWindow->positions.z),
                pen::Vec2(containerWindow->size.x, 0.05f * containerWindow->size.y), objectAccentColor, objectColor, 1, 0, containerWindow, nullptr));
            horizontalScrollBar = containerWindow->childItems[3];
            horizontalScrollBar->AllowActive(false);
            /*----Create the horizontal scroll bar for this window----*/

            CheckActiveStatus();
        }

        TextEditor::~TextEditor() {}

        void TextEditor::UpdateTextCursor(pen::ui::Item* textCharacter) {
            /*Set the text cursor based on which text character was clicked*/
            textCursor->positions.x = textCharacter->positions.x + textCharacter->size.x + 3.0f;
            textCursor->positions.y = textCharacter->positions.y - 3.0f;
            textCursor->positions.z = textCharacter->positions.z;

            cursorIdx = textCharacter->charRowIdx * TEXT_EDITOR_LINE_LENGTH + textCharacter->charColumnIdx + 1;

            windowTextBox->childItems.size() > 0 ? textCursor->AllowActive(true) : textCursor->AllowActive(false);

            pen::ui::Submit();
        }

        void TextEditor::ScrollBarUpdateItemPositions() {
            /*Scroll bar callback*/
            pen::State* inst = pen::State::Get();

            ///*For the horizontal scroll bar*/
            ////float textBoxLength = windowTextBox->size.x;
            //float textBoxLength = 10 * inst->textScaling; //TESTING
            //float initialX = windowTextBox->positions.x;
            //float finalX = 0.0f;
            //auto t = containerWindow->sliderOffset[1];
            //if (containerWindow->sliderOffset[1] == 0.0f && windowTextBox->positions.x > containerWindow->positions.x) {
            //    /*Caps it off at the beginning*/
            //    finalX = containerWindow->positions.x;
            //}
            //else if (containerWindow->sliderOffset[1] == 1.0f && (windowTextBox->positions.x < containerWindow->positions.x
            //    || windowTextBox->positions.x + windowTextBox->size.x > containerWindow->positions.x + containerWindow->size.x)) {
            //    /*Caps it off at the end*/
            //    finalX = containerWindow->positions.x - windowTextBox->size.x;
            //}
            //else {
            //    finalX = windowTextBox->positions.x + (containerWindow->sliderOffset[1] * textBoxLength);
            //}

            /*For the vertical scroll bar*/
            windowTextBox->heightOfScrollParentItems = windowTextBox->size.y;
            float initialY = windowTextBox->positions.y + windowTextBox->size.y;
            float finalY = 0.0f;
            if (containerWindow->sliderOffset[0] == 0.0f && (windowTextBox->positions.y + windowTextBox->size.y >= containerWindow->positions.y + containerWindow->size.y - inst->textScaling - 0.5f
                || windowTextBox->positions.y + windowTextBox->size.y <= containerWindow->positions.y + containerWindow->size.y - inst->textScaling + 0.5f)) {
                /*Caps it off at the beginning*/
                finalY = containerWindow->positions.y + containerWindow->size.y - inst->textScaling - windowTextBox->size.y;
            }
            else if (containerWindow->sliderOffset[0] == 1.0f && (windowTextBox->positions.y >= containerWindow->positions.y + containerWindow->size.y - inst->textScaling - 0.5f
                || windowTextBox->positions.y <= containerWindow->positions.y + containerWindow->size.y - inst->textScaling + 0.5f)) {
                /*Caps it off at the end*/
                finalY = containerWindow->positions.y + containerWindow->size.y - inst->textScaling + windowTextBox->size.y;
            }
            else {
                finalY = containerWindow->positions.y + containerWindow->size.y - inst->textScaling
                    - (containerWindow->sliderOffset[0] * windowTextBox->heightOfScrollParentItems);
            }
            
            pen::Vec3 translation = pen::Vec3(0.0f, finalY - initialY, 0.0f);
            pen::ui::Translate(windowTextBox, translation, true, true, false);

            for (int i = 0; i < windowTextBox->childItems.size(); i++) {
                /*Check to see if text is within bounds*/
                if (windowTextBox->childItems[i]->positions.x + windowTextBox->childItems[i]->size.x < containerWindow->positions.x
                    || windowTextBox->childItems[i]->positions.x > containerWindow->positions.x + containerWindow->size.x
                    || windowTextBox->childItems[i]->positions.y + windowTextBox->childItems[i]->size.y < containerWindow->positions.y
                    || windowTextBox->childItems[i]->positions.y > containerWindow->positions.y + containerWindow->size.y) {
                    windowTextBox->childItems[i]->AllowActive(false);
                }
                else {
                    windowTextBox->childItems[i]->AllowActive(true);
                }
            }

            pen::ui::Submit();
        }

        bool TextEditor::OnClick(pen::ui::Item* item, const int& button, const int& action) {
            if (userOnClickCallback != nullptr) {
                return (*userOnClickCallback)(this, button, action);
            }
            else {
                if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
                    pen::State::SetDraggable((void*)item);
                    pen::State::SetKeyable((void*)item);
                }
                else if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::RELEASED) {
                    textEditorHighlightActive = false;
                }
                return true;
            }
        }

        bool TextEditor::OnDrag(pen::ui::Item* item, double* xPos, double* yPos) {
            if (userOnDragCallback != nullptr) {
                return (*userOnDragCallback)(item, xPos, yPos);
            }
            else {
                /*Highlight text, every even child item in this text box will be a highlighting quad and every odd child item will be the text character*/
                bool highlightBetween = false;
                int prevCharRow = -1;
                int prevCharColumn = -1;
                int firstHighlightedCharRow = -1;
                int firstHighlightedCharColumn = -1;
                bool isTextHighlighted = false;
                textHighlighted = false;
                if (!textEditorHighlightActive) {
                    /*Remove previous highlighted text*/
                    for (int i = 0; i < windowTextBox->childItems.size(); i += 2) {
                        windowTextBox->childItems[i]->AllowActive(false);
                    }
                    textEditorHighlightActive = true;
                }

                int counter = 0;
                for (int i = 0; i < windowTextBox->childItems.size(); i += 2) {
                    if ((*xPos >= windowTextBox->childItems[i + 1]->positions.x && *xPos <= windowTextBox->childItems[i + 1]->positions.x
                        + windowTextBox->childItems[i + 1]->size.x) && (*yPos >= windowTextBox->childItems[i + 1]->positions.y && *yPos
                            <= windowTextBox->childItems[i + 1]->positions.y + windowTextBox->childItems[i + 1]->size.y)) {
                        /*Highlight this character and update the text cursor position*/
                        windowTextBox->childItems[i]->AllowActive(true);

                        isTextHighlighted = true;

                        /*Check if something is highlighted on a previous row*/
                        if (highlightBetween) {
                            for (int j = 2 * prevCharRow * TEXT_EDITOR_LINE_LENGTH + prevCharColumn; j <
                                2 * windowTextBox->childItems[i + 1]->charRowIdx * TEXT_EDITOR_LINE_LENGTH + windowTextBox->childItems[i + 1]->charColumnIdx + 1; j++) {
                                if (!windowTextBox->childItems[j]->forceActive) windowTextBox->childItems[j]->AllowActive(true);
                            }
                        }
                        break;
                    }
                    else if (windowTextBox->childItems[i]->forceActive) {
                        highlightBetween = true;
                        if (prevCharColumn == -1 && prevCharRow == -1) {
                            firstHighlightedCharColumn = windowTextBox->childItems[i + 1]->charColumnIdx;
                            firstHighlightedCharRow = windowTextBox->childItems[i + 1]->charRowIdx;
                        }
                        prevCharColumn = windowTextBox->childItems[i + 1]->charColumnIdx;
                        prevCharRow = windowTextBox->childItems[i + 1]->charRowIdx;
                        isTextHighlighted = true;
                    }
                    counter += 2;
                }

                /*Loop through to fill in any missing highlights*/
                if (firstHighlightedCharColumn > -1 && firstHighlightedCharRow > -1 && counter < windowTextBox->childItems.size() - 2) {
                    for (int k = 2 * firstHighlightedCharRow * TEXT_EDITOR_LINE_LENGTH + firstHighlightedCharColumn;
                        k < counter; k += 2) {
                        windowTextBox->childItems[k]->AllowActive(true);
                    }
                }

                if (isTextHighlighted) textHighlighted = true;
                pen::ui::Submit();
                return true;
            }
        }

        bool TextEditor::OnKey(pen::ui::Item* item, const int& key, const int& action) {
            if (userOnKeyCallback != nullptr) {
                return (*userOnKeyCallback)(this, key, action);
            }
            else {
                if (action == pen::in::KEYS::PRESSED) {
                    int selectedTextStart = -1;
                    int selectedTextEnd = -1;
                    if (textHighlighted) {
                        /*Grab start and end indices for highlighted text*/
                        for (int i = 0; i < windowTextBox->childItems.size(); i += 2) {
                            if (windowTextBox->childItems[i]->forceActive && selectedTextStart == -1) {
                                /*Grab the starting point*/
                                selectedTextStart = (i + 1) / 2;
                            }
                            else if (windowTextBox->childItems[i]->forceActive) {
                                selectedTextEnd = (i + 1) / 2;
                            }
                        }
                    }
                    
                    if (key == pen::in::KEYS::DELETE || key == pen::in::KEYS::BACKSPACE) {
                        /*If user hit delete or backspace, remove selected text or character*/
                        if (selectedTextStart != -1 && selectedTextEnd != -1) {
                            if (selectedTextStart == 0) {
                                windowTextBox->origText = windowTextBox->origText.substr(selectedTextEnd + 1);
                            }
                            else {
                                windowTextBox->origText = windowTextBox->origText.substr(0, selectedTextStart) + windowTextBox->origText.substr(selectedTextEnd + 1);
                            }
                        }
                        else {
                            if (key == pen::in::KEYS::DELETE) {
                                if (cursorIdx < windowTextBox->origText.size()) {
                                    windowTextBox->origText = windowTextBox->origText.substr(0, cursorIdx) + windowTextBox->origText.substr(cursorIdx + 1);
                                }
                            }
                            else {
                                /*Backspace*/
                                if (cursorIdx > 1) {
                                    windowTextBox->origText = windowTextBox->origText.substr(0, cursorIdx - 1) + windowTextBox->origText.substr(cursorIdx);
                                    cursorIdx--;
                                }
                            }
                        }
                    }
                    else if (key == pen::in::KEYS::LEFT) {
                        cursorIdx--;
                    }
                    else if (key == pen::in::KEYS::RIGHT) {
                        cursorIdx++;
                    }
                    else if (key == pen::in::KEYS::DOWN) {
                        cursorIdx += TEXT_EDITOR_LINE_LENGTH;
                        if (cursorIdx > windowTextBox->childItems.size() / 2) cursorIdx -= TEXT_EDITOR_LINE_LENGTH;
                    }
                    else if (key == pen::in::KEYS::UP) {
                        cursorIdx -= TEXT_EDITOR_LINE_LENGTH;
                        if (cursorIdx < 0) cursorIdx += TEXT_EDITOR_LINE_LENGTH;
                    }
                    else {
                        /*Key is valid to be added to string*/
                        if (selectedTextStart != -1 && selectedTextEnd != -1) {
                            if (selectedTextStart == 0) {
                                windowTextBox->origText = ConvertChar(key) + windowTextBox->origText.substr(selectedTextEnd + 1);
                            }
                            else {
                                windowTextBox->origText = windowTextBox->origText.substr(0, selectedTextStart) + ConvertChar(key) + windowTextBox->origText.substr(selectedTextEnd + 1);
                            }
                        }
                        else if (cursorIdx == 0) {
                            windowTextBox->origText = ConvertChar(key) + windowTextBox->origText;
                        }
                        else {
                            if (cursorIdx >= windowTextBox->origText.size()) {
                                /*Cursor is at the end*/
                                windowTextBox->origText += ConvertChar(key);
                            }
                            else {
                                windowTextBox->origText = windowTextBox->origText.substr(0, cursorIdx) + ConvertChar(key) + windowTextBox->origText.substr(cursorIdx);
                            }
                        }
                        cursorIdx++;
                    }

                    windowTextBox->UpdateText(windowTextBox->origText);
                    for (int i = 0; i < windowTextBox->childItems.size(); i += 2) windowTextBox->childItems[i + 1]->isClickable = true;

                    /*Check to see if vertical scroll bar is needed*/
                    if (windowTextBox->size.y > containerWindow->size.y / 4.0f * 3.0f) {
                        verticalScrollBar->AllowActive(true);
                    }
                    else {
                        verticalScrollBar->AllowActive(false);
                    }

                    UpdateTextCursor(windowTextBox->childItems[(cursorIdx > 0 ? cursorIdx * 2 - 1 : 0)]);
                    pen::ui::Submit();
                }
            }
        }

        std::string TextEditor::ConvertChar(const int& character) {
            /*Convert char to ascii bytecode*/
            if (character == pen::in::KEYS::ENTER) return "\n";
            if (character == pen::in::KEYS::TAB) return "\t";
            if (character == pen::in::KEYS::SPACE) return " ";

            pen::State* inst = pen::State::Get();
            for (int i = 0; i < inst->asciiMap.Size(); i++) {
                if (inst->asciiMap.items[i].second == character) return inst->asciiMap.items[i].first;
            }
            return "";
        }
	}
}