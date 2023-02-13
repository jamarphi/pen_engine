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
            if (objectTextureName != "") {
                data = pen::CreateSprite(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectAccentColor;
            }
            else {
                data = pen::DrawRect(objectPositions.x, objectPositions.y, objectSize.x, objectSize.y, objectAccentColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(objectPositions);
            SetSize(objectSize);
            parent = objectParent;
            userOnClickCallback = onClickCallback;
            userOnKeyCallback = onKeyCallback;
            isClickable = true;
            isTextEditor = true;

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            origText = initialText;

            shapeType = pen::ui::Shape::QUAD;

            /*----First child item is the container window----*/
            Push(new pen::ui::Item(ID_ANY, pen::Vec3(GetPosition()->x + (0.03f * GetSize()->x), GetPosition()->y + (0.03f * GetSize()->y), GetPosition()->z), 
                pen::Vec2(0.94f * GetSize()->x, 0.94f * GetSize()->y), objectColor, this, onClickCallback));
            containerWindow = childItems[0];

            float* offset = new float[2]{0.0f};
            containerWindow->sliderOffset = offset;
            /*----First child item is the container window----*/

            /*----Initialize the text box within the container window----*/
            containerWindow->Push(new pen::ui::TextBox(ID_ANY, initialText, pen::Vec3(containerWindow->GetPosition()->x,
                containerWindow->GetPosition()->y + containerWindow->GetSize()->y - pen::State::Get()->textScaling, containerWindow->GetPosition()->z),
                100, pen::PEN_TRANSPARENT, objectAccentColor, containerWindow, onClickCallback, "", 0.0f, 0.0f, 1.0f, 1.0f));
            windowTextBox = containerWindow->childItems[0];
            float textBoxScaling = (windowTextBox->GetSize()->x / containerWindow->GetSize()->x > 1.0f || windowTextBox->GetSize()->x / containerWindow->GetSize()->x < 0.5f ? 0.6f : windowTextBox->GetSize()->x / containerWindow->GetSize()->x);
            pen::ui::Scale(windowTextBox, pen::Vec2(textBoxScaling, textBoxScaling), true);
            /*Scale the textbox by itself without updating its itemScaling variable*/
            windowTextBox->SetSize(pen::Vec2(containerWindow->GetSize()->x, windowTextBox->GetSize()->y));
            for (int i = 0; i < windowTextBox->childItems.size() - 1; i+=2) windowTextBox->childItems[i + 1]->isClickable = true;
            /*----Initialize the text box within the container window----*/

            /*----Create text cursor----*/
            containerWindow->Push(new pen::ui::Item(ID_ANY,
                pen::Vec3(containerWindow->GetPosition()->x, containerWindow->GetPosition()->y + containerWindow->GetSize()->y - 20.0f, containerWindow->GetPosition()->z),
                pen::Vec2(inst->textScaling * windowTextBox->itemScaling / 2.0f, 3.0f), objectAccentColor, containerWindow, nullptr));
            textCursor = containerWindow->childItems[1];
            textCursor->AllowActive(false);
            /*----Create text cursor----*/

            /*----Create the vertical scroll bar for this window----*/
            containerWindow->Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(containerWindow->GetPosition()->x + containerWindow->GetSize()->x - (0.05f * containerWindow->GetSize()->x),
                containerWindow->GetPosition()->y, containerWindow->GetPosition()->z),
                pen::Vec2(0.05f * containerWindow->GetSize()->x, containerWindow->GetSize()->y), objectAccentColor, objectColor, 0, 1, containerWindow, nullptr));
            verticalScrollBar = containerWindow->childItems[2];
            verticalScrollBar->AllowActive(false);
            /*----Create the vertical scroll bar for this window----*/

            /*----Create the horizontal scroll bar for this window----*/
            containerWindow->Push(new pen::ui::ScrollBar(ID_ANY, pen::Vec3(containerWindow->GetPosition()->x,
                containerWindow->GetPosition()->y, containerWindow->GetPosition()->z),
                pen::Vec2(containerWindow->GetSize()->x, 0.05f * containerWindow->GetSize()->y), objectAccentColor, objectColor, 1, 0, containerWindow, nullptr));
            horizontalScrollBar = containerWindow->childItems[3];
            horizontalScrollBar->AllowActive(false);
            /*----Create the horizontal scroll bar for this window----*/

            CheckActiveStatus();
        }

        void TextEditor::UpdateTextCursor(pen::ui::Item* textCharacter) {
            /*Set the text cursor based on which text character was clicked*/
            textCursor->SetPosition(pen::Vec3(textCharacter->GetPosition()->x + textCharacter->GetSize()->x + 3.0f, textCharacter->GetPosition()->y - 3.0f, textCharacter->GetPosition()->z));

            cursorIdx = textCharacter->charRowIdx * TEXT_EDITOR_LINE_LENGTH + textCharacter->charColumnIdx + 1;

            windowTextBox->childItems.size() > 0 ? textCursor->AllowActive(true) : textCursor->AllowActive(false);

            pen::Flush();
            pen::ui::Submit();
        }

        void TextEditor::ScrollBarUpdateItemPositions() {
            /*Scroll bar callback*/
            pen::State* inst = pen::State::Get();

            if (prevScrollOffsetX != containerWindow->sliderOffset[0] || prevScrollOffsetY != containerWindow->sliderOffset[1]) {
                editorScrolling = true;
                prevScrollOffsetX = containerWindow->sliderOffset[0];
                prevScrollOffsetY = containerWindow->sliderOffset[1];
            }

            /*For the horizontal scroll bar*/
            float textBoxLength = windowTextBox->GetSize()->x;
            float initialX = windowTextBox->GetPosition()->x;
            float finalX = 0.0f;

            if (containerWindow->sliderOffset[1] == 0.0f && windowTextBox->GetPosition()->x > containerWindow->GetPosition()->x) {
                /*Caps it off at the beginning*/
                finalX = containerWindow->GetPosition()->x;
            }
            else if (containerWindow->sliderOffset[1] == 1.0f && (windowTextBox->GetPosition()->x < containerWindow->GetPosition()->x
                || windowTextBox->GetPosition()->x + windowTextBox->GetSize()->x > containerWindow->GetPosition()->x + containerWindow->GetSize()->x)) {
                /*Caps it off at the end*/
                finalX = containerWindow->GetPosition()->x - windowTextBox->GetSize()->x;
            }
            else {
                finalX = containerWindow->GetPosition()->x + (-1.0f * containerWindow->sliderOffset[1] * textBoxLength);
            }

            /*For the vertical scroll bar*/
            windowTextBox->heightOfScrollParentItems = windowTextBox->GetSize()->y;
            float initialY = windowTextBox->GetPosition()->y + windowTextBox->GetSize()->y;
            float finalY = 0.0f;
            if (containerWindow->sliderOffset[0] == 0.0f && (windowTextBox->GetPosition()->y + windowTextBox->GetSize()->y >= containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling - 0.5f
                || windowTextBox->GetPosition()->y + windowTextBox->GetSize()->y <= containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling + 0.5f)) {
                /*Caps it off at the beginning*/
                finalY = containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling - windowTextBox->GetSize()->y;
            }
            else if (containerWindow->sliderOffset[0] == 1.0f && (windowTextBox->GetPosition()->y >= containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling - 0.5f
                || windowTextBox->GetPosition()->y <= containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling + 0.5f)) {
                /*Caps it off at the end*/
                finalY = containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling + windowTextBox->GetSize()->y;
            }
            else {
                finalY = containerWindow->GetPosition()->y + containerWindow->GetSize()->y - inst->textScaling
                    - (containerWindow->sliderOffset[0] * windowTextBox->heightOfScrollParentItems);
            }
            
            pen::Vec2 translation = pen::Vec2(finalX - initialX, finalY - initialY);
            pen::ui::Translate(windowTextBox, translation, true);

            for (int i = 0; i < windowTextBox->childItems.size(); i++) {
                /*Check to see if text is within bounds*/
                if (windowTextBox->childItems[i]->GetPosition()->x + windowTextBox->childItems[i]->GetSize()->x < containerWindow->GetPosition()->x
                    || windowTextBox->childItems[i]->GetPosition()->x > containerWindow->GetPosition()->x + containerWindow->GetSize()->x
                    || windowTextBox->childItems[i]->GetPosition()->y + windowTextBox->childItems[i]->GetSize()->y < containerWindow->GetPosition()->y
                    || windowTextBox->childItems[i]->GetPosition()->y > containerWindow->GetPosition()->y + containerWindow->GetSize()->y) {
                    windowTextBox->childItems[i]->AllowActive(false);
                }
                else {
                    if(i % 2 != 0) windowTextBox->childItems[i]->AllowActive(true);
                }
            }

            pen::Flush();
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
                    editorScrolling = false;
                }
                return true;
            }
        }

        bool TextEditor::OnDrag(pen::ui::Item* item, double* xPos, double* yPos) {
            if (userOnDragCallback != nullptr) {
                return (*userOnDragCallback)(item, xPos, yPos);
            }
            else {
                if (!editorScrolling) {
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
                        if ((*xPos >= windowTextBox->childItems[i + 1]->GetPosition()->x && *xPos <= windowTextBox->childItems[i + 1]->GetPosition()->x
                            + windowTextBox->childItems[i + 1]->GetSize()->x) && (*yPos >= windowTextBox->childItems[i + 1]->GetPosition()->y && *yPos
                                <= windowTextBox->childItems[i + 1]->GetPosition()->y + windowTextBox->childItems[i + 1]->GetSize()->y)) {
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
                    pen::Flush();
                    pen::ui::Submit();
                }
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
                        if(cursorIdx > 0) cursorIdx--;
                    }
                    else if (key == pen::in::KEYS::RIGHT) {
                        if (cursorIdx < windowTextBox->origText.size()) cursorIdx++;
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
                    if (windowTextBox->GetSize()->y > containerWindow->GetSize()->y / 4.0f * 3.0f) {
                        verticalScrollBar->AllowActive(true);
                    }

                    /*Check to see if horizontal scroll bar is needed*/
                    if (windowTextBox->childItems[windowTextBox->childItems.size() - 1]->GetPosition()->x > containerWindow->GetPosition()->x + containerWindow->GetSize()->x) {
                        horizontalScrollBar->AllowActive(true);
                        if (key != pen::in::KEYS::BACKSPACE) {
                            float textOffset = (containerWindow->GetPosition()->x + containerWindow->GetSize()->x - windowTextBox->childItems[windowTextBox->childItems.size() - 1]->GetSize()->x)
                                - (windowTextBox->childItems[windowTextBox->childItems.size() - 1]->GetPosition()->x);
                            pen::ui::Translate(windowTextBox, pen::Vec2(textOffset, 0.0f), true);
                            containerWindow->sliderOffset[1] += (-1.0f * textOffset * (horizontalScrollBar->GetSize()->x - horizontalScrollBar->childItems[0]->GetSize()->x - horizontalScrollBar->childItems[1]->GetSize()->x)
                                / (TEXT_EDITOR_LINE_LENGTH * windowTextBox->itemScaling * pen::State::Get()->textScaling));
                            if (containerWindow->sliderOffset[1] > 1.0f) {
                                containerWindow->sliderOffset[1] /= 1000.0f;
                                if (containerWindow->sliderOffset[1] > 1.0f) containerWindow->sliderOffset[1] = 1.0f;
                            }
                            pen::ui::Translate(horizontalScrollBar->childItems[2], pen::Vec2(containerWindow->sliderOffset[1]
                                * (horizontalScrollBar->GetSize()->x - horizontalScrollBar->childItems[0]->GetSize()->x - horizontalScrollBar->childItems[1]->GetSize()->x), 0.0f), true);
                            if (horizontalScrollBar->childItems[2]->GetPosition()->x + horizontalScrollBar->childItems[2]->GetSize()->x > horizontalScrollBar->childItems[1]->GetPosition()->x)
                                horizontalScrollBar->childItems[2]->GetPosition()->x = horizontalScrollBar->childItems[1]->GetPosition()->x - horizontalScrollBar->childItems[2]->GetSize()->x;
                        }
                    }

                    if (key == pen::in::KEYS::BACKSPACE && horizontalScrollBar->forceActive) {
                        float textOffset = windowTextBox->itemScaling * pen::State::Get()->textScaling;
                        pen::ui::Translate(windowTextBox, pen::Vec2(textOffset, 0.0f), true);
                        containerWindow->sliderOffset[1] += (-1.0f / 1000.0f * textOffset * (horizontalScrollBar->GetSize()->x - horizontalScrollBar->childItems[0]->GetSize()->x - horizontalScrollBar->childItems[1]->GetSize()->x)
                            / (TEXT_EDITOR_LINE_LENGTH * windowTextBox->itemScaling * pen::State::Get()->textScaling));
                        if (containerWindow->sliderOffset[1] < 0.0f) {
                            containerWindow->sliderOffset[1] = 0.0f;
                        }
                        pen::ui::Translate(horizontalScrollBar->childItems[2], pen::Vec2(-1.0f * containerWindow->sliderOffset[1]
                            * (horizontalScrollBar->GetSize()->x - horizontalScrollBar->childItems[0]->GetSize()->x - horizontalScrollBar->childItems[1]->GetSize()->x), 0.0f), true);
                        if (horizontalScrollBar->childItems[2]->GetPosition()->x < horizontalScrollBar->childItems[0]->GetPosition()->x + horizontalScrollBar->childItems[0]->GetSize()->x
                            || containerWindow->sliderOffset[1] < 0.1f)
                            horizontalScrollBar->childItems[2]->GetPosition()->x = horizontalScrollBar->childItems[0]->GetPosition()->x + horizontalScrollBar->childItems[0]->GetSize()->x;
                    }

                    UpdateTextCursor(windowTextBox->childItems[(cursorIdx > 0 ? cursorIdx * 2 - 1 : 0)]);

                    for (int i = 1; i < windowTextBox->childItems.size(); i+=2) {
                        if (windowTextBox->childItems[i]->GetPosition()->x < containerWindow->GetPosition()->x ||
                            windowTextBox->childItems[i]->GetPosition()->x > containerWindow->GetPosition()->x + containerWindow->GetSize()->x) {
                            windowTextBox->childItems[i]->AllowActive(false);
                        }
                        else {
                            windowTextBox->childItems[i]->AllowActive(true);
                        }
                    }

                    pen::Flush();
                    pen::ui::Submit();
                }
            }
            return true;
        }

        std::string TextEditor::ConvertChar(const int& character) {
            /*Convert char to ascii byte code*/
            if (character == pen::in::KEYS::ENTER) {
                pen::ui::Translate(windowTextBox, pen::Vec2(containerWindow->GetPosition()->x - windowTextBox->GetPosition()->x, 0.0f), true);
                horizontalScrollBar->childItems[2]->GetPosition()->x = horizontalScrollBar->childItems[0]->GetPosition()->x + horizontalScrollBar->childItems[0]->GetSize()->x;
                containerWindow->sliderOffset[1] = 0.0f;
                return "\n";
            }
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
