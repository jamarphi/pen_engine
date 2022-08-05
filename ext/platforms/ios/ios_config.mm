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
void IOSConfig::Init(unsigned int width, unsigned int height, const char* appName){
	/*Handles app creation for ios*/
    IOSState* inst = IOSState::Get();
	inst->iosCommandQueue = (void*)inst->iosDevice->newCommandQueue();

    CGRect frame = (CGRect){ {(float)width, (float)width}, {(float)height, (float)height} };

    NS::Window* iosWindow = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false);

    inst->iosDevice = MTL::CreateSystemDefaultDevice();

    inst->iosMtkView = MTK::View::alloc()->init(frame, inst->iosDevice);
    inst->iosMtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    iosViewDelegate = new PenMTKViewDelegate(inst->iosDevice);
    inst->iosMtkView->setDelegate(iosViewDelegate);

    iosWindow->setContentView(inst->iosMtkView);
    iosWindow->setTitle(NS::String::string(appName, NS::StringEncoding::UTF8StringEncoding));

    iosWindow->makeKeyAndOrderFront(nullptr);

    NS::Application* pApp = reinterpret_cast<NS::Application*>(inst->iosLaunchNotification->object());
    pApp->activateIgnoringOtherApps(true);

    /*Initialize uniforms*/
    inst->iosMVPBuffer = inst->iosDevice->newBuffer(sizeof(float) * 16, MTL::ResourceStorageModeManaged);
}
#endif