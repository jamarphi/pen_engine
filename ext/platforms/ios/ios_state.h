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
#import <cassert>
#ifndef NS_PRIVATE_IMPLEMENTATION
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#endif
#ifdef TARGET_IPHONE_SIMULATOR
#import <UIKit/UIKit.h>
#elifdef TARGET_OS_IPHONE
#import <UIKit/UIKit.hpp>
#elifdef TARGET_OS_IOS
#import <UIKit/UIKit.hpp>
#elifdef TARGET_OS_MAC
#import <AppKit/AppKit.hpp>
#endif
#import <Metal/Metal.hpp>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <MetalKit/MetalKit.hpp>
#import <simd/simd.h>

struct BatchVertexData {
	simd::float3 vertex;
	simd::float4 color;
	simd::float2 texCoord;
	simd::float texId;
};

struct IOSUniformData {
	simd::float4x4 uMVP;
}

struct IOSInstanceData {
	simd::float3 uInstancedOffsets;
};

class IOSState {
	static IOSState* instance;
public:
	MTK::View* iosMtkView;
	MTL::Device* iosDevice;
	MTL::CommandQueue* iosCommandQueue;
	MTL::RenderPipelineState* iosPipelineState;
	NS::Notification* iosLaunchNotification;
	MTL::ArgumentEncoder* iosArgEncoder;
	MTL::DepthStencilState* iosDepthStencilState;
	MTL::Texture* iosPixelBuffer;
	MTL::Buffer* iosUniformBuffer;
	MTL::Buffer* iosInstanceBuffer;
	MTL::Texture* iosTextures[8];
	MTL::RenderCommandEncoder* iosCommandEncoder;
	MTL::CommandBuffer* iosCommandBuffer;
	NS::AutoreleasePool* iosAutoReleasePool;
	NS::AutoreleasePool* iosAutoReleasePool;
	dispatch_semaphore_t dispatchSemaphore;

public:
	static IOSState* Get() {
		if (!instance)
			instance = new IOSState();
		return instance;
	}
};
#endif
