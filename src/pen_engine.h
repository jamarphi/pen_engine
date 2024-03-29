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
#ifndef __PEN_MAC_IOS__
#include "../dependencies/glad/glad.h"
#endif
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
#include "state/pixel_3d.h"
#include "state/layer_manager.h"
#include "state/keys.h"
#include "ui/animation/animation_ui.h"
#include "ui/animation/animation_3d.h"
#include "ui/animation/animation_pixel_2d.h"
#include "ui/animation/animation_pixel_3d.h"
#include "ops/matrices/mat4x4.h"
#include "ops/matrices/mat2x4.h"
#include "ops/matrices/mat3x3.h"
#include "ops/matrices/mat.h"
#include "ops/operations/operations.h"
#include "ops/operations/trig.h"
#include "ops/operations/3d.h"
#include "renderer/camera.h"
#include "objects/containers/map.h"
#include "objects/containers/json.h"
#include "../ext/platforms/android/pen_engine_android/src/cpp/android_log.h"
#include "../ext/platforms/android/pen_engine_android/src/cpp/android_database.h"
#include "../ext/platforms/android/pen_engine_android/src/cpp/android_bluetooth.h"
#include "../ext/platforms/android/pen_engine_android/src/cpp/android_http.h"
#include "../ext/platforms/android/pen_engine_android/src/cpp/android_socket.h"
#ifdef __PEN_MAC_IOS__
#include <TargetConditionals.h>
#endif
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
#include "ui/key_board.h"

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

            while (pen::Pen::Running())
            {
                pen::Pen::OnInput();
                if(pen::Render::Get()->firstTime) pen::Render::Background();

                ***Rendering logic goes here***
                
                pen::Pen::ManageRender();
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

        static void ManageRender();

#ifndef __PEN_MOBILE__
        static glfwwindow* GetWindow();
#endif

        static bool Running();
        static void CloseWindow();
        void SetOrthoCoordinates(std::vector<float> orthoCoord);

        static float GetDeltaTime();

        static void Sleep(long ms);

        static unsigned int ScreenWidth();

        static unsigned int ScreenHeight();

        /*----Mouse functions----*/
#ifndef __PEN_MOBILE__
        static void GetMousePos(double* x, double* y);
#else
        static std::vector<pen::Tap*>* GetMousePos();
#endif
        static void MakeMouseHidden();
        static void MakeMouseShow();
        static bool WindowActive();
        static int MouseState(int key);
        /*----Mouse functions----*/

        static int KeyState(int key);

        static void HandleGUIClickEvents(bool choice, bool (*userClickCatchAll)());
        static void HandleGUIDragEvents(bool choice);
        static void HandleGUIKeyEvents(bool choice);
        static bool HandleClick(pen::ui::Item* item, const int& xPos, const int& yPos, const int& button, const int& action);
        static void EnableDepthTesting(bool choice);
        static void HandleCameraInput(bool choice, float speed = 1.0f, void (*onCameraEvent)() = nullptr);
        static void RunAnimations();
        static void SetPBEventListeners(bool (*onClickCallback)(pen::ui::Item*, int, int), bool (*onDragCallback)(pen::ui::Item*, double*, double*), bool (*onKeyCallback)(pen::ui::Item*, int, int));

#ifdef __PEN_MOBILE__
        static void mobile_click_callback(int button, int action, int mods);
#endif
        static void SetMobileCallbacks(void (*onRenderCallback)(), void (*onClickCallback)(double, double), void (*onResumeCallback)(),
            void (*onPauseCallback)(), void (*onKeyCallback)(char), void (*onTiltCallback)(double, double, double), 
            void (*onAndroidBluetoothCallback)(), void (*onMacIosBluetoothCallback)(char*, long, unsigned int),
            void (*onHttpCallback)(pen::Map<std::string,std::string>), void (*onSocketCallback)(char*, unsigned int));

        static void SetMobileTextures(const std::vector<std::string>& textureList);
        static void SetInstancedOffsets(std::vector<pen::Vec3*>* offsets);

    private:
        void InitializeAsciiMap();
    };

    namespace ui {
        static void Pan(float panX, float panY, bool reset = false);
        static void Zoom(float mag, bool reset = false);
    }
}
