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
#include "ios_app_delegate.h"

#ifdef __PEN_IOS__

@implementation PenIOSAppDelegate

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
    IOSState* inst = [IOSState Get];
    MTLDepthStencilDescriptor* pDsDesc = [[MTLDepthStencilDescriptor alloc] init];
    [pDsDesc setDepthCompareFunction:MTLCompareFunctionLess];
    [pDsDesc setDepthWriteEnabled:true];
    inst.iosDepthStencilState = [inst.iosDevice newDepthStencilStateWithDescriptor:pDsDesc];
    inst.iosLaunchNotification = pNotification;

    /*Currently at max three different buffer types sent to metal shaders*/
    inst.dispatchSemaphore = dispatch_semaphore_create(3);
    
    /*Initialize static arrays*/
    NSMutableArray* textures = [IOSState GetTextures];
    NSMutableDictionary* vertexBuffers = [IOSVertexBuffer IOSVertexBuffersGet];
    NSMutableDictionary* argumentBuffers = [IOSArgumentBuffer IOSArgumentBuffersGet];
    NSMutableDictionary* indexBuffers = [IOSIndexBuffer IOSIndexBuffersGet];
    textures = [[NSMutableArray alloc] init];
    vertexBuffers = [NSMutableDictionary dictionary];
    argumentBuffers = [NSMutableDictionary dictionary];
    indexBuffers = [NSMutableDictionary dictionary];

    App* app = new App();
    pen::State::Get()->mobileActive = true;
    app->CreateApplication("App", 110, 540, "");
    app->OnCreate();
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
