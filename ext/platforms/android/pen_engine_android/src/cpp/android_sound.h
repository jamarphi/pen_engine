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
#include "../../../../../../src/state/state.h"

#ifdef __PEN_ANDROID__
#include <iostream>
#include <android/asset_manager_jni.h>
#endif

extern "C" {
    namespace pen {
        namespace android {
            namespace sound {
                static void Play(std::string file, bool isMusic = false) {
                    /*Connects to a server hosting a socket*/
#ifdef __PEN_ANDROID__
                    if (file != "") {
                        if (file.find(".") != std::string::npos) file = file.substr(0, file.find("."));
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass mainClass = env->FindClass("com/jamar/penengine/MainActivity");
                        if (mainClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(mainClass, "playSound", "(Ljava/lang/String;I)V");
                            if (methodID != nullptr) {
                                jstring javaFile = env->NewStringUTF(file.c_str());
                                env->CallStaticVoidMethod(mainClass, methodID, javaFile, isMusic ? 1 : 0);
                                env->DeleteLocalRef(javaFile);
                            }
                        }
                    }
#endif
                }
            }
        }

    }
}
