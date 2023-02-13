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
#include "renderer.h"
#include <iostream>

namespace pen {
    /*Draw types based on which primitive shape is being used for a given layer*/
#ifndef __PEN_MAC_IOS__
    int Renderer::drawType[6] = { GL_POINTS,GL_LINES,GL_TRIANGLES,GL_TRIANGLES,GL_TRIANGLES, GL_TRIANGLES };
#endif

    void Renderer::Clear() {
        /*Clears the buffer before rendering*/
#ifndef __PEN_MAC_IOS__
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    }

#ifndef __PEN_MAC_IOS__
    void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, int& indexCount, const VertexBuffer& vb, const pen::Shader& shader, const unsigned int& shapeType,
        const bool& isInstanced, const unsigned int& instanceCount) {
        /*Draw a batched object to the screen

        This function goes through the pipeline and does not need to be called directly by you*/
        shader.Bind();
        va.Bind();
        vb.Bind();
        ib.Bind();
        
        isInstanced ? glDrawElementsInstanced(drawType[shapeType], indexCount, GL_UNSIGNED_INT, 0, instanceCount) : glDrawElements(drawType[shapeType],indexCount, GL_UNSIGNED_INT, 0);
    }
#else
    void Renderer::Draw() {
        /*Draw a batched object to the screen

        This function goes through the pipeline and does not need to be called directly by you*/
        MapMacIOSRender(3);
    }
#endif
}
