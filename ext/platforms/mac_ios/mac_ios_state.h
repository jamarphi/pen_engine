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

#ifdef __PEN_MAC_IOS__
#import <TargetConditionals.h>
#ifdef TARGET_IPHONE_SIMULATOR
#import <UIKit/UIKit.h>
#elifdef TARGET_OS_IPHONE
#import <UIKit/UIKit.hpp>
#elifdef TARGET_OS_IOS
#import <UIKit/UIKit.hpp>
#elifdef TARGET_OS_MAC
#import <AppKit/AppKit.hpp>
#endif
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/QuartzCore.h>
#import <MetalKit/MetalKit.h>
#import <simd/simd.h>
#import "mac_ios_cpp_objective_c_mapping.h"

@class IOSState;

@interface IOSState : NSObject

	@property MTKView* iosMtkView;
    @property id<MTLDevice> iosDevice;
    @property id<MTLCommandQueue> iosCommandQueue;
    @property id<MTLRenderPipelineState> iosPipelineState;
    @property NSNotification* iosLaunchNotification;
    @property id<MTLArgumentEncoder> iosArgEncoder;
    @property id<MTLDepthStencilState> iosDepthStencilState;
    @property id<MTLTexture> iosPixelBuffer;
    @property id<MTLBuffer> iosUniformBuffer;
    @property id<MTLBuffer> iosInstanceBuffer;
    @property id<MTLRenderCommandEncoder> iosCommandEncoder;
    @property id<MTLCommandBuffer> iosCommandBuffer;
    @property dispatch_semaphore_t dispatchSemaphore;
#ifndef TARGET_OS_IOS
    @property NSWindow* iosWindow;
#endif

+ (IOSState*) Get;
+ (NSMutableArray*) GetTextures;
@end
#endif
