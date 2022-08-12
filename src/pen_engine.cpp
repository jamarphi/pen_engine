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
#include "pen_engine.h"

namespace pen {
    std::vector<pen::Layer*> pen::ui::LM::layers = {};
    pen::Layer* pen::ui::LM::pixelLayer = nullptr;
    uint16_t pen::ui::LM::generalLayerId = 0;

#ifndef __PEN_MOBILE__
    void framebuffer_size_callback(glfwwindow* window, int width, int height);
    void click_callback(glfwwindow* window, int button, int action, int mods);
    void cursor_position_callback(glfwwindow* window, double xpos, double ypos);
    void key_callback(glfwwindow* window, int key, int scancode, int action, int mods);
#endif

    Pen* Pen::instance = 0;

    void Pen::CreateApplication(const char* appName, const int SCR_WIDTH, const int SCR_HEIGHT, const std::string& penEngineDir, const std::vector<std::string>& textureList, const bool debug) {
        /*Creates an instance of pen_engine*/
        Render* inst = pen::Render::Get();
        State* stateInst = pen::State::Get();
        stateInst->penEngineDir = penEngineDir;

#ifndef __PEN_MOBILE__
        glfwInit();
        glfwWindowHint(glfw_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(glfw_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(glfw_OPENGL_PROFILE, glfw_OPENGL_CORE_PROFILE);

        /*__APPLE__ is a glfw macro*/
#ifdef __APPLE__
#ifndef __PEN_IOS__
        glfwWindowHint(glfw_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#endif

        glfwwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, appName, NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create glfw window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, click_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetKeyCallback(window, key_callback);
#else
        void* window = nullptr;
#endif

#ifndef __PEN_MOBILE__
        if (!gladLoadGLLoader((gladloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize glad" << std::endl;
        }
#else
#ifdef __PEN_ANDROID__
        if (!gladLoadGLLoader((gladloadproc)eglGetProcAddress))
        {
            pen::android::AppLog("Failed to initialize glad");
        }
#endif
#endif

        /*Enables blending for textures*/
#ifndef __PEN_IOS__
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

        Get()->appWindow = (void*)window;
        stateInst->appWindow = (void*)window;
        Get()->SCREEN_WIDTH = SCR_WIDTH;
        Get()->SCREEN_HEIGHT = SCR_HEIGHT;
        inst->appOrthoCoord = { 0.0f,(float)SCR_WIDTH,0.0f,(float)SCR_HEIGHT,-1.0f,1.0f };
        stateInst->screenWidth = SCR_WIDTH;
        stateInst->screenHeight = SCR_HEIGHT;
        stateInst->actualScreenWidth = SCR_WIDTH;
        stateInst->actualScreenHeight = SCR_HEIGHT;
        stateInst->debug = debug;

        /*Get the allowed number of textures*/
#ifndef __PEN_IOS__
        int textureUnits = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
        stateInst->textureUnits = textureUnits / 6;
#else
        stateInst->textureUnits = 8;
#endif

        /*Initialize shaders*/
        pen::Shader shader(1);
        inst->appShader = shader;
        pen::Shader instancedShader(2);
        inst->instancedShader = instancedShader;

        /*Set initial scaling for text characters*/
        inst->textScaling = 25;
        stateInst->textScaling = 25;

        /*Set the projections and views for the application*/
        pen::Mat4x4 proj = pen::op::Ortho(inst->appOrthoCoord[0], inst->appOrthoCoord[1], inst->appOrthoCoord[2],
            inst->appOrthoCoord[3], inst->appOrthoCoord[4], inst->appOrthoCoord[5]);
        inst->appOrthoProj = proj;

        pen::Mat4x4 persProj = pen::op::Perspective(45, ((float)SCR_HEIGHT / (float)SCR_WIDTH), 0.1f, 100.0f);
        inst->appPerspectiveProj = persProj;

        inst->appOrthoView = pen::Mat4x4(1.0f, false);
        inst->appPerspectiveView = pen::Mat4x4(1.0f, true);

        pen::Camera camera(pen::Vec3(0.0f, 0.0f, 0.0f), SCR_WIDTH, SCR_HEIGHT);
        inst->camera = camera;

        /*
        Load text characters for rendering.

        This disables the byte alignment restriction so the text can use 1 bit for grayscale colors instead of the required
        4 bits.
        */
#ifndef __PEN_IOS__
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif
        pen::State::Get()->LoadCharacters();

        /*Initialize the state map of ascii characters for bitmap font rendering*/
        InitializeAsciiMap();


        /*Load preset textures*/
        Texture::LoadTexture(textureList.size() > 0 ? &textureList[0] : nullptr, textureList.size());

#ifndef __PEN_MOBILE__
        /*Set initial dimensions for points and lines*/
        glPointSize(3.0f);
        glLineWidth(3.0f);
#endif

        /*Sets time points for time elapsed*/
        Get()->timePoint1 = std::chrono::system_clock::now();

        /*Adds framerate info to the screen*/
        if (debug) {
            pen::ui::AddItem(new pen::ui::TextBox(DEBUG_ID, "Framerate: 0.0 fps", pen::Vec3(SCREEN_WIDTH / 9 * 5, Get()->SCREEN_HEIGHT / 10 * 9, -0.5f),
                Get()->SCREEN_WIDTH / 9 * 5, pen::PEN_TRANSPARENT, pen::PEN_WHITE, nullptr, nullptr, true));
        }
    }

    void Pen::End() {
        /*Terminate the application*/
#ifndef __PEN_MOBILE__
        glfwTerminate();
#endif
    }

    void Pen::ManageRender() {
        /*Manage swaps for each render*/
        for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
            pen::Render::RenderLayer(pen::ui::LM::layers[i]);
        }

#ifndef __PEN_MOBILE__
        glfwSwapBuffers(GetWindow());
        glfwPollEvents();
#endif
    }

#ifndef __PEN_MOBILE__
    glfwwindow* Pen::GetWindow() {
        /*Grabs the window pointer for the application*/
        return (glfwwindow*)Get()->appWindow;
    }
#endif

    bool Pen::Running() {
        /*Checks if the application is still running*/
        pen::Pen* inst = Get();

        /*Updates delta time*/
        inst->timePoint2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = inst->timePoint2 - inst->timePoint1;
        inst->deltaTime = elapsedTime.count();
        pen::State::Get()->deltaTime = inst->deltaTime;
        inst->timePoint1 = inst->timePoint2;

        /*Update framerate info if in debug mode*/
        if (pen::State::Get()->debug) {
            pen::ui::Item* item = pen::ui::FindItem(DEBUG_ID);
            std::string frameRate = "Framerate: " + std::to_string(1.0f / inst->deltaTime);
            frameRate += " fps";
            item->UpdateText(frameRate);
            pen::ui::Submit();
        }
        pen::Renderer::Clear();
#ifndef __PEN_MOBILE__
        return (!glfwWindowShouldClose(Get()->GetWindow())) ? true : false;
#else
        return pen::State::Get()->mobileActive;
#endif
    }

#ifndef __PEN_MOBILE__
    void Pen::CloseWindow() {
        glfwSetWindowShouldClose(GetWindow(), true);
    }
#endif

    void Pen::SetOrthoCoordinates(std::vector<float> orthoCoord) {
        /*Updates the orthographic projection for the application*/
        pen::Render::Get()->appOrthoCoord = orthoCoord;
    }

    float Pen::GetDeltaTime() {
        /*Returns the time taken per frame*/
        return Get()->deltaTime;
    }

    void Pen::Sleep(long ms) {
        /*Halts the main thread for the given time*/
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    unsigned int Pen::ScreenWidth() {
        /*Returns the screen width of the application*/
        return pen::State::Get()->screenWidth;
    }

    unsigned int Pen::ScreenHeight() {
        /*Returns the screen height of the application*/
        return pen::State::Get()->screenHeight;
    }

    void Pen::GetMousePos(double* x, double* y) {
        /*Returns the mouse position*/
        pen::State* inst = pen::State::Get();
#ifndef __PEN_MOBILE__
        glfwGetCursorPos(GetWindow(), x, y);

        /*Flip y position to start from the bottom*/
        *y = inst->actualScreenHeight - *y;

        /*Scale based on screen width and height*/
        *x = *x * inst->screenWidth / inst->actualScreenWidth;
        *y = *y * inst->screenHeight / inst->actualScreenHeight;
#else
        * x = inst->mobileMouseX;
        *y = inst->mobileMouseY;
#endif
    }

#ifndef __PEN_MOBILE__
    void Pen::MakeMouseHidden() {
        /*Hides the mouse*/
        glfwSetInputMode(GetWindow(), glfw_CURSOR, glfw_CURSOR_HIDDEN);
    }

    void Pen::MakeMouseShow() {
        /*Shows the mouse*/
        glfwSetInputMode(GetWindow(), glfw_CURSOR, glfw_CURSOR_NORMAL);
    }

    bool Pen::WindowActive() {
        /*Returns true if the mouse is in the content area of the application window*/
        return glfwGetWindowAttrib(GetWindow(), glfw_HOVERED);
    }

    int Pen::MouseState(int key) {
        return glfwGetMouseButton(GetWindow(), key);
    }

    int Pen::KeyState(int key) {
        return glfwGetKey(GetWindow(), key);
    }

    void Pen::HandleGUIClickEvents(bool choice, bool (*userClickCatchAll)()) {
        pen::State::Get()->handleGUIClickEvents = choice;
        Get()->clickCatchAll = userClickCatchAll;
    }

    void Pen::HandleGUIDragEvents(bool choice) {
        pen::State::Get()->handleGUIDragEvents = choice;
    }

    void Pen::HandleGUIKeyEvents(bool choice) {
        pen::State::Get()->handleGUIKeyEvents = choice;
    }
#endif

    void Pen::EnableDepthTesting(bool choice) {
#ifndef __PEN_IOS__
        if (choice) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
#endif
    }

    void Pen::HandleCameraInput(bool choice) {
        pen::State::Get()->handleCameraInput = choice;
    }

    void Pen::InitializeAsciiMap() {
        /*Initializes the map that is used for text character to unsigned char conversion*/
        pen::State* inst = pen::State::Get();
        inst->asciiMap.Insert(" ", 0);
        inst->asciiMap.Insert("!", 1);
        inst->asciiMap.Insert("\"", 2);
        inst->asciiMap.Insert("#", 3);
        inst->asciiMap.Insert("$", 4);
        inst->asciiMap.Insert("%", 5);
        inst->asciiMap.Insert("&", 6);
        inst->asciiMap.Insert("'", 7);
        inst->asciiMap.Insert("(", 8);
        inst->asciiMap.Insert(")", 9);
        inst->asciiMap.Insert("*", 10);
        inst->asciiMap.Insert("+", 11);
        inst->asciiMap.Insert(",", 12);
        inst->asciiMap.Insert("-", 13);
        inst->asciiMap.Insert(".", 14);
        inst->asciiMap.Insert("/", 15);
        inst->asciiMap.Insert("0", 16);
        inst->asciiMap.Insert("1", 17);
        inst->asciiMap.Insert("2", 18);
        inst->asciiMap.Insert("3", 19);
        inst->asciiMap.Insert("4", 20);
        inst->asciiMap.Insert("5", 21);
        inst->asciiMap.Insert("6", 22);
        inst->asciiMap.Insert("7", 23);
        inst->asciiMap.Insert("8", 24);
        inst->asciiMap.Insert("9", 25);
        inst->asciiMap.Insert(":", 26);
        inst->asciiMap.Insert(";", 27);
        inst->asciiMap.Insert("<", 28);
        inst->asciiMap.Insert("=", 29);
        inst->asciiMap.Insert(">", 30);
        inst->asciiMap.Insert("?", 31);
        inst->asciiMap.Insert("@", 32);
        inst->asciiMap.Insert("A", 33);
        inst->asciiMap.Insert("B", 34);
        inst->asciiMap.Insert("C", 35);
        inst->asciiMap.Insert("D", 36);
        inst->asciiMap.Insert("E", 37);
        inst->asciiMap.Insert("F", 38);
        inst->asciiMap.Insert("G", 39);
        inst->asciiMap.Insert("H", 40);
        inst->asciiMap.Insert("I", 41);
        inst->asciiMap.Insert("J", 42);
        inst->asciiMap.Insert("K", 43);
        inst->asciiMap.Insert("L", 44);
        inst->asciiMap.Insert("M", 45);
        inst->asciiMap.Insert("N", 46);
        inst->asciiMap.Insert("O", 47);
        inst->asciiMap.Insert("P", 48);
        inst->asciiMap.Insert("Q", 49);
        inst->asciiMap.Insert("R", 50);
        inst->asciiMap.Insert("S", 51);
        inst->asciiMap.Insert("T", 52);
        inst->asciiMap.Insert("U", 53);
        inst->asciiMap.Insert("V", 54);
        inst->asciiMap.Insert("W", 55);
        inst->asciiMap.Insert("X", 56);
        inst->asciiMap.Insert("Y", 57);
        inst->asciiMap.Insert("Z", 58);
        inst->asciiMap.Insert("[", 59);
        inst->asciiMap.Insert("\\", 60);
        inst->asciiMap.Insert("]", 61);
        inst->asciiMap.Insert("^", 62);
        inst->asciiMap.Insert("_", 63);
        inst->asciiMap.Insert("`", 64);
        inst->asciiMap.Insert("a", 65);
        inst->asciiMap.Insert("b", 66);
        inst->asciiMap.Insert("c", 67);
        inst->asciiMap.Insert("d", 68);
        inst->asciiMap.Insert("e", 69);
        inst->asciiMap.Insert("f", 70);
        inst->asciiMap.Insert("g", 71);
        inst->asciiMap.Insert("h", 72);
        inst->asciiMap.Insert("i", 73);
        inst->asciiMap.Insert("j", 74);
        inst->asciiMap.Insert("k", 75);
        inst->asciiMap.Insert("l", 76);
        inst->asciiMap.Insert("m", 77);
        inst->asciiMap.Insert("n", 78);
        inst->asciiMap.Insert("o", 79);
        inst->asciiMap.Insert("p", 80);
        inst->asciiMap.Insert("q", 81);
        inst->asciiMap.Insert("r", 82);
        inst->asciiMap.Insert("s", 83);
        inst->asciiMap.Insert("t", 84);
        inst->asciiMap.Insert("u", 85);
        inst->asciiMap.Insert("v", 86);
        inst->asciiMap.Insert("w", 87);
        inst->asciiMap.Insert("x", 88);
        inst->asciiMap.Insert("y", 89);
        inst->asciiMap.Insert("z", 90);
        inst->asciiMap.Insert("{", 91);
        inst->asciiMap.Insert("|", 92);
        inst->asciiMap.Insert("}", 93);
        inst->asciiMap.Insert("~", 94);
    }

#ifndef __PEN_MOBILE__
    void framebuffer_size_callback(glfwwindow* window, int width, int height)
    {
        /*Frame buffer callback function for OpenGL*/
        pen::State* inst = pen::State::Get();
        if (width < inst->screenWidth || height < inst->screenHeight) {
            width = inst->screenWidth;
            height = inst->screenHeight;
            glfwSetWindowSize(Pen::GetWindow(), width, height);
        }

        inst->actualScreenHeight = height;
        inst->actualScreenWidth = width;
        glViewport(0, 0, width, height);
    }
#endif

    bool Pen::HandleClick(pen::ui::Item* item, double* xPos, double* yPos, const int& button, const int& action) {
        bool eventHandled = false;
        int counter = item->childItems.size() - 1;
        for (int i = 0; i < item->childItems.size(); i++) {
            eventHandled = Pen::HandleClick(item->childItems[counter - i], xPos, yPos, button, action);

            if (eventHandled) return true;
        }

        if ((*xPos >= item->positions.x && *xPos <= item->positions.x + item->size.x) &&
            (*yPos >= item->positions.y && *yPos <= item->positions.y + item->size.y) &&
            item->isActive && item->forceActive && item->isClickable) {
            return item->OnClick(item, button, action);
        }
        else {
            return false;
        }
    }

#ifndef __PEN_MOBILE__
    void click_callback(glfwwindow* window, int button, int action, int mods)
    {
        pen::State* inst = pen::State::Get();
        if (inst->handleGUIClickEvents || inst->handleCameraInput) {
            if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) pen::State::Get()->keyableItem = nullptr;
            if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::RELEASED) pen::State::Get()->draggableItem = nullptr;
            double x = 0.0f, y = 0.0f;
            bool eventHandled = false;
            double* xPos = &x;
            double* yPos = &y;
            Pen::GetMousePos(xPos, yPos);

            bool cameraHandled = pen::Render::Get()->camera.HandleInput(pen::Pen::GetWindow());
            if (!cameraHandled) {
                int layerCounter = pen::ui::LM::layers.size() - 1;
                for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
                    if (pen::ui::LM::layers[layerCounter - i]->isFixed) {
                        /*Only loop through layers that have GUI components*/
                        int counter = pen::ui::LM::layers[layerCounter - i]->layerItems.size() - 1;
                        for (int j = 0; j < pen::ui::LM::layers[layerCounter - i]->layerItems.size(); j++) {
                            if (pen::ui::LM::layers[layerCounter - i]->layerItems[counter - j]->isUI) {
                                eventHandled = Pen::HandleClick(pen::ui::LM::layers[layerCounter - i]->layerItems[counter - j], xPos, yPos, button, action);
                                if (eventHandled) break;
                            }
                        }

                        if (!eventHandled && pen::Pen::Get()->clickCatchAll != nullptr) (*pen::Pen::Get()->clickCatchAll)();
                    }
                }
            }
        }
    }

    void cursor_position_callback(glfwwindow* window, double xPos, double yPos)
    {
        pen::State* inst = pen::State::Get();
        if ((inst->handleGUIDragEvents && inst->draggableItem != nullptr)
            || inst->handleCameraInput) {
            /*Flip y position to start from the bottom*/
            yPos = inst->actualScreenHeight - yPos;

            /*Scale based on screen width and height*/
            xPos = xPos * inst->screenWidth / inst->actualScreenWidth;
            yPos = yPos * inst->screenHeight / inst->actualScreenHeight;

            bool cameraHandled = pen::Render::Get()->camera.HandleInput(pen::Pen::GetWindow());
            if (!cameraHandled) {
                pen::ui::Item* item = (pen::ui::Item*)pen::State::Get()->draggableItem;
                item->OnDrag(item, &xPos, &yPos);
            }
        }
    }

    void key_callback(glfwwindow* window, int key, int scancode, int action, int mods)
    {
        pen::State* inst = pen::State::Get();
        if ((inst->handleGUIKeyEvents && inst->keyableItem != nullptr) || inst->handleCameraInput) {
            bool cameraHandled = pen::Render::Get()->camera.HandleInput(pen::Pen::GetWindow());
            if (!cameraHandled) {
                pen::ui::Item* item = (pen::ui::Item*)pen::State::Get()->keyableItem;
                item->OnKey(item, key, action);
            }
        }
    }
#endif

#ifdef __PEN_MOBILE__
    void Pen::mobile_click_callback(int button, int action, int mods)
    {
        /*Handles touch events for mobile devices*/
        double x = 0.0f, y = 0.0f;
        double* xPos = &x;
        double* yPos = &y;
        Pen::GetMousePos(xPos, yPos);
        bool eventHandled = false;

        pen::State* inst = pen::State::Get();
        if (inst->handleGUIClickEvents) {
            int layerCounter = pen::ui::LM::layers.size() - 1;
            for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
                if (pen::ui::LM::layers[layerCounter - i]->isFixed) {
                    /*Only loop through layers that have GUI components*/
                    int counter = pen::ui::LM::layers[layerCounter - i]->layerItems.size() - 1;
                    for (int j = 0; j < pen::ui::LM::layers[layerCounter - i]->layerItems.size(); j++) {
                        if (pen::ui::LM::layers[layerCounter - i]->layerItems[counter - j]->isUI) {
                            eventHandled = Pen::HandleClick(pen::ui::LM::layers[layerCounter - i]->layerItems[counter - j], xPos, yPos, button, action);
                            if (eventHandled) break;
                        }
                    }

                    if (!eventHandled && pen::Pen::Get()->clickCatchAll != nullptr) {
                        (*pen::Pen::Get()->clickCatchAll)();
                        eventHandled = true;
                    }
                }
            }
        }

        if (!eventHandled) {
            if (pen::State::Get()->mobileOnClickCallback != nullptr) (*pen::State::Get()->mobileOnClickCallback)(*xPos, *yPos);
        }
    }
#endif

    void Pen::SetMobileCallbacks(void (*onRenderCallback)(), void (*onClickCallback)(double, double), void (*onResumeCallback)(),
        void (*onPauseCallback)(), void (*onKeyCallback)(char), void (*onTiltCallback)(double, double, double, double),
        void (*onBluetoothCallback)()) {
        /*Sets the mobile callback functions for mobile devices*/
#ifdef __PEN_MOBILE__
        pen::State* inst = pen::State::Get();
        inst->mobileOnRenderCallback = onRenderCallback;
        inst->mobileOnClickCallback = onClickCallback;
        inst->mobileOnResumeCallback = onResumeCallback;
        inst->mobileOnPauseCallback = onPauseCallback;
        inst->mobileOnKeyCallback = onKeyCallback;
        inst->mobileOnTiltCallback = onTiltCallback;
        inst->mobileOnBluetoothCallback = onBluetoothCallback;

        /*Adds in the default assets*/
        pen::Asset::Add(pen::Asset("default"));
        pen::Asset::Add(pen::Asset("fonts/bitmap.png"));
        pen::Asset::assetMap.Find(1)->second.name = "fonts/bitmap.png";
        pen::Asset::Add(pen::Asset("pixel"));
#endif
    }

    void Pen::SetMobileTextures(const std::vector<std::string>& textureList) {
        /*Adds texture list for mobile to load in*/
#ifdef __PEN_MOBILE__
        pen::State::Get()->mobileTextureNameList = textureList;
        for (int i = 0; i < textureList.size(); i++) {
            pen::Asset::Add(pen::Asset(textureList[i]));
        }
#endif
    }
}
