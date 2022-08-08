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
#include "ios_shader.h"

#ifdef __PEN_IOS__
IOSShader::IOSShader(const char* shaderProgram) {
	/*Creates a Metal shader*/
	using NS::StringEncoding::UTF8StringEncoding;

	NS::Error* pError = nullptr;
	MTL::Library* pLibrary = _pDevice->newLibrary(NS::String::string(shaderProgram, UTF8StringEncoding), nullptr, &pError);
	if (!pLibrary)
	{
		__builtin_printf("%s", pError->localizedDescription()->utf8String());
		assert(false);
	}

	MTL::Function* pVertexFn = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
	MTL::Function* pFragFn = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

	MTL::ArgumentEncoder* pArgEncoder = pVertexFn->newArgumentEncoder(0);
	IOSState::Get()->iosArgEncoder = pArgEncoder;

	MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
	pDesc->setVertexFunction(pVertexFn);
	pDesc->setFragmentFunction(pFragFn);
	pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

	IOSState::Get()->iosPipelineState = IOSState::Get()->iosDevice->newRenderPipelineState(pDesc, &pError);
	if (!IOSState::Get()->iosPipelineState)
	{
		__builtin_printf("%s", pError->localizedDescription()->utf8String());
		assert(false);
	}

	pVertexFn->release();
	pFragFn->release();
	pDesc->release();
	pLibrary->release();
}

void IOSShader::UpdateInstanceUniform(IOSInstanceData* data){
	/*Updates the instanced offsets*/
	int size = sizeof(InstanceData) * 400;
	MTL::Buffer* instanceBuffer = IOSState::Get()->iosDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
	std::memcpy(instanceBuffer->contents(), data, size);
	instanceBuffer->didModifyRange(NS::Range::Make(0, instanceBuffer->length()));
	IOSState::Get()->iosInstanceBuffer = instanceBuffer;
}
#endif