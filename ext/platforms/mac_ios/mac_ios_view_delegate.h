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
#import "../../../src/state/state.h"
#import "mac_ios_state.h"
#import "mac_ios_vertex_buffer.h"
#import "mac_ios_index_buffer.h"
#if !TARGET_OS_OSX
#import <CoreMotion/CoreMotion.h>
#endif
/*
  This file is for the instantiation of Pen Engine by the user in order to include it's OnCreate function.
  In the OnCreate function it is expected that pen::Pen::SetMobileCallbacks(); is called.
*/
#import "../../../../app.h"

@interface PenMacIOSMTKViewDelegate : NSObject<MTKViewDelegate>

@property(nonatomic, strong) id<CAMetalDrawable> _Nullable previousDrawable;
@property(nonatomic, strong) id<MTLTexture> _Nullable previousTexture;
#if !TARGET_OS_OSX
@property(nonatomic, strong) CMMotionManager* _Nonnull  motionManager;
#endif

    /*Virtual method inherited from MTKView*/
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view size:(CGSize)size;
- (void)drawInMTKView:(nonnull MTKView *)view;
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size;
#if TARGET_OS_OSX
    - (BOOL)acceptsFirstResponder;
    - (void)mouseMoved:(NSEvent*)event;
    - (void)mouseDown:(NSEvent *)event;
    - (void)mouseDragged:(NSEvent *)event;
    - (void)mouseUp:(NSEvent *)event;
    - (void)keyDown:(NSEvent *)event;
    - (void)keyUp:(NSEvent *)event;
    - (NSSize)windowWillResize:(NSWindow *)sender
        toSize:(NSSize)frameSize;
    +(void)MakeMouseHidden;
    +(void)MakeMouseShow;
    +(BOOL)WindowActive;
#else
    - (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
    - (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
    - (void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
#endif

    + (PenMacIOSMTKViewDelegate*) Get;
    + (void) AddUniform:(unsigned int) layerId :(pen::Mat4x4) mvp;
    + (void) UpdateUniforms;
    + (void) SubmitBatch:(id<MTLBuffer>) iosVertexBuffer
                         :(BatchVertexData*) data
                        :(int) size;
    + (void) Render:(unsigned int) shapeType;
    + (void) Background: (float) r
                     : (float) g
                     : (float) b
                     : (float) a;

@end
#endif
