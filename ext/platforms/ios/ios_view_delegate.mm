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
#include "ios_view_delegate.h"

#ifdef __PEN_IOS__
@implementation PenMTKViewDelegate

- (void) drawInMTKView: (MTKView*) pView
{
    /*Draws data in MTK view*/
    IOSState* inst = [IOSState Get];
    inst.iosMtkView = pView;
    (*pen::State::Get()->mobileOnRenderCallback)();
}

void MapIOSUpdateUniforms(pen::Mat4x4 mvp){
    /*Updates the uniform data*/
    [PenMTKViewDelegate UpdateUniforms:mvp];
}

+ (void) UpdateUniforms: (pen::Mat4x4) mvp{
	/*Updates the uniform data*/
    IOSState* inst = [IOSState Get];
	simd::float4 colA = {mvp.matrix[0][0], mvp.matrix[1][0], mvp.matrix[2][0], mvp.matrix[3][0]};
	simd::float4 colB = {mvp.matrix[0][1], mvp.matrix[1][1], mvp.matrix[2][1], mvp.matrix[3][1]};
	simd::float4 colC = {mvp.matrix[0][2], mvp.matrix[1][2], mvp.matrix[2][2], mvp.matrix[3][2]};
	simd::float4 colD = {mvp.matrix[0][3], mvp.matrix[1][3], mvp.matrix[2][3], mvp.matrix[3][3]};
	simd::float4x4 mat = simd::float4x4(colA, colB, colC, colD);

	IOSUniformData* data = new IOSUniformData[1];
	data[0].uMVP = mat;

	int size = sizeof(IOSUniformData);

    if(inst.iosUniformBuffer == nil){
#ifndef TARGET_OS_IOS
        inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeManaged];
#else
        inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeShared];
#endif
    }
    
    std::memcpy([inst.iosUniformBuffer contents], data, size);
#ifndef TARGET_OS_IOS
    [inst.iosUniformBuffer didModifyRange: NSMakeRange(0, [inst.iosUniformBuffer length])];
#endif
}

void MapIOSSubmitBatch(unsigned int layerId, BatchVertexData* data, int size, pen::Mat4x4 mvp){
    /*Submits the vertex data to the GPU*/
    NSMutableDictionary* argumentBuffers = [IOSArgumentBuffer IOSArgumentBuffersGet];
    NSMutableDictionary* vertexBuffers = [IOSVertexBuffer IOSVertexBuffersGet];
    [PenMTKViewDelegate SubmitBatch:[argumentBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]]
                                   :[vertexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]]
                                   :data :size :mvp];
}

+ (void) SubmitBatch: (id<MTLBuffer>) iosArgumentBuffer
    :(id<MTLBuffer>) iosVertexBuffer
    :(BatchVertexData*) data
    :(int) size
    :(pen::Mat4x4) mvp{
    /*Submits the vertex data to the GPU*/
    std::memcpy([iosVertexBuffer contents], data, size);
#ifndef TARGET_OS_IOS
    [iosVertexBuffer didModifyRange: NSMakeRange(0, [iosVertexBuffer length])];
#endif
    [PenMTKViewDelegate DrawIOSView:iosArgumentBuffer :iosVertexBuffer];
}

+ (void) DrawIOSView: (id<MTLBuffer>) iosArgumentBuffer
                      :(id<MTLBuffer>) iosVertexBuffer {
    /*Use the ios mtk view to render batch data*/
    IOSState* inst = [IOSState Get];
    dispatch_semaphore_wait(inst.dispatchSemaphore, DISPATCH_TIME_FOREVER);
    id<MTLCommandBuffer> pCmd = [inst.iosCommandQueue commandBuffer];
    MTLRenderPassDescriptor* pRpd = [inst.iosMtkView currentRenderPassDescriptor];
    inst.iosCommandEncoder = [pCmd renderCommandEncoderWithDescriptor:pRpd];
    inst.iosCommandBuffer = pCmd;
    
    [pCmd addCompletedHandler:^(id<MTLCommandBuffer> dispatchCallback) {
        dispatch_semaphore_signal( inst.dispatchSemaphore );
    }];

    [inst.iosCommandEncoder setRenderPipelineState:inst.iosPipelineState];
    [inst.iosCommandEncoder setDepthStencilState:inst.iosDepthStencilState];
    [inst.iosCommandEncoder setVertexBuffer:iosArgumentBuffer offset:0 atIndex:0];
    [inst.iosCommandEncoder useResource:iosVertexBuffer usage:MTLResourceUsageRead];
    [inst.iosCommandEncoder setVertexBuffer:inst.iosUniformBuffer offset:0 atIndex:1];
    [inst.iosCommandEncoder setVertexBuffer:inst.iosInstanceBuffer offset:0 atIndex:2];
    NSMutableArray * textures = [IOSState GetTextures];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:0] atIndex:0];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:1] atIndex:1];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:2] atIndex:2];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:3] atIndex:3];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:4] atIndex:4];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:5] atIndex:5];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:6] atIndex:6];
    [inst.iosCommandEncoder setFragmentTexture:[textures objectAtIndex:7] atIndex:7];
    [inst.iosCommandEncoder setCullMode:MTLCullModeBack];
    [inst.iosCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
}

void MapIOSRender(unsigned int shapeType, int indexCount, unsigned int layerId, unsigned int instanceCount){
    /*Render the ios mtk view*/
    NSMutableDictionary* indexBuffers = [IOSIndexBuffer IOSIndexBuffersGet];
    [PenMTKViewDelegate Render:shapeType :indexCount :[indexBuffers objectForKey:[NSString stringWithFormat:@"%d", layerId]] :instanceCount];
}

+ (void) Render: (unsigned int) shapeType
                 :(int) indexCount
                 :(id<MTLBuffer>) iosIndexBuffer
                 :(unsigned int) instanceCount{
    /*Render the ios mtk view*/
    MTLPrimitiveType type;
    IOSState* inst = [IOSState Get];

    switch (shapeType) {
    case 0:
        type = MTLPrimitiveTypePoint;
        break;
    case 1:
        type = MTLPrimitiveTypeLine;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        type = MTLPrimitiveTypeTriangle;
        break;
    default:
        break;
    }

    [inst.iosCommandEncoder drawIndexedPrimitives:type indexCount:indexCount indexType:MTLIndexTypeUInt16 indexBuffer:iosIndexBuffer indexBufferOffset:0 instanceCount:instanceCount];
    [inst.iosCommandEncoder endEncoding];
    [inst.iosCommandBuffer presentDrawable:[inst.iosMtkView currentDrawable]];
    [inst.iosCommandBuffer commit];
}

void MapIOSBackground(float r, float g, float b, float a){
    /*Updates the background of the mtk window*/
    [PenMTKViewDelegate Background:r :g :b :a];
}

+ (void) Background: (float) r
:(float) g
:(float) b
:(float) a{
    /*Updates the background of the mtk window*/
    IOSState* inst = [IOSState Get];
    [inst.iosMtkView setClearColor:MTLClearColorMake(r, g, b, a)];
}
@end
#endif
