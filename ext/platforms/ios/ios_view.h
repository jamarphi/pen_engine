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
#include "../../../src/state/config.h"
#include "../../../src/state/state.h"
#ifdef __PEN_IOS__
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <CoreFoundation/CoreFoundation.h>

//CLASS INTERFACE:

/*
 * This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 * The view content is basically an EAGL surface you render your OpenGL scene into.
 * Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface PenIOSView : UIView
{
    BOOL                    preserveBackbuffer_;
    CGRect                  safeArea_;
    BOOL                    discardFramebufferSupported_;

    //fsaa addition
    unsigned int            requestedSamples_;
}

/** creates an initializes an PenIOSView with a frame and 0-bit depth buffer, and a RGB565 color buffer */
+ (id)viewWithFrame:(CGRect)frame;
/** creates an initializes an PenIOSView with a frame, a color buffer format, and 0-bit depth buffer */
+(id)viewWithFrame:(CGRect)frame pixelFormat : (NSString*)format;
/** creates an initializes an PenIOSView with a frame, a color buffer format, and a depth buffer format */
+(id)viewWithFrame:(CGRect)frame pixelFormat : (NSString*)format depthFormat : (GLuint)depth;
/** creates an initializes an PenIOSView with a frame, a color buffer format, a depth buffer format, a sharegroup, and multisampling */
+(id)viewWithFrame:(CGRect)frame pixelFormat : (NSString*)format depthFormat : (GLuint)depth preserveBackbuffer : (BOOL)retained sharegroup : (EAGLSharegroup*)sharegroup multiSampling : (BOOL)multisampling numberOfSamples : (unsigned int)samples;

/** Initializes an PenIOSView with a frame and 0-bit depth buffer, and a RGB565 color buffer */
-(id)initWithFrame:(CGRect)frame; //These also set the current context
/** Initializes an PenIOSView with a frame, a color buffer format, and 0-bit depth buffer */
-(id)initWithFrame:(CGRect)frame pixelFormat : (NSString*)format;
/** Initializes an PenIOSView with a frame, a color buffer format, a depth buffer format, a sharegroup and multisampling support */
-(id)initWithFrame:(CGRect)frame pixelFormat : (NSString*)format depthFormat : (GLuint)depth preserveBackbuffer : (BOOL)retained sharegroup : (EAGLSharegroup*)sharegroup multiSampling : (BOOL)sampling numberOfSamples : (unsigned int)nSamples;

//// * pixel format: it could be RGBA8 (32-bit) or RGB565 (16-bit)
@property(nonatomic, readonly) NSString* pixelFormat;
/** depth format of the render buffer: 0, 16 or 24 bits*/
@property(nonatomic, readonly) GLuint depthFormat;

/** returns surface size in pixels */
@property(nonatomic, readonly) CGSize surfaceSize;

@property(nonatomic, readwrite) BOOL multiSampling;


/** PenIOSView uses double-buffer. This method swaps the buffers */
-(void)swapBuffers;

-(CGRect)convertRectFromViewToSurface:(CGRect)rect;
-(CGPoint)convertPointFromViewToSurface:(CGPoint)point;

-(int)getWidth;
-(int)getHeight;
@end
#endif