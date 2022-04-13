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
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include "state/config.h"
#include "../dependencies/glad/glad.h"
#ifndef __PEN_MOBILE__
#include "../dependencies/glfw/include/glfw3.h"
#endif
#include <chrono>
#include <thread>
#include "renderer/renderer.h"
#include "core/vertex_buffer.h"
#include "core/vertex_buffer_schema.h"
#include "core/index_buffer.h"
#include "core/vertex_array.h"
#include "core/shader.h"
#include "core/texture.h"
#include "ui/position.h"
#include "core/color.h"
#include "renderer/render.h"
#include "state/state.h"
#include "state/pixel.h"
#include "state/layer_manager.h"
#include "state/keys.h"
#include "ui/animation.h"
#include "state/animate.h"
#include "ops/matrices/mat4x4.h"
#include "ops/matrices/mat2x4.h"
#include "ops/matrices/mat3x3.h"
#include "ops/matrices/mat.h"
#include "ops/operations/operations.h"
#include "ops/operations/trig.h"
#include "ops/operations/3d.h"
#include "renderer/camera.h"
#include "objects/containers/map.h"
#include "../ext/platforms/android/app/src/cpp/log.h"
#include "../ext/platforms/android/app/src/cpp/android_bluetooth.h"
#include "../ext/ai/agent.h"
#include "../ext/ai/free_agent.h"
#include "../ext/misc/tile_map/tile_map.h"

/*UI objects*/
#include "ui/ui_object.h"
#include "ui/text_box.h"
#include "ui/button.h"
#include "ui/nav_bar.h"
#include "ui/vertical_list.h"
#include "ui/scroll_bar.h"
#include "ui/slider.h"
#include "ui/text_editor.h"
#include "ui/context_window.h"
#include "ui/file_modal.h"
#include "ui/div.h"

#define SHADER_SOURCE ""
#define TEXTURE_SOURCE "res/textures/"

namespace pen {
    class Pen {
        /*
        Inherit the methods of the Pen class:

        class Demo : public pen::Pen {
            void OnCreate() override {

            }

            void OnInput() override {

            }

            void OnRender() override {

            }
        };
        */
        static Pen* instance;
    public:
        void* appWindow;
        std::chrono::system_clock::time_point timePoint1;
        std::chrono::system_clock::time_point timePoint2;
        float deltaTime;

        /*User input*/
        bool (*clickCatchAll)();

        Pen() {

        }
    private:
        int SCREEN_WIDTH;
        int SCREEN_HEIGHT;
    public:
        static Pen* Get() {
            if (!instance)
                instance = new Pen();
            return instance;
        }

        void CreateApplication(const char* appName, const int SCR_WIDTH, const int SCR_HEIGHT, const std::string& penEngineDir, const std::vector<std::string>& textureList = {}, const bool debug = false);

        virtual void OnRender() {
            /*
            Draw elements to the screen in this function.

            Inherit the OnRender function like this:

            class Demo : pen::Pen {
                void OnRender() {

                }
            };
            ----------------------------------------------------------

            Create while loop when using this function like this:

            pen::Renderer renderer;
            while (pen::Pen::Running())
            {
                pen::Pen::OnInput();
                renderer.Clear();
                if(pen::Render::Get()->firstTime) renderer.Background();

                ***Rendering logic goes here***
                
                pen::Pen::ManageRender(&renderer);
            }
            */
        }

        static void End();

        virtual void OnInput() {
            /*Override this function to add user input for keys.

              An example of this is:
              if(pen::Pen::KeyState(pen::in::KEYS::ESCAPE) == pen::in::KEYS::PRESSED) CloseWindow();
            */
        }

        virtual void OnCreate() {
            /*Override this function before rendering your application*/
        }

        static void ManageRender(pen::Renderer* renderer);

#ifndef __PEN_MOBILE__
        static glfwwindow* GetWindow();
#endif

        static bool Running();

#ifndef __PEN_MOBILE__
        static void CloseWindow();
#endif

        void SetOrthoCoordinates(std::vector<float> orthoCoord);

        static float GetDeltaTime();

        static void Sleep(long ms);

        static unsigned int ScreenWidth();

        static unsigned int ScreenHeight();

        /*----Mouse functions----*/
        static void GetMousePos(double* x, double* y);

#ifndef __PEN_MOBILE__
        static void MakeMouseHidden();
#endif

#ifndef __PEN_MOBILE__
        static void MakeMouseShow();
#endif

#ifndef __PEN_MOBILE__
        static bool WindowActive();
#endif

#ifndef __PEN_MOBILE__
        static int MouseState(int key);
#endif
        /*----Mouse functions----*/

#ifndef __PEN_MOBILE__
        static int KeyState(int key);
#endif

#ifndef __PEN_MOBILE__
        static void HandleGUIClickEvents(bool choice, bool (*userClickCatchAll)());
#endif

#ifndef __PEN_MOBILE__
        static void HandleGUIDragEvents(bool choice);
#endif

#ifndef __PEN_MOBILE__
        static void HandleGUIKeyEvents(bool choice);
#endif

#ifndef __PEN_MOBILE__
        static bool HandleClick(pen::ui::Item* item, double* xPos, double* yPos, const int& button, const int& action);
#endif

        static void EnableDepthTesting(bool choice);

        static void HandleCameraInput(bool choice);

#ifdef __PEN_MOBILE__
        static void mobile_click_callback(int button, int action, int mods);

        static void SetMobileCallbacks(void (*onRenderCallback)(), void (*onClickCallback)(double, double), void (*onResumeCallback)(),
            void (*onPauseCallback)(), void (*onKeyCallback)(char), void (*onTiltCallback)(double, double, double, double), 
            void (*onBluetoothCallback)());

        static void SetMobileTextures(const std::vector<std::string>& textureList);
#endif

    private:
        void InitializeAsciiMap();
    };
}
