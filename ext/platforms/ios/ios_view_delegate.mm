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
PenMTKViewDelegate::PenMTKViewDelegate()
    : MTK::ViewDelegate()
{
}

void PenMTKViewDelegate::DrawInMTKView(MTK::View* pView)
{
    /*Draws data in MTK view*/
    pen::State::Get()->iosMtkView = pView;
    (*pen::State::Get()->mobileOnRenderCallback)();
}

void PenMTKViewDelegate::UpdateUniforms(pen::Mat4x4 mvp){
	/*Updates the uniform data*/
	simd::float4 colA = {mvp.matrix[0][0], mvp.matrix[1][0], mvp.matrix[2][0], mvp.matrix[3][0]};
	simd::float4 colB = {mvp.matrix[0][1], mvp.matrix[1][1], mvp.matrix[2][1], mvp.matrix[3][1]};
	simd::float4 colC = {mvp.matrix[0][2], mvp.matrix[1][2], mvp.matrix[2][2], mvp.matrix[3][2]};
	simd::float4 colD = {mvp.matrix[0][3], mvp.matrix[1][3], mvp.matrix[2][3], mvp.matrix[3][3]};
	simd::float4x4 mat = simd::float4x4::init(colA, colB, colC, colD);

	IOSUniformData* data = new IOSUniformData[1];
	data.uMVP = mat;

	int size = sizeof(IOSUniformData);

	iosUniformBuffer->release();

	iosUniformBuffer = IOSState::Get()->iosDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
	std::memcpy(iosUniformBuffer->contents(), data, size);
	iosUniformBuffer->didModifyRange(NS::Range::Make(0, iosUniformBuffer->length()));
}

void PenMTKViewDelegate::SubmitBatch(IOSVertexBuffer* iosVertexBuffer, BatchVertexData* data, int size, pen::Mat4x4 mvp){
    /*Submits the vertex data to the GPU*/
    std::memcpy(iosVertexBuffer->iosVertexBuffer->contents(), data, size);
    iosVertexBuffer->iosVertexBuffer->didModifyRange(NS::Range::Make(0, iosVertexBuffer->iosVertexBuffer->length()));

    PenMTKViewDelegate::UpdateUniforms(mvp);
    PenMTKViewDelegate::DrawIOSView(iosVertexBuffer);
}

void PenMTKViewDelegate::DrawIOSView(IOSVertexBuffer* iosVertexBuffer) {
    /*Use the ios mtk view to render batch data*/
    IOSState* inst = pen::State::Get();
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    MTL::CommandBuffer* pCmd = inst->iosCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = inst->iosMtkView->currentRenderPassDescriptor();
    inst->iosCommandEncoder = pCmd->renderCommandEncoder(pRpd);
    inst->iosCommandBuffer = pCmd;
    inst->iosAutoReleasePool = pPool;

    inst->iosCommandEncoder->setRenderPipelineState(inst->iosPipelineState);
    inst->iosCommandEncoder->setDepthStencilState(inst->iosDepthStencilState);
    inst->iosCommandEncoder->setVertexBuffer(iosVertexBuffer->iosVertexBuffer, 0, 0);
    inst->iosCommandEncoder->setVertexBuffer(inst->iosMVPBuffer, 0, 1);
    inst->iosCommandEncoder->setFragmentTextures(inst->iosTextures, 0);
    inst->iosCommandEncoder->setCullMode(MTL::CullModeBack);
    inst->iosCommandEncoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);
}

void PenMTKViewDelegate::Render(unsigned int shapeType, int indexCount, IOSIndexBuffer* iosIndexBuffer, unsigned int instanceCount){
    /*Render the ios mtk view*/
    unsigned int type = 0;
    IOSState* inst = IOSState::Get();

    switch (shapeType) {
    case 0:
        type = MTL::PrimitiveType::PrimitiveTypePoint;
        break;
    case 1:
        type = MTL::PrimitiveType::PrimitiveTypeLine;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        type = MTL::PrimitiveType::PrimitiveTypeTriangle;
        break;
    default:
        break;
    }

    inst->iosCommandEncoder->drawIndexedPrimitives(type, indexCount, MTL::IndexType::IndexTypeUInt16, iosIndexBuffer, 0, instanceCount);
    inst->iosCommandEncoder->endEncoding();
    inst->iosCommandBuffer->presentDrawable(inst->iosMtkView->currentDrawable());
    inst->iosCommandBuffer->commit();
    inst->iosAutoReleasePool->release();
}

void PenMTKViewDelegate::Background(float r, float g, float b, float a){
    /*Updates the background of the mtk window*/
    IOSState::Get()->iosMtkView->setClearColor(MTL::ClearColor::Make(r, g, b, a));
}
#endif