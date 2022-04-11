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
#include <android/asset_manager_jni.h>
#include "../../../../../../src/state/state.h"

#ifdef __PEN_MOBILE__

extern "C" {
    static void AndroidDrawPixel(int x, int y, float r, float g, float b, float a) {
        /*Draws pixel to android pixel buffer*/
        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
        jclass pixelClass = env->FindClass("com/jamar/penengine/PenSurfaceRenderer");
        if (pixelClass != nullptr) {
            jmethodID methodID = env->GetStaticMethodID(pixelClass, "drawPixels", "(IIFFFF)V");
            if (methodID != nullptr) {
                env->CallStaticVoidMethod(pixelClass, methodID, x, y, r, g, b, a);
            }
        }
    }

    static void AndroidPixelFlush() {
        /*Clear the pixel buffer*/
        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
        jclass pixelClass = env->FindClass("com/jamar/penengine/PenSurfaceRenderer");
        if (pixelClass != nullptr) {
            jmethodID methodID = env->GetStaticMethodID(pixelClass, "flushPixels", "()V");
            if (methodID != nullptr) {
                env->CallStaticVoidMethod(pixelClass, methodID);
            }
        }
    }

    static unsigned char* AndroidLoadSprite(const char* name, int& texWidth, int& texHeight) {
        /*Clear the pixel buffer*/
        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
        jclass pixelClass = env->FindClass("com/jamar/penengine/PenSurfaceRenderer");
        if (pixelClass != nullptr) {
            jmethodID methodID = env->GetStaticMethodID(pixelClass, "loadSprite",
                "(Ljava/lang/String;)[B");
            if (methodID != nullptr) {
                jstring javaName = env->NewStringUTF(name);
                jbyteArray image = (jbyteArray)env->CallStaticObjectMethod(pixelClass, methodID, javaName);
                jsize imageArraySize = env->GetArrayLength(image);

                unsigned char* nativeImage = new unsigned char[imageArraySize];
                jbyte* imageItems = env->GetByteArrayElements(image, 0);
                for(int i = 0; i < imageArraySize; i++){
                    nativeImage[i] = (unsigned char)imageItems[i];
                    if(imageItems[i] != 0){
                        nativeImage[i];
                    }
                }

                /*Now update the width and height*/
                jmethodID widthMethodID = env->GetStaticMethodID(pixelClass, "getSpriteWidth","()I");
                if(widthMethodID != nullptr){
                    jint javaWidth = env->CallStaticIntMethod(pixelClass, widthMethodID);
                    texWidth = (int) javaWidth;

                    jmethodID heightMethodID = env->GetStaticMethodID(pixelClass, "getSpriteHeight","()I");
                    if(heightMethodID != nullptr){
                        /*The sprite gets removed on the java end*/
                        jint javaHeight = env->CallStaticIntMethod(pixelClass, heightMethodID);
                        texHeight = (int) javaHeight;

                        env->DeleteLocalRef(javaName);
                        env->ReleaseByteArrayElements(image, imageItems, JNI_ABORT);
                        return nativeImage;
                    }
                }
            }
        }
    }
}

#endif