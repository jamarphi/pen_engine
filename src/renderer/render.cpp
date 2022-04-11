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
#include "render.h"
#include "../../ext/platforms/android/app/src/cpp/log.h"

namespace pen {
    Render* Render::instance = nullptr;

    void Render::RenderLayer(pen::Renderer* renderer, pen::Layer* layer) {
        /*Render a batch of objects*/
        Render* inst = pen::Render::Get();

#ifndef __PEN_MOBILE__
        if (layer->isWireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
#endif

        inst->appShader.Bind();

        /*Bind the initial assets*/
        if (inst->firstTime) {
            TextureSet();
            inst->firstTime = false;
        }
        
        /*If pixel-by-pixel drawing is needed*/
        if (pen::State::Get()->usingBuffer) Texture::UpdatePixels();

        pen::op::Translate(&layer->model, pen::Vec3(layer->translation.x, layer->translation.y, layer->translation.z));

        /*Update camera*/
        inst->camera.Update(45, 0.1f, 100.0f, &inst->appPerspectiveView, &inst->appPerspectiveProj, layer);

        /*This model view projection matrix is used for transformations of a given layer*/
        pen::Mat4x4 mvp = (layer->model * (layer->isFixed ? inst->appOrthoView : inst->appPerspectiveView)) * (layer->is3D ? inst->appPerspectiveProj : inst->appOrthoProj);

        inst->appShader.SetUniformMat4f("uMVP", mvp);

        /*Binds the vertex buffer of a given layer and updates the GPU with the buffer data*/
        layer->vb.Bind();
        if (pen::State::Get()->updateBatch) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(layer->batchVertices), layer->batchVertices);
            if (pen::State::Get()->firstUpdateFrame) {
                pen::State::Get()->firstUpdateFrame = false;
            }
            else {
                pen::State::Get()->updateBatch = false;
            }
        }

        renderer->Draw(layer->va, layer->ib, layer->indexCount, layer->vb, inst->appShader, 0, layer->shapeType);
    }

    void Render::TextureSet() {
        pen::State* inst = pen::State::Get();

#ifndef __PEN_MOBILE__
        /*Loops through and binds the assets for a particular batch*/
        for (int i = 0; i < inst->textureUnits; i++) {
            std::string a = pen::Asset::FindById(i);

            /*Asset regex gets returned if the given asset name could not be found*/
            if (a != ASSET_REGEX) {
                Texture::Get()->Initialize(pen::Asset::FindById(i).c_str(), i);
            }
        }
#endif
        /*These only get set once otherwise there is a weird texture error of only showing the last loaded texture*/
        if (Render::Get()->firstTime) {
            for (int k = 0; k < inst->textureUnits; k++) {
                Render::Get()->appShader.SetUniform1i("uTextures[" + std::to_string(k) + "]", k);
            }
        }
    }
}
