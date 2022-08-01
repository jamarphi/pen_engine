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
#import "platforms/ios/ios_view.h"
#import <QuartzCore/QuartzCore.h>

#define IOS_MAX_TOUCHES_COUNT     10

@interface PenIOSView ()
@property(nonatomic, assign) CGRect originalRect;
@end

@implementation PenIOSView

@synthesize surfaceSize=size_;
@synthesize pixelFormat=pixelformat_, depthFormat=depthFormat_;
@synthesize multiSampling=multiSampling_;
@synthesize originalRect = originalRect_;

+ (id) viewWithFrame:(CGRect)frame
{
    return [[[self alloc] initWithFrame:frame] autorelease];
}

+ (id) viewWithFrame:(CGRect)frame pixelFormat:(NSString*)format
{
    return [[[self alloc]initWithFrame:frame pixelFormat:format] autorelease];
}

+ (id) viewWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth
{
    return [[[self alloc] initWithFrame:frame pixelFormat:format depthFormat:depth preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0] autorelease];
}

+ (id) viewWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained sharegroup:(EAGLSharegroup*)sharegroup multiSampling:(BOOL)multisampling numberOfSamples:(unsigned int)samples
{
    return [[[self alloc]initWithFrame:frame pixelFormat:format depthFormat:depth preserveBackbuffer:retained sharegroup:sharegroup multiSampling:multisampling numberOfSamples:samples] autorelease];
}

- (id) initWithFrame:(CGRect)frame
{
    return [self initWithFrame:frame pixelFormat:kEAGLColorFormatRGB565 depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0];
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format 
{
    return [self initWithFrame:frame pixelFormat:format depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0];
}

-(id) initWithCoder:(NSCoder *)aDecoder
{
    if ( (self = [super initWithCoder:aDecoder]) )
    {
        size_ = [self bounds].size;
    }
    
    return self;
}


-(int) getWidth
{
    CGSize bound = [self bounds].size;
    return (int)bound.width * self.contentScaleFactor;
}

-(int) getHeight
{
    CGSize bound = [self bounds].size;
    return (int)bound.height * self.contentScaleFactor;
}

- (void) dealloc
{
    [self.textInputView release];
    [super dealloc];
}

- (void) layoutSubviews
{
    if (!pen::State::Get()->mobileActive)
        return;

    size_ = [self bounds].size;
    size_.width *= self.contentScaleFactor;
    size_.height *= self.contentScaleFactor;
}

- (void) swapBuffers
{
}

#pragma mark PenIOSView - Point conversion

- (CGPoint) convertPointFromViewToSurface:(CGPoint)point
{
    CGRect bounds = [self bounds];
    
    CGPoint ret;
    ret.x = (point.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.y =  (point.y - bounds.origin.y) / bounds.size.height * size_.height;
    
    return ret;
}

- (CGRect) convertRectFromViewToSurface:(CGRect)rect
{
    CGRect bounds = [self bounds];
    
    CGRect ret;
    ret.origin.x = (rect.origin.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.origin.y = (rect.origin.y - bounds.origin.y) / bounds.size.height * size_.height;
    ret.size.width = rect.size.width / bounds.size.width * size_.width;
    ret.size.height = rect.size.height / bounds.size.height * size_.height;
    
    return ret;
}

// Pass the touches to the superview
#pragma mark PenIOSView - Touch Delegate
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (self.isKeyboardShown)
        [self closeKeyboardOpenedByEditBox];
    
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    
    ids[0] = touches[0];
    xs[0] = [touches[0] locationInView: [touches[0] view]].x * self.contentScaleFactor;
    ys[0] = [touches[0] locationInView: [touches[0] view]].y * self.contentScaleFactor;

    pen::State::Get()->mobileMouseX = (double)xs[0];
    pen::State::Get()->mobileMouseY = (double)ys[0];
    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::PRESSED, 0);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    
    ids[0] = touches[0];
    xs[0] = [touches[0] locationInView: [touches[0] view]].x * self.contentScaleFactor;
    ys[0] = [touches[0] locationInView: [touches[0] view]].y * self.contentScaleFactor;

    pen::State::Get()->mobileMouseX = (double)xs[0];
    pen::State::Get()->mobileMouseY = (double)ys[0];
    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::RELEASED, 0);
}

#pragma UIKeyboard notification

namespace {
    UIInterfaceOrientation getFixedOrientation(UIInterfaceOrientation statusBarOrientation)
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)
        {
            statusBarOrientation = UIInterfaceOrientationPortrait;
        }
        return statusBarOrientation;
    }
}

- (void)didMoveToWindow
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardWillShowNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardDidShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardWillHideNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardDidHideNotification object:nil];
}

@end