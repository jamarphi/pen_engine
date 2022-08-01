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
#import "platforms/ios/ios_manager.h"
#import "platforms/ios/ios_view.h"
#include <mach/mach_time.h>
#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>

static id s_sharedIosManager;

@interface NSObject(CADisplayLink)
+(id) displayLinkWithTarget: (id)arg1 selector:(SEL)arg2;
-(void) addToRunLoop: (id)arg1 forMode: (id)arg2;
-(void) setFrameInterval: (NSInteger)interval;
-(void) invalidate;
@end

@implementation PenIOSManager

@synthesize interval;

+(id) sharedIosManager
{
    if (s_sharedIosManager == nil)
    {
        s_sharedIosManager = [[PenIOSManager alloc] init];
    }
    
    return s_sharedIosManager;
}

+(void) destroy
{
    [s_sharedIosManager stopMainLoop];
    [s_sharedIosManager release];
    s_sharedIosManager = nil;
}


- (instancetype)init
{
    if (self = [super init])
    {
        isAppActive = [UIApplication sharedApplication].applicationState == UIApplicationStateActive;
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc addObserver:self selector:@selector(appDidBecomeActive) name:UIApplicationDidBecomeActiveNotification object:nil];
        [nc addObserver:self selector:@selector(appDidBecomeInactive) name:UIApplicationWillResignActiveNotification object:nil];
        
        self.interval = 1;
    }
    return self;
}

-(void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [displayLink release];
    [super dealloc];
}

- (void)appDidBecomeActive
{
    // initialize initLastDisplayTime, or the dt will be invalid when
    // - the app is lauched
    // - the app resumes from background
    [self initLastDisplayTime];

    isAppActive = YES;
}

- (void)appDidBecomeInactive
{
    isAppActive = NO;
}

-(void) startMainLoop
{
    // Director::setAnimationInterval() is called, we should invalidate it first
    [self stopMainLoop];
    
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
    [displayLink setFrameInterval: self.interval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    CGRect bounds = [self bounds];

    if (pen::State::Get()->screenWidth == 0) {
            /*Initialize app*/
            App* app = new App();
            std::string rootDir = ROOT_DIR;

            /*"pen_engine/" is removed at the end since it will be added on later*/
            app->CreateApplication("App", w, h, rootDir != "" ? rootDir.substr(0, rootDir.length() - 11) : "");

            pen::State::Get()->mobileActive = true;
            glViewport(0, 0, bounds.size.width, bounds.size.height);

            app->OnCreate();
    }
    else {
        /*Update window size*/
        pen::State::Get()->mobileActive = true;
        pen::State::Get()->actualScreenHeight = bounds.size.height;
        pen::State::Get()->actualScreenWidth = bounds.size.width;
    }
}

-(void) stopMainLoop
{
    [displayLink invalidate];
    displayLink = nil;
}

-(void) setAnimationInterval:(double)intervalNew
{
    // Director::setAnimationInterval() is called, we should invalidate it first
    [self stopMainLoop];
        
    self.interval = 60.0 * intervalNew;
        
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
    [displayLink setFrameInterval: self.interval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}
                      
-(void) doCaller: (id) sender
{
    /*Happens each frame*/
    if (isAppActive) {      
        if (pen::State::Get()->handleMobileRender != nullptr) {
            (*pen::State::Get()->handleMobileRender)();
        }
    }
}

-(void)initLastDisplayTime
{
    struct mach_timebase_info timeBaseInfo;
    mach_timebase_info(&timeBaseInfo);
    CGFloat clockFrequency = (CGFloat)timeBaseInfo.denom / (CGFloat)timeBaseInfo.numer;
    clockFrequency *= 1000000000.0;
    // convert absolute time to seconds and should minus one frame time interval
    lastDisplayTime = (mach_absolute_time() / clockFrequency) - ((1.0 / 60) * self.interval);
}

@end