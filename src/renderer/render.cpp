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

namespace pen {
    Render* Render::instance = nullptr;

    void Render::Background(pen::Vec4 color) {
        /*Sets the background color for the window*/
#ifndef __PEN_MAC_IOS__
        glClearColor(color.x, color.y, color.z, color.w);
#else
        MapMacIOSBackground(color.x, color.y, color.z, color.w);
#endif
    }

#ifndef __PEN_MAC_IOS__
    void Render::RenderLayer(pen::Layer* layer) {
        /*Render a batch of objects*/
        Render* inst = pen::Render::Get();
        pen::Shader shader = layer->isInstanced ? inst->instancedShader : inst->appShader;

#ifndef __PEN_MOBILE__
        if (layer->isWireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
#endif

        shader.Bind();

        /*Bind the initial assets*/
        if (inst->firstTime) {
            TextureSet();
            inst->firstTime = false;
        }
        
        /*Update the instanced uniforms*/
        if (layer->isInstanced) UpdatedInstancedUniforms(layer);
        
        /*If pixel-by-pixel drawing is needed*/
        if (pen::State::Get()->usingBuffer) Texture::UpdatePixels();
        
        pen::op::Translate(&layer->model, pen::Vec3(layer->translation.x, layer->translation.y, layer->translation.z));

        /*Update camera*/
        inst->camera.Update(0.1f, 1000.0f, &inst->appPerspectiveView, &inst->appPerspectiveProj, layer);

        /*This model view projection matrix is used for transformations of a given layer*/
        pen::Mat4x4 mvp = (layer->model * (layer->isUI ? inst->appOrthoView : inst->appPerspectiveView)) * (layer->is3D ? inst->appPerspectiveProj : inst->appOrthoProj);

        shader.SetUniformMat4x4f("uMVP", mvp);

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

        pen::Renderer::Draw(layer->va, layer->ib, layer->indexCount, layer->vb, shader, layer->shapeType, layer->isInstanced, layer->instancedDataList.size());
    }
#else
    void Render::RenderLayer() {
        /*Render a batch of objects*/
        Render* inst = pen::Render::Get();

        /*Bind the initial assets*/
        if (inst->firstTime) {
            TextureSet();
            
            inst->firstTime = false;
        }
        
        /*If pixel-by-pixel drawing is needed*/
        if (pen::State::Get()->usingBuffer) Texture::UpdatePixels();
        
        for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
            pen::op::Translate(&pen::ui::LM::layers[i]->model, pen::Vec3(pen::ui::LM::layers[i]->translation.x, pen::ui::LM::layers[i]->translation.y, pen::ui::LM::layers[i]->translation.z));

            /*Update camera*/
            inst->camera.Update(90, 0.1f, 1000.0f, &inst->appPerspectiveView, &inst->appPerspectiveProj, pen::ui::LM::layers[i]);

            /*This model view projection matrix is used for transformations of a given layer*/
            pen::Mat4x4 mvp = (pen::ui::LM::layers[i]->model * (pen::ui::LM::layers[i]->isUI ? inst->appOrthoView : inst->appPerspectiveView)) * (pen::ui::LM::layers[i]->is3D ? inst->appPerspectiveProj : inst->appOrthoProj);

            MapMacIOSAddUniform(pen::ui::LM::layers[i]->id, mvp);
            
            /*Update the instanced uniforms*/
            UpdatedInstancedUniforms(pen::ui::LM::layers[0]);
        }
        
        MapMacIOSUpdateUniforms();

        /*Updates the GPU with the buffer data*/
        if (pen::State::Get()->updateBatch) {
            MapMacIOSSubmitBatch(pen::Layer::batchVertices, BATCH_VERTICES_SIZE);
            if (pen::State::Get()->firstUpdateFrame) {
                pen::State::Get()->firstUpdateFrame = false;
            }
            else {
                pen::State::Get()->updateBatch = false;
            }
        }

        pen::Renderer::Draw();
    }
#endif

    void Render::TextureSet() {
        pen::State* inst = pen::State::Get();
        pen::Render* render = pen::Render::Get();

#ifndef __PEN_ANDROID__
        /*
         Loops through and binds the texture assets
         Android textures are loaded in on the Java side via a different thread
         */
        for (int i = 0; i < inst->textureUnits; i++) {
            std::string a = pen::Asset::FindById(i);

            /*Asset regex gets returned if the given asset name could not be found*/
            if (a != PEN_ASSET_NOT_FOUND) {
                Texture::Get()->Initialize(pen::Asset::FindById(i), i);
            }
        }
#endif
        /*These only get set once otherwise there is a weird texture error of only showing the last loaded texture*/
        if (render->firstTime) {
            for (int j = 0; j < inst->textureUnits; j++) {
                render->appShader.SetUniform1i("uTextures[" + std::to_string(j) + "]", j);
                render->instancedShader.SetUniform1i("uTextures[" + std::to_string(j) + "]", j);
            }
        }
    }


    void Render::UpdatedInstancedUniforms(pen::Layer* layer) {
        /*Update the uniforms for the instanced shader*/
#ifndef __PEN_MAC_IOS__
        pen::Render* inst = pen::Render::Get();
        int vecCount = layer->instancedDataList.size() > 400 ? 400 : layer->instancedDataList.size();
        for (int i = 0; i < vecCount; i++) {
            inst->instancedShader.SetUniform3f("uInstancedOffsets[" + std::to_string(i) + "]", layer->instancedDataList[i]);
        }
#else
        /*Update the instanced uniform data for the Metal shader*/
        pen::Render* inst = pen::Render::Get();
        std::vector<pen::Vec3*> instancedDataList = layer->instancedDataList != nullptr ? *layer->instancedDataList : std::vector<pen::Vec3*>();
        int vecCount = instancedDataList.size() > 0 ? instancedDataList.size() : 400;
        IOSInstanceData* instanceData = MapMacIOSGetInstanceData();
        if(instanceData == nullptr) instanceData = new IOSInstanceData[vecCount];
        if(layer->instancedDataList != nullptr){
            for (int i = 0; i < vecCount; i++) {
                instanceData[i].uInstancedOffsets.x = instancedDataList[i]->x;
                instanceData[i].uInstancedOffsets.y = instancedDataList[i]->y;
                instanceData[i].uInstancedOffsets.z = instancedDataList[i]->z;
            }
        }else{
            for (int i = 0; i < vecCount; i++) {
                instanceData[i].uInstancedOffsets.x = 0.0f;
                instanceData[i].uInstancedOffsets.y = 0.0f;
                instanceData[i].uInstancedOffsets.z = 0.0f;
            }
        }
        MapMacIOSUpdateInstanceUniform(instanceData);
#endif
    }
}
