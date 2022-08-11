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

#include "../../../src/ops/matrices/mat4x4.h"

#ifdef __PEN_IOS__
#import <simd/simd.h>
#endif

extern "C" {

    /*BatchVertexData already defined in renderer.h*/
    #ifdef __PEN_IOS__
    struct BatchVertexData {
        simd::float3 vertex;
        simd::float4 color;
        simd::float2 texCoord;
        simd::float1 texId;
    };
    #endif

    struct IOSUniformData {
        simd::float4x4 uMVP;
    };

    struct IOSInstanceData {
        simd::float3 uInstancedOffsets;
    };

//    class IOS_CPPObjectCMapping {
//    public:
//        /*----ios_config----*/
//        static void MapIOSConfigInit(unsigned int width, unsigned int height, const char* appName);
//        /*----ios_config----*/
//
//        /*----ios_view_delegate----*/
//        static void MapIOSUpdateUniforms(pen::Mat4x4 mvp);
//        static void MapIOSSubmitBatch(unsigned int layerId, BatchVertexData* data, int size, pen::Mat4x4 mvp);
//        static void MapIOSRender(unsigned int shapeType, int indexCount, unsigned int layerId, unsigned int instanceCount);
//        static void MapIOSBackground(float r, float g, float b, float a);
//        /*----ios_view_delegate----*/
//
//        /*----ios_vertex_buffer----*/
//        static void MapIOSVertexBufferInit(unsigned int layerId, BatchVertexData* data, unsigned int size);
//        static void MapIOSVertexBufferDestroy(unsigned int layerId);
//        /*----ios_vertex_buffer----*/
//
//        /*----ios_argument_buffer----*/
//        static void MapIOSArgumentBufferInit(unsigned int layerId);
//        static void MapIOSArgumentBufferBind(unsigned int layerId);
//        static void MapIOSArgumentBufferDestroy(unsigned int layerId);
//        /*----ios_argument_buffer----*/
//
//        /*----ios_index_buffer----*/
//        static void MapIOSIndexBufferInit(unsigned int layerId, int* data, unsigned int count);
//        static void MapIOSIndexBufferDestroy(unsigned int layerId);
//        /*----ios_index_buffer----*/
//
//        /*----ios_shader----*/
//        static void MapIOSShaderInit(const char* shaderProgram);
//        static void MapIOSUpdateInstanceUniform(IOSInstanceData* data);
//        /*----ios_shader----*/
//
//        /*----ios_texture----*/
//        static void MapIOSInitializeTexture(unsigned int texWidth, unsigned int texHeight, unsigned int type, unsigned int texSlot, unsigned char* textureData);
//        static void MapIOSUpdatePixels();
//        /*----ios_texture----*/
//    };

    /*----ios_config----*/
    static void MapIOSConfigInit(unsigned int width, unsigned int height, const char* appName);
    /*----ios_config----*/

    /*----ios_view_delegate----*/
    static void MapIOSUpdateUniforms(pen::Mat4x4 mvp);
    static void MapIOSSubmitBatch(unsigned int layerId, BatchVertexData* data, int size, pen::Mat4x4 mvp);
    static void MapIOSRender(unsigned int shapeType, int indexCount, unsigned int layerId, unsigned int instanceCount);
    static void MapIOSBackground(float r, float g, float b, float a);
    /*----ios_view_delegate----*/

    /*----ios_vertex_buffer----*/
    static void MapIOSVertexBufferInit(unsigned int layerId, BatchVertexData* data, unsigned int size);
    static void MapIOSVertexBufferDestroy(unsigned int layerId);
    /*----ios_vertex_buffer----*/

    /*----ios_argument_buffer----*/
    static void MapIOSArgumentBufferInit(unsigned int layerId);
    static void MapIOSArgumentBufferBind(unsigned int layerId);
    static void MapIOSArgumentBufferDestroy(unsigned int layerId);
    /*----ios_argument_buffer----*/

    /*----ios_index_buffer----*/
    static void MapIOSIndexBufferInit(unsigned int layerId, int* data, unsigned int count);
    static void MapIOSIndexBufferDestroy(unsigned int layerId);
    /*----ios_index_buffer----*/

    /*----ios_shader----*/
    static void MapIOSShaderInit(const char* shaderProgram);
    static void MapIOSUpdateInstanceUniform(IOSInstanceData* data);
    /*----ios_shader----*/

    /*----ios_texture----*/
    static void MapIOSInitializeTexture(unsigned int texWidth, unsigned int texHeight, unsigned int type, unsigned int texSlot, unsigned char* textureData);
    static void MapIOSUpdatePixels();
    /*----ios_texture----*/
};
