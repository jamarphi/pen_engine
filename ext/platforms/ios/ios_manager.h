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

#include "../../../src/pen_engine.h"
#ifdef __PEN_IOS__
/*
  This file is for the instantiation of Pen Engine by the user in order to include it's OnCreate function.
  In the OnCreate function it is expected that pen::Pen::SetMobileCallbacks(); is called.
*/
#include "../../../../app.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <CoreFoundation/CoreFoundation.h>

@interface PenIOSManager : NSObject {
    id displayLink;
    int interval;
    BOOL isAppActive;
    CFTimeInterval lastDisplayTime;
}
@property(readwrite) int interval;
-(void)startMainLoop;
-(void)stopMainLoop;
-(void)doCaller: (id)sender;
-(void)setAnimationInterval:(double)interval;
+(id)sharedIosManager;
+(void)destroy;
@end
#endif