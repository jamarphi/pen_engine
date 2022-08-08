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

NS::Menu* PenIOSAppDelegate::CreateMenuBar()
{
    /*Creates menu bar for application*/
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu* pMainMenu = NS::Menu::alloc()->init();
    NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* pAppMenu = NS::Menu::alloc()->init(NS::String::string("App", UTF8StringEncoding));

    NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quitItemName = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(appName);
    SEL quitCb = NS::MenuItem::registerActionCallback("appQuit", [](void*, SEL, const NS::Object* pSender) {
        auto pApp = NS::Application::sharedApplication();
        pApp->terminate(pSender);
        });

    NS::MenuItem* pAppQuitItem = pAppMenu->addItem(quitItemName, quitCb, NS::String::string("q", UTF8StringEncoding));
    pAppQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    pAppMenuItem->setSubmenu(pAppMenu);

    NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* pWindowMenu = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));

    SEL closeWindowCb = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*) {
        auto pApp = NS::Application::sharedApplication();
        pApp->windows()->object< NS::Window >(0)->close();
        });
    NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem(NS::String::string("Close Window", UTF8StringEncoding), closeWindowCb, NS::String::string("w", UTF8StringEncoding));
    pCloseWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    pWindowMenuItem->setSubmenu(pWindowMenu);

    pMainMenu->addItem(pAppMenuItem);
    pMainMenu->addItem(pWindowMenuItem);

    pAppMenuItem->release();
    pWindowMenuItem->release();
    pAppMenu->release();
    pWindowMenu->release();

    return pMainMenu->autorelease();
}

void PenIOSAppDelegate::ApplicationWillFinishLaunching(NS::Notification* pNotification)
{
    /*Before application is done launching*/
    NS::Menu* pMenu = createMenuBar();
    NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
    pApp->setMainMenu(pMenu);
    pApp->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void PenIOSAppDelegate::ApplicationDidFinishLaunching(NS::Notification* pNotification)
{
    /*After application is done launching*/
    MTL::DepthStencilDescriptor* pDsDesc = MTL::DepthStencilDescriptor::alloc()->init();
    pDsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    pDsDesc->setDepthWriteEnabled(true);
    IOSState::Get()->iosDepthStencilState = _pDevice->newDepthStencilState(pDsDesc);
    pDsDesc->release();
    IOSState::Get()->iosLaunchNotification = pNotification;

    /*Currently at max three different buffer types sent to metal shaders*/
    IOSState::Get()->dispatch_semaphore_t = dispatch_semaphore_create(3);

    App* app = new App();
    pen::State::Get()->mobileActive = true;
    app->CreateApplication("App", 110, 540, "");
    app->OnCreate();
}

bool PenIOSAppDelegate::ApplicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender)
{
    /*Closes application after last window is closed*/
    return true;
}
#endif
