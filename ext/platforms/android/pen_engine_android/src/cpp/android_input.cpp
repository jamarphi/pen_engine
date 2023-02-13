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
#include "../../../../../../src/pen_engine.h"

#ifdef __PEN_ANDROID__
#include <android/asset_manager_jni.h>
#define ANDROID_MOBILE_ACCELERATION_CONSTANT 9.80665f

extern "C" {
    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesBegin(JNIEnv* env, jclass obj, jint id, jfloat x, jfloat y) {
        /*A touch has started*/
        pen::State* inst = pen::State::Get();

        /*Flip y position to start from the bottom*/
        float xPos;
        float yPos = inst->actualScreenHeight - (float)y;

        /*Scale based on screen width and height and convert to pixel buffer coordinates*/
        xPos = (float)x * inst->screenWidth / inst->actualScreenWidth;
        yPos = yPos * inst->screenHeight / inst->actualScreenHeight;
        xPos = xPos * pen::PixelBufferWidth() / inst->actualScreenWidth;
        yPos = yPos * pen::PixelBufferHeight() / inst->actualScreenHeight;
        pen::State::Get()->mobileMouse->push_back(new pen::Tap{ id, (int)xPos, (int)yPos });
        pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::PRESSED, 0);
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesEnd(JNIEnv* env, jclass obj, jint id, jfloat x, jfloat y) {
        /*A touch has ended*/
        pen::State* inst = pen::State::Get();
        std::vector<pen::Tap*>* tempTaps = new std::vector<pen::Tap*>();
        for (int i = 0; i < inst->mobileMouse->size(); i++) {
            if (inst->mobileMouse->at(i)->id != id) {
                tempTaps->push_back(inst->mobileMouse->at(i));
            }
            else {
                /*Updates the mobileMouse vector with the released point for handling in mobile_click_callback before removing it*/

                /*Flip y position to start from the bottom*/
                float xPos;
                float yPos = inst->actualScreenHeight - (float)y;

                /*Scale based on screen width and height and convert to pixel buffer coordinates*/
                xPos = (float)x * inst->screenWidth / inst->actualScreenWidth;
                yPos = yPos * inst->screenHeight / inst->actualScreenHeight;
                xPos = xPos * pen::PixelBufferWidth() / inst->actualScreenWidth;
                yPos = yPos * pen::PixelBufferHeight() / inst->actualScreenHeight;

                inst->mobileMouse->at(i)->x = (int)xPos;
                inst->mobileMouse->at(i)->y = (int)yPos;
            }
        }
        pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::RELEASED, 0);
        delete inst->mobileMouse;
        inst->mobileMouse = tempTaps;
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesMove(JNIEnv* env, jclass obj, jintArray ids, jfloatArray xs, jfloatArray ys) {
        /*A touch is moving*/
        pen::State* inst = pen::State::Get();
        const int size = env->GetArrayLength(ids);
        jint* id = new jint[size];
        jfloat* x = new jfloat[size];
        jfloat* y = new jfloat[size];

        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < inst->mobileMouse->size(); j++) {
                if (inst->mobileMouse->at(j)->id == id[i]) {
                    /*Flip y position to start from the bottom*/
                    float xPos;
                    float yPos = inst->actualScreenHeight - (float)y[i];

                    /*Scale based on screen width and height and convert to pixel buffer coordinates*/
                    xPos = (float)x[i] * inst->screenWidth / inst->actualScreenWidth;
                    yPos = yPos * inst->screenHeight / inst->actualScreenHeight;
                    xPos = xPos * pen::PixelBufferWidth() / inst->actualScreenWidth;
                    yPos = yPos * pen::PixelBufferHeight() / inst->actualScreenHeight;

                    inst->mobileMouse->at(j)->x = (int)xPos;
                    inst->mobileMouse->at(j)->y = (int)yPos;
                    break;
                }
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenAccelerometer_onSensorChanged(JNIEnv* env, jclass obj, jfloat x, jfloat y, jfloat z, jlong timeStamp) {
        /*Handle tilt events*/
        double acelX = -((double)x / ANDROID_MOBILE_ACCELERATION_CONSTANT);
        double acelY = -((double)y / ANDROID_MOBILE_ACCELERATION_CONSTANT);
        double acelZ = -((double)z / ANDROID_MOBILE_ACCELERATION_CONSTANT);

        if (pen::State::Get()->mobileOnTiltCallback != nullptr) (*pen::State::Get()->mobileOnTiltCallback)(acelX, acelY, acelZ);
    }
}
#endif
