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
#include "mac_ios_app_delegate.h"

#ifdef __PEN_MAC_IOS__

@implementation PenMacIOSAppDelegate

- (void) applicationWillFinishLaunching: (NSNotification*) pNotification
{
    /*Before application is done launching*/
#ifndef TARGET_OS_IOS
    NSApplication* pApp = reinterpret_cast<NSApplication*>([pNotification object]);
    pApp->setActivationPolicy(NSActivationPolicy::ActivationPolicyRegular);
#endif
}

- (void) applicationDidFinishLaunching: (NSNotification*) pNotification
{
    /*After application is done launching*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    MTLDepthStencilDescriptor* pDsDesc = [[MTLDepthStencilDescriptor alloc] init];
    [pDsDesc setDepthCompareFunction:MTLCompareFunctionLess];
    [pDsDesc setDepthWriteEnabled:true];
    inst.iosDepthStencilState = [inst.iosDevice newDepthStencilStateWithDescriptor:pDsDesc];
    inst.iosLaunchNotification = pNotification;

    /*Currently at max three different buffer types sent to metal shaders*/
    inst.dispatchSemaphore = dispatch_semaphore_create(3);
    
    /*Initialize static arrays*/
    NSMutableArray* textures = [PenMacIOSState GetTextures];
    NSMutableDictionary* vertexBuffers = [PenMacIOSVertexBuffer PenMacIOSVertexBuffersGet];
    NSMutableDictionary* argumentBuffers = [PenMacIOSArgumentBuffer PenMacIOSArgumentBuffersGet];
    NSMutableDictionary* indexBuffers = [PenMacIOSIndexBuffer PenMacIOSIndexBuffersGet];
    textures = [[NSMutableArray alloc] init];
    vertexBuffers = [NSMutableDictionary dictionary];
    argumentBuffers = [NSMutableDictionary dictionary];
    indexBuffers = [NSMutableDictionary dictionary];

    App* app = new App();
    pen::State::Get()->mobileActive = true;
    app->CreateApplication("App", 110, 540, "");    
    
    inst.iosCommandQueue = [inst.iosDevice newCommandQueue];
    CGRect frame = CGRectMake(0, 0, 110, 540);
#ifndef TARGET_OS_IOS
    NSWindow* iosWindow = [[NSWindow alloc] init: frame :NSWindowStyleMaskClosable | NSWindowStyleMaskTitled :NSBackingStoreBuffered :false];
#endif

    inst.iosDevice = MTLCreateSystemDefaultDevice();

    inst.iosMtkView = [[PenMacIOSMTKViewDelegate alloc] initWithFrame: frame device:inst.iosDevice];
    [inst.iosMtkView setColorPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB];

#ifndef TARGET_OS_IOS
    inst.iosWindow = iosWindow;
    [iosWindow setContentView: inst.iosMtkView];
    [iosWindow setTitle: [NSString stringWithUTF8String:appName]];
    [iosWindow makeKeyAndOrderFront:nil];
#endif

#ifndef TARGET_OS_IOS
    NSApplication* pApp = reinterpret_cast<NSApplication*>([inst.iosLaunchNotification object]);
    [pApp activateIgnoringOtherApps:true];
#endif

    /*Initialize uniforms*/
#ifndef TARGET_OS_IOS
    inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:sizeof(float) * 16 options:MTLResourceStorageModeManaged];
#else
    inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:sizeof(float) * 16 options:MTLResourceStorageModeShared];
#endif
    
    app->OnCreate();
}

- (void) applicationDidBecomeActive: (NSNotification*) pNotification{
    /*Application has resumed*/
    (*pen::State::Get()->mobileOnResumeCallback)();
}

- (void) applicationWillResignActive: (NSNotification*) pNotification{
    /*Application has paused*/
    (*pen::State::Get()->mobileOnPauseCallback)();
}

#ifndef TARGET_OS_IOS
-( BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) pSender
{
    /*Closes application after last window is closed*/
    return true;
}
#endif
@end
#endif
