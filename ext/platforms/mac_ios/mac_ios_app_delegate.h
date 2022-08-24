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

#if TARGET_OS_OSX
@interface PenMacIOSAppDelegate : NSObject<NSApplicationDelegate>
#else
@interface PenMacIOSAppDelegate : UIResponder<UIApplicationDelegate>
#endif

#if TARGET_OS_OSX
@property (nonatomic, strong) NSWindow* window;
#else
@property (nonatomic, strong) UIWindow* window;
#endif

/*These methods are inherited from NSObject or UIResponder*/
#if TARGET_OS_OSX
- (void) applicationWillFinishLaunching: (NSNotification*) pNotification;
- (void) applicationDidFinishLaunching: (NSNotification*) pNotification;
- (bool) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) pSender;
#else
- (BOOL)application:(UIApplication *)application
didFinishLaunchingWithOptions:(NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;
#endif
- (void) applicationDidBecomeActive: (NSNotification*) pNotification;
- (void) applicationWillResignActive: (NSNotification*) pNotification;
@end
#endif
