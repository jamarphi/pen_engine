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
#import "mac_ios_state.h"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "mac_ios_vertex_buffer.h"
#import "mac_ios_argument_buffer.h"
#import "mac_ios_index_buffer.h"
/*
  This file is for the instantiation of Pen Engine by the user in order to include it's OnCreate function.
  In the OnCreate function it is expected that pen::Pen::SetMobileCallbacks(); is called.
*/
#import "../../../../app.h"

#define MVP_MATRIX_SIZE sizeof(float) * 16

@interface PenMacIOSMTKViewDelegate : MTKView

    /*Virtual methods inherited from MTKView*/
    - (void) drawRect:(CGRect)rect;
#ifndef TARGET_OS_IOS
    - (BOOL)acceptsFirstResponder;
    - (void)mouseDown:(NSEvent *)event;
    - (void)mouseDragged:(NSEvent *)event;
    - (void)mouseUp:(NSEvent *)event;
    - (void)keyDown:(NSEvent *)event;
    - (void)keyUp:(NSEvent *)event;
    - (NSSize)windowWillResize:(NSWindow *)sender
                    toSize:(NSSize)frameSize;
#else
    - (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
    - (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
    - (void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
#endif

    + (void) UpdateUniforms: (pen::Mat4x4) mvp;
    + (void) SubmitBatch: (id<MTLBuffer>) iosArgumentBuffer
                         :(id<MTLBuffer>) iosVertexBuffer
                         :(BatchVertexData*) data
                         :(int) size
                        :(pen::Mat4x4) mvp;
    + (void) DrawIOSView: (id<MTLBuffer>) iosArgumentBuffer
                        :(id<MTLBuffer>) iosVertexBuffer;
    + (void) Render: (unsigned int) shapeType
                     :(int) indexCount
                     :(id<MTLBuffer>) iosIndexBuffer
                     :(unsigned int) instanceCount;
    + (void) Background: (float) r
                     : (float) g
                     : (float) b
                     : (float) a;

@end
#endif
