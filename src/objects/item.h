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
#include "../renderer/renderer.h"
#include "../core/texture.h"
#include "../core/vertex_buffer_schema.h"
#include "../ops/vectors/vec4.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec2.h"
#include "../objects/shape.h"
#include "../state/state.h"
#include "../state/asset.h"
#include "../state/keys.h"
#include <iostream>

#define ITEM_BATCH_VERTEX_ELEMENTS 10

namespace pen {
    namespace ui {
        class Item {
        public:
            Texture texture;
            pen::Vec3 positions;
            pen::Vec2 size;
            pen::Vec3 angles;
            std::vector<float> bufferPositions;
            unsigned int shapeType;
            pen::Vec4 color;
            std::vector<pen::ui::Item*> childItems;
            float itemScaling = 1.0f;
           pen::ui::Item* parent;
            bool isActive = false;
            bool forceActive = true;
            bool isFixed = false;
            bool isWireFrame = false;
            pen::Mat4x4 model;

            /*Used mainly for the text editor ui component*/
            bool isSingular = false;

            /*Used for if an item is a text character*/
            bool isText = false;
            unsigned int itemCount = 0;

            /*Optional for user passing in a texture*/
            std::string textureName;
            /*This id is optional for sorting by the user*/
            uint32_t id;

            /*Texture coordinates for using sprite sheets*/
            float texCoordStartX = 0.0f;
            float texCoordStartY = 0.0f;
            float texCoordEndX = 1.0f;
            float texCoordEndY = 1.0f;

            /*----For ui items----*/
            unsigned int cycles = 0;
            unsigned int textLength = 20;
            unsigned int textPadding = 10;
            bool isUI = false;
            bool isClickable = false;
            bool isDraggable = false;
            std::string origText;
            pen::Vec4 textColor;
            bool (*userOnClickCallback)(Item*, int, int);
            bool (*userOnDragCallback)(Item*, double*, double*);
            bool (*userOnKeyCallback)(Item*, int, int);

            /*----For text characters----*/
            unsigned int charRowIdx = 0;
            unsigned int charColumnIdx = 0;
            /*----For text characters----*/

            /*----For vertical list components----*/
            bool isList = false;
            bool isListItem = false;
            /*----For vertical list components----*/

            /*----For scroll bars----*/
            float heightOfScrollParentItems = 0.0f;
            /*----For scroll bars----*/

            /*----For sliders----*/
            bool sliderActive = false;
            /*List of offsets one for each slider used for sliders value from 0 to 1 from the beginning of the bar*/
            float* sliderOffset = nullptr;
            /*For slider mode 0 is horizontal and 1 is vertical*/
            unsigned int sliderMode = 0;
            /*Which slider it is in a list of sliders*/
            unsigned int sliderOrder = 0;
            /*----For sliders----*/

            /*----For text editors----*/
            /*This is used to determine whether highlighting quads should be added with each text character*/
            bool isTextEditor = false;
            bool textEditorHighlightActive = false;
            /*----For text editors----*/

            /*----For file modals----*/
            void (*userOnPathLoadCallback)(Item*, std::string, bool);
            unsigned int fileModalIntent;
            std::string fileModalMimeType;
            std::vector<std::string> fileModalPathPrefix = { "C://", "Users" };
            std::vector<std::string> fileModalList;
            /*----For file modals----*/
            /*----For ui items----*/

            /*----For 3D items----*/
            int* indices3D = nullptr;
            unsigned int indexCount3D = 0;
            /*----For 3D items----*/

            /*----For complex shape items----*/
            unsigned int complexIndexCount = 0;
            unsigned int originalComplexIndexCount = 0;
            std::vector<int> complexIndices;
            /*----For complex shape items----*/

            Item();

            Item(uint32_t objectId, float* positions, unsigned int objectVertexCount, int* indices, unsigned int objectIndexCount, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), bool objectIsFixed = false,
                bool objectIsWireFrame = false);

            Item(uint32_t objectId, pen::Vec3 objectPositions, pen::Vec2 objectSize, unsigned int objectShapeType, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
               pen::ui::Item* objectParent = nullptr, bool (*onClickCallback)(Item*, int, int) = nullptr, bool objectIsFixed = false, std::string objectTextureName = "",
                float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f, float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);

            ~Item();

            virtual void Push(Item* item);

            virtual void Pop();

           pen::ui::Item* FindItem(uint32_t id);
            void RemoveItemById(uint32_t id);
            bool RemoveItem(Item* id);
            void RemoveNestedItems(Item* id);

            /*----For updating child item offsets after scaling----*/
            void UpdateChildOffsets(const pen::Vec2& scaling);
            pen::Vec2 ItemGetPointOfOrigin(float* positions, const int& axis);
            void ItemRotate(const float& objectAngle, const int& axis, bool staticTransform = false,
                const pen::Vec2& objectPointOfOrigin = pen::Vec2(0.0f, 0.0f), const bool& calculatePointOfOrigin = true);
            /*----For updating child item offsets after scaling----*/

            pen::Vec3 GetPosition();
            void SetPosition(pen::Vec3 objectPos);
            pen::Vec2 GetSize();
            void SetSize(pen::Vec2 objectSize);
            pen::Vec4 GetColor();
            void SetColor(pen::Vec4 objectColor);
            bool GetActiveStatus();
            void AllowActive(bool status);

            /*----For ui objects----*/
            /*----For textboxes----*/
            virtual void SetTextColor(pen::Vec4 itemTextColor, int pos = -1) {}
            virtual void UpdateText(const std::string& userText) {}
            /*----For textboxes----*/

            /*----For sliders----*/
            virtual void UpdateSliderOrientation() {}
            /*----For sliders----*/

            /*----For parent items of scroll bar/slider components----*/
            virtual void ScrollBarUpdateItemPositions() {}
            virtual void SliderUpdateItemPositions() {}
            /*----For parent items of scroll bar/slider components----*/

            /*----For text editors----*/
            virtual void UpdateTextCursor(pen::ui::Item* textCharacter) {}
            /*----For text editors----*/

            /*----For file modals----*/
            virtual void UpdateFileModalListing(std::string directoryPath, bool recursive = false, bool firstIter = true) {}
            /*----For file modals----*/
            /*----For ui objects----*/

            virtual void CombineChildBuffers();
            int GetAssetId();
            void CheckActiveStatus();

            virtual bool OnClick(Item* item, const int& button, const int& action);
            virtual bool OnDrag(Item* item, double* xPos, double* yPos);
            virtual bool OnKey(Item* item, const int& key, const int& action);
        };
    }
}