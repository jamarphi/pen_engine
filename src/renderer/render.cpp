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
#ifndef __PEN_IOS__
        glClearColor(color.x, color.y, color.z, color.w);
#else
        pen::State::Get()->iosMtkView->setClearColor(MTL::ClearColor::Make(color.x, color.y, color.z, color.w));
#endif
    }

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
        inst->camera.Update(45, 0.1f, 100.0f, &inst->appPerspectiveView, &inst->appPerspectiveProj, layer);

        /*This model view projection matrix is used for transformations of a given layer*/
        pen::Mat4x4 mvp = (layer->model * (layer->isFixed ? inst->appOrthoView : inst->appPerspectiveView)) * (layer->is3D ? inst->appPerspectiveProj : inst->appOrthoProj);

        shader.SetUniformMat4x4f("uMVP", mvp);

#ifdef __PEN_IOS__
        MTL::RenderCommandEncoder* iosCommandEncoder;
#endif

        /*Binds the vertex buffer of a given layer and updates the GPU with the buffer data*/
        layer->vb.Bind();
        if (pen::State::Get()->updateBatch) {
#ifndef __PEN_IOS__
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(layer->batchVertices), layer->batchVertices);
#else
            
            pen::Render::DrawIOSLayer(iosCommandEncoder, layer->vb);
#endif
            if (pen::State::Get()->firstUpdateFrame) {
                pen::State::Get()->firstUpdateFrame = false;
            }
            else {
                pen::State::Get()->updateBatch = false;
            }
        }

#ifndef __PEN_IOS__
        pen::Renderer::Draw(layer->va, layer->ib, layer->indexCount, layer->vb, shader, 0, layer->shapeType, layer->isInstanced, layer->instancedDataList.size());
#else
        pen::Renderer::Draw(iosCommandEncoder, layer->va, layer->ib, layer->indexCount, layer->vb, shader, 0, layer->shapeType, layer->isInstanced, layer->instancedDataList.size());
#endif
    }

    void Render::TextureSet() {
        pen::State* inst = pen::State::Get();
        pen::Render* render = pen::Render::Get();

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
        if (render->firstTime) {
            for (int j = 0; j < inst->textureUnits; j++) {
                render->appShader.SetUniform1i("uTextures[" + std::to_string(j) + "]", j);
                render->instancedShader.SetUniform1i("uTextures[" + std::to_string(j) + "]", j);
            }
        }
    }

    void Render::UpdatedInstancedUniforms(pen::Layer* layer) {
        /*Update the uniforms for the instanced shader*/
        pen::Render* render = pen::Render::Get();
        int vecCount = layer->instancedDataList.size() > 400 ? 400 : layer->instancedDataList.size();
        for (int i = 0; i < vecCount; i++) {
            render->instancedShader.SetUniform3f("uInstancedOffsets[" + std::to_string(i) + "]", layer->instancedDataList[i]);
        }
    }

#ifdef __PEN_IOS__
    void Render::RenderIOSLayers(MTK::View* pView) {
        /*Render an ios layer*/
        pen::Render::Get()->iosView = pView;
        for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
            pen::Render::RenderLayer(pen::ui::LM::layers[i]);
        }
    }

    void DrawIOSView(MTL::RenderCommandEncoder* commandEncoder, const VertexBuffer& vb) {
        /*Use the ios view to render batch data*/
        NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

        MTL::CommandBuffer* pCmd = pen::State::Get()->iosCommandQueue->commandBuffer();
        MTL::RenderPassDescriptor* pRpd = pen::Render::Get()->iosView->currentRenderPassDescriptor();
        commandEncoder = pCmd->renderCommandEncoder(pRpd);

        commandEncoder->setRenderPipelineState(pen::State::Get()->iosPipelineState);
        commandEncoder->setVertexBuffer(vb.iosBuffer, 0, 0);
        commandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

        commandEncoder->endEncoding();
        pCmd->presentDrawable(pView->currentDrawable());
        pCmd->commit();

        pPool->release();
    }
#endif
}
