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
#include "file_modal.h"

#ifndef __PEN_MOBILE__
std::string ParseFileModalFilesList(pen::ui::Item* item) {
    /*Return a formatted path of the current directory*/
    std::string currentPaths = "";

    for (int i = 0; i < item->fileModalList.size(); i++) {
        if (i == item->fileModalList.size() - 1) {
            currentPaths = currentPaths + item->fileModalList[i];
        }
        else {
            currentPaths = currentPaths + item->fileModalList[i] + ";";
        }
    }

    return currentPaths;
}

std::string CombineFileModalPathPrefix(pen::ui::Item* item) {
    /*Return a formatted path of the current directory*/
    std::string currentDir = "";

    for (int i = 0; i < item->fileModalPathPrefix.size(); i++) {
        if (i == 0) {
            currentDir = item->fileModalPathPrefix[i];
        }
        else {
            currentDir = currentDir + item->fileModalPathPrefix[i] + "/";
        }
    }

    if (currentDir.length() == 0) currentDir = "C://";
    return currentDir;
}

bool OnFileModalItemSelected(pen::ui::Item* item, int button, int action) {
    /*Update the callback path*/
    if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
        if (item->origText == "..") {
            /*Go out one directory*/
            if (item->parent->parent->fileModalPathPrefix.size() > 1) {
                item->parent->parent->fileModalPathPrefix.pop_back();
                item->parent->parent->UpdateFileModalListing(CombineFileModalPathPrefix(item->parent->parent));
            }
        }
        else {
            /*Go in a directory or select file depending on intent*/
            if (item->fileModalIntent == pen::ui::FILE_INTENT::SELECT_DIRECTORY) {
                if (item->origText.find(".") == std::string::npos) {
                    std::string newDir = item->origText.find("/") != std::string::npos ? item->origText.substr(0, item->origText.find("/")) : item->origText;
                    item->parent->parent->fileModalPathPrefix.push_back(newDir);
                    item->parent->parent->UpdateFileModalListing(CombineFileModalPathPrefix(item->parent->parent));
                }
            }
            else {
                if (item->origText.find(".") == std::string::npos) {
                    std::string newDir = item->origText.find("/") != std::string::npos ? item->origText.substr(0, item->origText.find("/")) : item->origText;
                    item->parent->parent->fileModalPathPrefix.push_back(newDir);
                    item->parent->parent->UpdateFileModalListing(CombineFileModalPathPrefix(item->parent->parent));
                }
                else {
                    item->parent->parent->origText = item->origText;
                    item->parent->parent->childItems[4]->UpdateText(item->origText);
                    pen::ui::Submit();
                }
            }
        }
    }
    return true;
}

bool OnFileModalSetFocus(pen::ui::Item* item, int button, int action) {
    /*Return focus to the file modal*/
    if (item->fileModalIntent == pen::ui::FILE_INTENT::SAVE_AS) {
        if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
            pen::State::Get()->SetKeyable((void*)item->childItems[4]);
        }
    }
    return true;
}

bool OnFileModalButtonClicked(pen::ui::Item* item, int button, int action) {
    /*If the cancel or confirm button has been pressed*/
    if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
        if (item->origText == "X") {
            if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, "", false);
        }
        else {
            switch (item->parent->fileModalIntent) {
            case pen::ui::FILE_INTENT::SAVE_AS:
                if (item->parent->origText != "") {
                    if (item->parent->origText.find(".") != std::string::npos) {
                        int dotIdx = item->parent->origText.find(".");
                        item->parent->origText = item->parent->origText.substr(0, dotIdx) + item->parent->fileModalMimeType;
                    }
                    else {
                        item->parent->origText += item->parent->fileModalMimeType;
                    }

                    for (int i = 0; i < item->parent->origText.length(); i++) {
                        item->parent->origText[i] = std::tolower(item->parent->origText[i]);
                    }

                    std::string currentDir = CombineFileModalPathPrefix(item->parent);
                    for (int i = 0; i < item->parent->fileModalList.size(); i++) {
                        if (item->parent->fileModalList[i] == currentDir + item->parent->origText) {
                            /*To prevent overwriting of file that is already there*/
                            break;
                            break;
                        }
                    }
                    if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, CombineFileModalPathPrefix(item->parent) + item->parent->origText, true);
                }
                break;
            case pen::ui::FILE_INTENT::SELECT_FILE:
                if (item->parent->origText != "") {
                    if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, CombineFileModalPathPrefix(item->parent) + item->parent->origText, true);
                }
                break;
            case pen::ui::FILE_INTENT::SELECT_DIRECTORY:
                if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, CombineFileModalPathPrefix(item->parent), true);
                break;
            case pen::ui::FILE_INTENT::SELECT_FILES:
                item->parent->UpdateFileModalListing(CombineFileModalPathPrefix(item->parent), true, true);
                if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, ParseFileModalFilesList(item->parent), true);
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool OnFileInputKeyPress(pen::ui::Item* item, int key, int action) {
    /*Update the file input box*/
    if (item->parent->fileModalIntent == pen::ui::FILE_INTENT::SAVE_AS) {
        if (action == pen::in::KEYS::PRESSED) {
            if ((key >= 65 && key <= 90) || key == pen::in::KEYS::SEMICOLON || key == pen::in::KEYS::MINUS || key == pen::in::KEYS::PERIOD) {
                pen::State* inst = pen::State::Get();
                std::string letter = "";
                for (int i = 0; i < inst->characters.Size(); i++) {
                    if (inst->characters.items[i].second.character == key) {
                        letter = inst->characters.items[i].first;
                        break;
                    }
                }
                item->parent->origText += letter;
                item->UpdateText(item->parent->origText);

                if (item->parent->origText.length() > 2) {
                    if (item->parent->origText.substr(item->parent->origText.length() - 2) == "--") {
                        item->parent->origText =
                            item->parent->origText.substr(0, item->parent->origText.length() - 2) + "_";
                        item->UpdateText(item->parent->origText);
                    }
                }
                pen::ui::Submit();
            }
            else if (key == pen::in::KEYS::BACKSPACE) {
                if (item->parent->origText.length() > 0)
                    item->parent->origText = item->parent->origText.substr(0, item->parent->origText.length() - 1);
                item->UpdateText(item->parent->origText);
                pen::ui::Submit();
            }
            else if (key == pen::in::KEYS::ENTER) {
                if (item->parent->origText != "") {
                    if (item->parent->origText.find(".") != std::string::npos) {
                        int dotIdx = item->parent->origText.find(".");
                        item->parent->origText = item->parent->origText.substr(0, dotIdx) + item->parent->fileModalMimeType;
                    }
                    else {
                        item->parent->origText += item->parent->fileModalMimeType;
                    }

                    for (int i = 0; i < item->parent->origText.length(); i++) {
                        item->parent->origText[i] = std::tolower(item->parent->origText[i]);
                    }

                    std::string currentDir = CombineFileModalPathPrefix(item->parent);
                    for (int i = 0; i < item->parent->fileModalList.size(); i++) {
                        if (item->parent->fileModalList[i] == currentDir + item->parent->origText) {
                            /*To prevent overwriting of file that is already there*/
                            break;
                            break;
                        }
                    }

                    pen::State::Get()->SetKeyable(nullptr);
                    if (item->parent->userOnPathLoadCallback != nullptr) (*item->parent->userOnPathLoadCallback)(item->parent, CombineFileModalPathPrefix(item->parent) + item->parent->origText, true);
                }
            }
        }
    }
    return true;
}

namespace pen {
    namespace ui {
        FileModal::FileModal() { isUI = true; }

        FileModal::FileModal(uint32_t objectId, unsigned int userIntent, std::string userMimeType, pen::Vec4 objectColor, pen::Vec4 objectAccentColor,
            pen::ui::Item* objectParent, void (*onPathLoadCallback)(pen::ui::Item*, std::string, bool), std::string objectTextureName, float itemTexCoordStartX, float itemTexCoordStartY,
            float itemTexCoordEndX, float itemTexCoordEndY) {
            pen::Vec2 position = pen::Vec2(pen::PixelBufferHeight() / 3.0f, pen::PixelBufferHeight() / 3.0f);
            pen::Vec2 size = pen::Vec2(pen::PixelBufferHeight() / 3.0f, pen::PixelBufferHeight() / 2.0f);
            if (objectTextureName != "") {
                data = pen::CreateSprite(position.x, position.y, size.x, size.y, objectTextureName, itemTexCoordStartX,
                    itemTexCoordStartY, itemTexCoordEndX, itemTexCoordEndY);
                data->color = objectAccentColor;
            }
            else {
                data = pen::DrawRect(position.x, position.y, size.x, size.y, objectAccentColor);
            }
            id = objectId;
            isUI = true;
            pen::State* inst = pen::State::Get();
            SetPosition(pen::Vec3(position.x, position.y, 0.0f));
            SetSize(pen::Vec2(size.x, size.y));
            parent = objectParent;
            userOnClickCallback = &OnFileModalSetFocus;
            userOnPathLoadCallback = onPathLoadCallback;
            isClickable = true;
            fileModalIntent = userIntent;
            fileModalMimeType = "";
            if (userMimeType.length() > 0) {
                fileModalMimeType = (userMimeType.substr(0,1) == ".") ? userMimeType : ("." + userMimeType);
            }

            textureName = objectTextureName;

            texCoordStartX = itemTexCoordStartX;
            texCoordStartY = itemTexCoordStartY;
            texCoordEndX = itemTexCoordEndX;
            texCoordEndY = itemTexCoordEndY;

            shapeType = pen::ui::Shape::QUAD;

            std::string objectTitle = "";
            std::string fileInputText = "";
            switch (userIntent) {
            case pen::ui::FILE_INTENT::SAVE_AS:
                objectTitle = "Save As";
                fileInputText = "Enter file name";
                break;
            case pen::ui::FILE_INTENT::SELECT_DIRECTORY:
            case pen::ui::FILE_INTENT::SELECT_FILES:
                objectTitle = "Choose Folder";
                break;
            case pen::ui::FILE_INTENT::SELECT_FILE:
                objectTitle = "Choose File";
                break;
            default:
                break;
            }

            /*Initialize the title*/
            Push(new pen::ui::TextBox(ID_ANY, objectTitle, pen::Vec3(position.x, position.y + size.y - (0.1f * size.y), 0.0f),
                objectTitle.size(), 0.1f * size.y, pen::PEN_TRANSPARENT, objectColor, nullptr, nullptr));
            childItems[0]->SetSize(pen::Vec2(size.x, childItems[0]->GetSize()->y));

            /*List component*/
            Push(new pen::ui::VerticalList(ID_ANY, pen::Vec3(position.x, position.y + (size.y / 5.0f), 0.0f),
                pen::Vec2(size.x, size.y / 3.0f * 2.1f), objectAccentColor, objectColor, this, nullptr));
            list = childItems[1];

            /*Cancel button*/
            Push(new pen::ui::Button(ID_ANY, "X", pen::Vec3(position.x + (size.x / 10.0f * 9.0f), position.y + (size.y / 10.0f * 9.0f), 0.0f),
                size.x / 10.0f, pen::PEN_TRANSPARENT, objectAccentColor, this, &OnFileModalButtonClicked));
            cancelBtn = childItems[2];

            /*Confirm button*/
            Push(new pen::ui::Button(ID_ANY, "OK", pen::Vec3(position.x + (size.x / 10.0f * 8.0f), position.y + size.y / 11.0f, 0.0f),
                size.x / 10.0f, pen::PEN_TRANSPARENT, objectColor, this, &OnFileModalButtonClicked));
            confirmBtn = childItems[3];

            /*File input box*/
            Push(new pen::ui::TextBox(ID_ANY, fileInputText, pen::Vec3(position.x, position.y + size.y / 9.0f, 0.0f), 20, pen::PEN_TRANSPARENT, pen::PEN_WHITE,
                this, nullptr));
            fileInput = childItems[4];
            fileInput->itemScaling = 0.8f;

            if (userIntent == pen::ui::FILE_INTENT::SAVE_AS) {
                pen::State::Get()->SetKeyable((void*)fileInput);
                fileInput->userOnKeyCallback = &OnFileInputKeyPress;
            }

            if (childItems[0]->childItems.size() > 0) {
                if (childItems[0]->childItems[0]->GetPosition()->y < childItems[0]->GetPosition()->y) {
                    /*Hide the title*/
                    for (int i = 0; i < childItems[0]->childItems.size(); i++) childItems[0]->childItems[i]->AllowActive(false);
                }
            }

            UpdateFileModalListing(CombineFileModalPathPrefix(this));

            CheckActiveStatus();
        }

        void FileModal::UpdateFileModalListing(std::string directoryPath, bool recursive, bool firstIter) {
            /*Load in new list items based on given directory path for Windows*/      
            std::string entryName;
            std::ifstream stream;
            DIR* dir;
            struct dirent* entry;
            pen::State* inst = pen::State::Get();
            if(firstIter) fileModalList.clear();
            if(recursive){
                /*Go through and grab files recursively*/
                if (dir = opendir(directoryPath.c_str())) {
                    while (entry = readdir(dir)) {
                        entryName = entry->d_name;
                        if (entryName != "." && entryName != "..") {

                            if (entryName.find(fileModalMimeType) != std::string::npos || (fileModalMimeType == "*" && entryName.find(".") != std::string::npos)) {
                                fileModalList.push_back(directoryPath + entryName);
                            }

                            if (entryName.find(".") == std::string::npos) {
                                UpdateFileModalListing(directoryPath + entryName + "/", true, false);
                            }
                        }
                    }
                    closedir(dir);
                }
            }else{
                int listSize = list->childItems.size();
                for (int i = 0; i < listSize; i++) {
                    list->Pop();
                }

                if (dir = opendir(directoryPath.c_str())) {
                    while (entry = readdir(dir)) {
                        entryName = entry->d_name;
                        if (entryName != "$Recycle.bin" && entryName[0] != '.' && entryName != ".") {
                            if (entryName.length() < 50) {
                                if ((entryName.find(fileModalMimeType) != std::string::npos || (fileModalMimeType == "*" && entryName.find(".") != std::string::npos))
                                    && fileModalIntent != pen::ui::FILE_INTENT::SELECT_DIRECTORY) {
                                    /*Adds a file to the list*/
                                    fileModalList.push_back(directoryPath + entryName);
                                    if (fileModalIntent != pen::ui::FILE_INTENT::SELECT_FILES) {
                                        list->Push(new pen::ui::Button(ID_ANY, entryName, pen::Vec3(0.0f, pen::PixelBufferHeight() - 40.0f, 0.0f), 50, pen::PEN_BLUE, *GetColor(), list, &OnFileModalItemSelected));
                                        if (list->childItems[list->childItems.size() - 2]->itemScaling > 1.0f) {
                                            float scaling = 1.0f / list->childItems[list->childItems.size() - 2]->itemScaling;
                                            pen::ui::Scale(list->childItems[list->childItems.size() - 2], pen::Vec2(scaling, scaling), true);
                                        }
                                    }
                                }
                                else {
                                    /*Adds a directory to the list*/
                                    if (entryName.find(".") == std::string::npos && entryName != "..") {
                                        list->Push(new pen::ui::Button(ID_ANY, entryName, pen::Vec3(0.0f, pen::PixelBufferHeight() - 40.0f, 0.0f), 50, pen::PEN_YELLOW, *GetColor(), list, &OnFileModalItemSelected));
                                        if (list->childItems[list->childItems.size() - 2]->itemScaling > 1.0f) {
                                            float scaling = 1.0f / list->childItems[list->childItems.size() - 2]->itemScaling;
                                            pen::ui::Scale(list->childItems[list->childItems.size() - 2], pen::Vec2(scaling, scaling), true);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    closedir(dir);

                    /*This adds a way to get to the previous directory if not loaded in*/
                    if (list->childItems.size() >= 2 && directoryPath != "C://") {
                        if (list->childItems[1]->origText != "..") {
                            list->Push(new pen::ui::Button(ID_ANY, "..", pen::Vec3(0.0f, inst->screenHeight - 40.0f, 0.0f), 50, pen::PEN_YELLOW, *GetColor(), list, &OnFileModalItemSelected));
                            pen::ui::Item* prevDir = list->childItems[list->childItems.size() - 2];
                            std::vector<pen::ui::Item*> tempList;
                            for (int i = 0; i < list->childItems.size(); i++) {
                                if (i == 0) {
                                    tempList.push_back(list->childItems[i]);
                                }
                                else if (i == 1) {
                                    tempList.push_back(prevDir);
                                }
                                else if (i == list->childItems.size() - 1) {
                                    tempList.push_back(list->childItems[list->childItems.size() - 1]);
                                }
                                else {
                                    tempList.push_back(list->childItems[i - 1]);
                                }
                            }
                            list->childItems = tempList;

                            /*This redundant item is pushed and popped to force an update of the vertical list component without
                            having to make the UpdateItemPositions function available to pen::ui::Item*/
                            list->Push(new pen::ui::Button(ID_ANY, "..", pen::Vec3(0.0f, pen::PixelBufferHeight() - 40.0f, 0.0f), 50, pen::PEN_YELLOW, *GetColor(), list, &OnFileModalItemSelected));
                            list->Pop();
                        }
                    }

                    pen::ui::Submit();
                }
            }
        }
    }
}
#endif
