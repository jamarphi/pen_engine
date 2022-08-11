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

#ifdef __PEN_IOS__
@implementation IOSConfig
void MapIOSConfigInit(unsigned int width, unsigned int height, const char* appName){
    /*Handles app creation for ios*/
    [IOSConfig Init:width :height: appName];
}

+ (void) ConfigInit:(unsigned int) width :(unsigned int) height :(const char*) appName{
	/*Handles app creation for ios*/
    IOSState* inst = [IOSState Get];
    inst.iosCommandQueue = [inst.iosDevice newCommandQueue];

    CGRect frame = CGRectMake(0, 0, width, height);

#ifndef TARGET_OS_IOS
    NSWindow* iosWindow = [[NSWindow alloc] init: frame :NSWindowStyleMaskClosable | NSWindowStyleMaskTitled :NSBackingStoreBuffered :false];
#else
    UIWindow* iosWindow = [[UIWindow alloc] init: frame];
#endif

    inst.iosDevice = MTLCreateSystemDefaultDevice();

    inst.iosMtkView = [[MTKView alloc] initWithFrame: frame device:inst.iosDevice];
    [inst.iosMtkView setColorPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB];

    PenMTKViewDelegate iosViewDelegate = [[PenMTKViewDelegate alloc] init:inst.iosDevice];
    [inst.iosMtkView setDelegate:iosViewDelegate];

    [iosWindow setContentView: inst.iosMtkView];
    [iosWindow setTitle: [NSString stringWithUTF8String:appName]];
    [iosWindow makeKeyAndOrderFront:nil];

#ifndef TARGET_OS_IOS
    NSApplication* pApp = reinterpret_cast<NSApplication*>([inst.iosLaunchNotification object]);
#else
    UIApplication* pApp = reinterpret_cast<UIApplication*>([inst.iosLaunchNotification object]);
#endif
    [pApp activateIgnoringOtherApps:true];

    /*Initialize uniforms*/
#ifndef TARGET_OS_IOS
    inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:sizeof(float) * 16 options:MTLResourceStorageModeManaged];
#else
    inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:sizeof(float) * 16 options:MTLResourceStorageModeShared];
#endif
}
@end
#endif
