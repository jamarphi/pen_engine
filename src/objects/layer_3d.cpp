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
#include "layer_3d.h"

namespace pen {
    Layer3D::Layer3D() {}

    Layer3D::Layer3D(uint16_t generalId, unsigned int objectShapeType, bool objectIsFixed, bool objectIsSingular, bool objectIsWireFrame, bool objectIsInstanced, std::vector<pen::Vec3*> dataList) {
        /*Layers are separated mainly based on the shape type of the objects they contain, if it is fixed, and if it is instanced*/
        indexCount = 0;
        va = VertexArray();
        shapeType = objectShapeType;
        isFixed = objectIsFixed;
        isSingular = objectIsSingular;
        id = generalId;
        is3D = true;
        isWireFrame = objectIsWireFrame;
        translation = pen::Vec3(1.0f, 1.0f, 1.0f);
        isInstanced = objectIsInstanced;
        instancedDataList = dataList;
        if (objectIsInstanced) isSingular = true;
    }

    Layer3D::~Layer3D() {}

    bool Layer3D::Push(pen::ui::Item* item, const unsigned int& offset) {
        /*Adds a 3d item to the layer*/
        if (layerItems.size() > MAX_OBJECTS || (isSingular && itemCount > 0)) {
            return false;
        }
        else {
            layerItems.push_back(item);
            itemCount += item->itemCount + 1;
            itemOffset = offset;
            model = item->model;
            return true;
        }
    }

    void Layer3D::CombineBuffers() {
        /*Resets the vertices array before combining buffers*/
#ifndef __PEN_MAC_IOS__
        std::memset(batchVertices, 0, MAX_OBJECTS * BATCH_VERTEX_ELEMENTS);
#endif

        int bufferOffset = 0;
        int batchVertexSize = sizeof(BatchVertexData);
        int vertexNum = 1;
        int itemSize = 0;
        int itemCounter = 0;

        /*This is how many elements are in each buffer vector for a given item*/
        itemSize = vertexNum * BATCH_VERTEX_ELEMENTS;

        for (int i = 0; i < layerItems.size(); i++) {
            if (!layerItems[i]->isActive) continue;
            int subItemCount = itemOffset;

            /*Some objects can have more than the just their own buffer data since they can have child items*/
            while (subItemCount < layerItems[i]->bufferPositions.size()) {
                if (itemCounter == MAX_OBJECTS) {
                    break;
                    break;
                }

                if (layerItems[i]->bufferPositions.size() - subItemCount < itemSize) {
#ifndef __PEN_MAC_IOS__
                    for (int j = 0; j < layerItems[i]->bufferPositions.size() - subItemCount; j++) {
                        batchVertices[bufferOffset + j] = layerItems[i]->bufferPositions[subItemCount + j];
                    }
#else
                    int lastVertices = (layerItems[i]->bufferPositions.size() - subItemCount) / BATCH_VERTEX_ELEMENTS;
                    for (int j = 0; j < lastVertices; j++) {
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j)];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 1];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 2];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 3];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 4];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 5];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.w = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 6];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 7];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 8];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texId = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 9];
                    }
#endif
                }
                else {
#ifndef __PEN_MAC_IOS__
                    for (int j = 0; j < itemSize; j++) {
                        batchVertices[bufferOffset + j] = layerItems[i]->bufferPositions[subItemCount + j];
                    }
#else
                    for (int j = 0; j < vertexNum; j++) {
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j)];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 1];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].position.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 2];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 3];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 4];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.z = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 5];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].color.w = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 6];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.x = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 7];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texCoord.y = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 8];
                        batchVertices[(bufferOffset / BATCH_VERTEX_ELEMENTS) + j].texId = layerItems[i]->bufferPositions[subItemCount + (BATCH_VERTEX_ELEMENTS * j) + 9];
                    }
#endif
                }

                /*Index count for 3D objects is managed in the object itself*/
                subItemCount += itemSize;
                bufferOffset += itemSize;
                itemCounter++;
            }
        }
    }

    void Layer3D::InitializeIndices() {
        /*Initialize indices based on .obj file*/
        long offset = (layerItems[0]->itemCount / MAX_OBJECTS) * (RENDERER_INDICES_SIZE / 3);
        long offsetEnd = offset + (RENDERER_INDICES_SIZE / 3);
        if (offsetEnd > layerItems[0]->itemCount * 6) offsetEnd = offset + ((layerItems[0]->itemCount * 6) - offset);
        indexCount = (layerItems[0]->indexCount3D > RENDERER_INDICES_SIZE) ? RENDERER_INDICES_SIZE : layerItems[0]->indexCount3D;

        int counter = 0;
        for (int i = offset; i < offsetEnd; i++) {
            batchIndices[counter] = layerItems[0]->indices3D[i];
            counter++;
        }
    }
}
