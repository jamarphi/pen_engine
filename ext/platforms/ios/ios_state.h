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

#include "../../../src/state/config.h"

#ifdef __PEN_IOS__
#import <cassert>
#ifndef NS_PRIVATE_IMPLEMENTATION
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

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
#import <CoreFoundation/CoreFoundation.h>
#import <QuartzCore/QuartzCore.h>
#import <MetalKit/MetalKit.hpp>
#import <simd/simd.h>
#import "ios_cpp_objective_c_mapping.h"
#endif

@class IOSState;

@interface IOSState : NSObject
	@property MTK::View* iosMtkView;
    @property MTL::Device* iosDevice;
    @property MTL::CommandQueue* iosCommandQueue;
    @property MTL::RenderPipelineState* iosPipelineState;
    @property NS::Notification* iosLaunchNotification;
    @property MTL::ArgumentEncoder* iosArgEncoder;
    @property MTL::DepthStencilState* iosDepthStencilState;
    @property MTL::Texture* iosPixelBuffer;
    @property MTL::Buffer* iosUniformBuffer;
    @property MTL::Buffer* iosInstanceBuffer;
    @property MTL::Texture* iosTextures[8];
    @property MTL::RenderCommandEncoder* iosCommandEncoder;
    @property MTL::CommandBuffer* iosCommandBuffer;
    @property NS::AutoreleasePool* iosAutoReleasePool;
    @property dispatch_semaphore_t dispatchSemaphore;

+ (IOSState*) Get;
+ (void) Destroy;
@end
#endif
