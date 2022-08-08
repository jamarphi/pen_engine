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
#import "../../../src/state/config.h"

#ifdef __PEN_IOS__
#import "ios_config.h"
#import "ios_vertex_buffer.h"
#import "../../../src/state/state.h"
#import "../../../src/ops/matrices/mat4x4.h"

#define MVP_MATRIX_SIZE sizeof(float) * 16

class PenMTKViewDelegate : public MTK::ViewDelegate
{
public:
    PenMTKViewDelegate();
    virtual void DrawInMTKView(MTK::View* pView) override;
    static void UpdateUniforms(pen::Mat4x4 mvp);
    static void SubmitBatch(IOSVertexBuffer* iosVertexBuffer, void* data, int size, pen::Mat4x4 mvp);
    static void DrawIOSView(IOSVertexBuffer* iosVertexBuffer);
    static void Render(unsigned int shapeType, int indexCount, IOSIndexBuffer* iosIndexBuffer, unsigned int instanceCount);
    static void Background();
};
#endif
