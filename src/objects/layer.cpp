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
#include "layer.h"

namespace pen {
    
    Layer::Layer(){}

    Layer::Layer(uint16_t generalId, unsigned int objectShapeType, bool objectIsFixed, bool objectIsSingular, bool objectIsWireFrame) {
        /*Layers are separated mainly based on the shape type of the objects they contain, if it is fixed, and if it is instanced*/
        indexCount = 0;
        va = VertexArray();
        shapeType = objectShapeType;
        isFixed = objectIsFixed;
        isSingular = objectIsSingular;
        isWireFrame = objectIsWireFrame;
        id = generalId;
        translation = pen::Vec3(1.0f, 1.0f, 0.0f);
        model = pen::Mat4x4(1.0f);
    }

    void Layer::Initialize() {
        /*Must be called after items are added to layer*/
        CombineBuffers();

#ifndef __PEN_IOS__
        VertexBuffer tempVb(sizeof(BatchVertexData) * MAX_OBJECTS);
#else
        VertexBuffer tempVb(id, &batchVertices, sizeof(BatchVertexData) * MAX_OBJECTS);
#endif
        vb = tempVb;

        VertexBufferSchema layout;
        /*3 vertices for batch position*/
        layout.Push(3);

        /*4 vertices for batch color*/
        layout.Push(4);

        /*2 vertices for texture coordinate*/
        layout.Push(2);

        /*1 vertex for texture id*/
        layout.Push(1);

        vb.Bind();
        /*Adds the attribute layout to the vertex array object*/
#ifndef __PEN_IOS__
        va.AddBuffer(layout);
#else
        va.AddBuffer(id);
#endif

        InitializeIndices();

        /*Creates the index buffer to determine how vertices will be connected*/
#ifndef __PEN_IOS__
        IndexBuffer tempIb(batchIndices, RENDERER_INDICES_SIZE);
#else
        IndexBuffer tempIb(id, batchIndices, RENDERER_INDICES_SIZE);
#endif
        ib = tempIb;
        

        if (shapeType == pen::ui::Shape::COMPLEX) indexCount = layerItems[0]->complexIndexCount;
    }

    void Layer::Update() {
        CombineBuffers();
    }

    Layer::~Layer() {}

    bool Layer::Push(pen::ui::Item* item, const unsigned int& offset) {
        /*Adds an item with all its children to the layer*/
        if (layerItems.size() > MAX_OBJECTS || (isSingular && itemCount > 0)) {
            return false;
        }
        else {
            if (item->shapeType != shapeType) {
                std::cout << "You must use the same shape type for items that are part of this layer" << std::endl;
                return false;
            }
            else {
                layerItems.push_back(item);
                itemCount += item->itemCount + 1;
                return true;
            }
        }
    }

    void Layer::Pop() {
        /*Removes the most recent item with its children from the layer*/
        if (layerItems.size() > 0) {
            itemCount -= layerItems[layerItems.size() - 1]->itemCount - 1;
            pen::ui::Item* item = layerItems[layerItems.size() - 1];
            item->RemoveNestedItems(item);
            delete item;
            layerItems.pop_back();
        }
    }

    void Layer::Destroy() {
        /*Removes the vertex array, vertex buffer, and index buffer objects from memory on the GPU*/
        va.Destroy();
        vb.Destroy();
        ib.Destroy();
    }

    void Layer::CombineBuffers() {
        /*Combines the buffers of all the items associated with the layer*/

        /*Resets the vertices array before combining buffers*/
#ifndef __PEN_IOS__
        std::memset(batchVertices, 0, MAX_OBJECTS * BATCH_VERTEX_ELEMENTS);
#endif

        int bufferOffset = 0;
        int batchVertexSize = sizeof(BatchVertexData);
        int vertexNum = 0;
        int itemSize = 0;

        /*Primitive shape types are separate due to the multiples being different*/
        switch (shapeType) {
        case 0:
        case 6:
            //Points and complex shapes
            vertexNum = 1;
            break;
        case 1:
            //Lines
            vertexNum = 2;
            break;
        case 2:
            //Triangles
            vertexNum = 3;
            break;
        case 3:
        case 4:
            //Quads
            vertexNum = 4;
            break;
        default:
            break;
        }

        /*This is how many elements are in each buffer vector for a given item*/
        itemSize = vertexNum * BATCH_VERTEX_ELEMENTS;

        for (int i = 0; i < layerItems.size(); i++) {
            if (!layerItems[i]->isActive) continue;
            int subItemCount = 0;

            /*Some objects can have more than the just their own buffer data since they can have child items*/
            while (subItemCount < layerItems[i]->bufferPositions.size()) {
                if (layerItems[i]->bufferPositions.size() - subItemCount < itemSize) {
#ifndef __PEN_IOS__
                    for (int j = 0; j < layerItems[i]->bufferPositions.size() - subItemCount; j++) {
                        batchVertices[bufferOffset + j] = layerItems[i]->bufferPositions[subItemCount + j];
                    }
#else
                    int lastVertices = (layerItems[i]->bufferPositions.size() - subItemCount) / BATCH_VERTEX_ELEMENTS;
                    for (int j = 0; j < lastVertices; j++) {
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j)];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 1];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 2];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 3];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 4];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 5];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.w = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 6];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 7];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 8];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texIndex = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 9];
                    }
#endif
                }
                else {
#ifndef __PEN_IOS__
                    for (int j = 0; j < itemSize; j++) {
                        batchVertices[bufferOffset + j] = layerItems[i]->bufferPositions[subItemCount + j];
                    }
#else
                    for (int j = 0; j < vertexNum; j++) {
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j)];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 1];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].vertex.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 2];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 3];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 4];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 5];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.w = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 6];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 7];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 8];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texIndex = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 9];
                    }
#endif
                }

                subItemCount += itemSize;
                bufferOffset += itemSize;
                UpdateIndexCount(&indexCount, layerItems[i]->complexIndexCount);
            }
        }
    }

    void Layer::UpdateIndexCount(int* idxCount, const unsigned int& complexIdxCount) {
        /*Updates the index count for the active items to be rendered*/
        switch (shapeType) {
        case 0:
            //Points
            *idxCount += 1;
            break;
        case 1:
            //Lines
            *idxCount += 2;
            break;
        case 2:
            //Triangles
            *idxCount += 3;
            break;
        case 3:
        case 4:
            //Quads
            *idxCount += 6;
            break;
        case 6:
            //Complex shapes
            *idxCount = complexIdxCount;
            break;
        default:
            break;
        }
    }

    void Layer::InitializeIndices() {
        /*Initialize batch indices*/
        int offset = 0;
        unsigned int counter = 0;
        /*Primitive shape types are separate due to the multiples being different*/
        switch (shapeType) {
        case 0:
            //Points
            for (int i = 0; i < RENDERER_INDICES_SIZE; i++) {
                batchIndices[i] = i;
            }
            break;
        case 1:
            //Lines
            for (int i = 0; i < RENDERER_INDICES_SIZE; i += 2) {

                batchIndices[i] = offset;
                batchIndices[i + 1] = offset + 1;
                offset += 2;
            }
            break;
        case 2:
            //Triangles
            for (int i = 0; i < RENDERER_INDICES_SIZE; i += 3) {
                batchIndices[i] = offset;
                batchIndices[i + 1] = offset + 1;
                batchIndices[i + 2] = offset + 2;
                offset += 3;
            }
            break;
        case 3:
        case 4:
            //Quads
            for (int i = 0; i < RENDERER_INDICES_SIZE; i += 6) {
                batchIndices[i] = offset;
                batchIndices[i + 1] = offset + 1;
                batchIndices[i + 2] = offset + 2;

                batchIndices[i + 3] = offset + 2;
                batchIndices[i + 4] = offset + 3;
                batchIndices[i + 5] = offset;

                offset += 4;
            }
            break;
        case 6:
            //Complex shapes          
            for (int i = 0; i < layerItems.size(); i ++) {
                for (int j = 0; j < layerItems[i]->complexIndexCount; j++) {
                    batchIndices[counter] = layerItems[i]->complexIndices[j];
                    counter++;
                }
            }
            break;
        default:
            break;
        }
    }
}
