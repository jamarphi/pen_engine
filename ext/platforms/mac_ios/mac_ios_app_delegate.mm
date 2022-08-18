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

#ifndef TARGET_OS_IOS
- (void) applicationWillFinishLaunching: (NSNotification*) pNotification
{
    /*Before application is done launching*/
    NSApplication* pApp = reinterpret_cast<NSApplication*>([pNotification object]);
    pApp->setActivationPolicy(NSActivationPolicy::ActivationPolicyRegular);
}

- (void) applicationDidFinishLaunching: (NSNotification*) pNotification
{
    /*After application is done launching*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    inst.iosLaunchNotification = pNotification;
}

-( BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) pSender
{
    /*Closes application after last window is closed*/
    return YES;
}
#else
- (BOOL)application:(UIApplication *)application
didFinishLaunchingWithOptions:(NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    /*After application is done launching*/
    return YES;
}
#endif

- (void) applicationDidBecomeActive: (NSNotification*) pNotification{
    /*Application has resumed*/
    if(pen::State::Get()->mobileOnResumeCallback != nullptr){
        (*pen::State::Get()->mobileOnResumeCallback)();
    }
}

- (void) applicationWillResignActive: (NSNotification*) pNotification{
    /*Application has paused*/
    if(pen::State::Get()->mobileOnPauseCallback != nullptr){
        (*pen::State::Get()->mobileOnPauseCallback)();
    }
}
@end
#endif
