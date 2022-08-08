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

#include "ios_manager.h"

#ifdef __PEN_IOS__

#if TARGET_OS_SIMULATOR && (!defined(__IPHONE_13_0) ||  !defined(__TVOS_13_0))
#error No simulator support for Metal API for this SDK version.  Must build for a device
#endif

int main(int argc, char* argv[])
{
    NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    PenIOSAppDelegate del;

    NS::Application* pSharedApplication = NS::Application::sharedApplication();
    pSharedApplication->setDelegate(&del);
    pSharedApplication->run();

    pAutoreleasePool->release();

    return 0;
}

#endif
