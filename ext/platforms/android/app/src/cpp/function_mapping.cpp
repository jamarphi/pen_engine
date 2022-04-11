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
#include "jni/jni.h"
#include "../../../../../../src/pen_engine.h"

#ifdef __PEN_MOBILE__
#ifdef __PEN_LINUX__

/*
  This file is for the instantiation of Pen Engine by the user in order to include it's OnCreate function.
  In the OnCreate function it is expected that pen::Pen::SetMobileCallbacks(); is called.
*/
#include "../../../../../../../app.h"

#ifdef __PEN_CMAKE__
#include <abs_asset_dir_config.h>
#endif

extern "C" {

#if __ANDROID_API__ > 19
#include <signal.h>
#include <dlfcn.h>
    typedef __sighandler_t(*bsd_signal_func_t)(int, __sighandler_t);
    bsd_signal_func_t bsd_signal_func = NULL;

    __sighandler_t bsd_signal(int s, __sighandler_t f) {
        if (bsd_signal_func == NULL) {
            // For now (up to Android 7.0) this is always available 
            bsd_signal_func = (bsd_signal_func_t)dlsym(RTLD_DEFAULT, "bsd_signal");

            if (bsd_signal_func == NULL) {
                pen::platforms::android::AppLog("bad bsd signal...");
            }
        }
        return bsd_signal_func(s, f);
    }
#endif // __ANDROID_API__ > 19

#include <android/asset_manager.h>

    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        return JNI_VERSION_1_4;
    }

    JNIEXPORT jintArray Java_com_jamar_penengine_MainActivity_getGLContextAttrs(JNIEnv* env, jclass obj)
    {
        jint attributes[7] = { 8, 8, 8, 8, 16, 0, 0 };
        jintArray glContextAttrsJava = env->NewIntArray(7);
        env->SetIntArrayRegion(glContextAttrsJava, 0, 7, attributes);
        return glContextAttrsJava;
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeInit(JNIEnv* env, jclass obj, jint w, jint h) {
        /*Ran when the app first loads*/
        if (pen::State::Get()->screenWidth == 0) {
            /*Initialize app*/
            pen::State::Get()->javaEnv = (void*)env;
            pen::platforms::android::AppLog("native initialization starting...");
            App* app = new App();
            std::string rootDir = ROOT_DIR;

            /*"pen_engine/" is removed at the end since it will be added on later*/
            app->CreateApplication("App", w, h, rootDir.substr(0, rootDir.length() - 11));

            pen::State::Get()->mobileActive = true;
            glViewport(0, 0, w, h);

            app->OnCreate();
            pen::platforms::android::AppLog("native initialization finished...");
        }
        else {
            /*Update window size*/
            pen::State::Get()->mobileActive = true;
            pen::State::Get()->actualScreenHeight = h;
            pen::State::Get()->actualScreenWidth = w;
        }
    }

    JNIEXPORT void Java_com_jamar_penengine_PenSurfaceRenderer_nativeOnSurfaceChanged(JNIEnv* env, jclass obj, jint w, jint h)
    {
        /*If the screen size has changed*/
        pen::State::Get()->actualScreenHeight = h;
        pen::State::Get()->actualScreenWidth = w;
        pen::State::Get()->mobileActive = true;
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeRender(JNIEnv* env, jclass obj) {
        /*Happens each frame*/
        if (pen::State::Get()->handleMobileRender != nullptr) (*pen::State::Get()->handleMobileRender)();
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeOnPause(JNIEnv* env, jclass obj) {
        /*Pause the application*/
        if (pen::State::Get()->mobileOnPauseCallback != nullptr) {
            (*pen::State::Get()->mobileOnPauseCallback)();
        }
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeOnResume(JNIEnv* env, jclass obj) {
        /*Resume the application*/
        if (pen::State::Get()->mobileOnResumeCallback != nullptr) {
            (*pen::State::Get()->mobileOnResumeCallback)();
        }
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_loadAssetManager(JNIEnv* env, jclass obj, jobject assetManager) {
        /*Used to initialize the android asset manager*/

        /*TODO: AAssetManager_fromJava is an undefined reference*/
        //AAssetManager* manager = AAssetManager_fromJava(env, assetManager);
        //pen::State::Get()->androidAssetManager = (void*)manager;
    }

    JNIEXPORT jstring JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeImageName(JNIEnv* env, jclass obj, jint id) {
        /*Returns the texture name to java*/
        int texNamePos = (int)id;

        /*Subtract 3 to remove default textures from position count*/
        texNamePos -= 3;

        std::string texName = (texNamePos < pen::State::Get()->mobileTextureNameList.size()) ? pen::State::Get()->mobileTextureNameList[texNamePos] : "";

        texName = texName.substr(0, texName.find("."));
        jstring javaTexName = env->NewStringUTF(texName.c_str());
        return javaTexName;
    }

    JNIEXPORT jboolean JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_isPixelDrawn(JNIEnv* env, jclass obj) {
        /*Returns true if pixel has been drawn to pixel buffer*/
        jboolean pixelDrawn = (jboolean)pen::State::Get()->pixelDrawn;

        if (pen::State::Get()->pixelDrawn) pen::State::Get()->pixelDrawn = false;

        return pixelDrawn;
    }
}
#endif
#endif
