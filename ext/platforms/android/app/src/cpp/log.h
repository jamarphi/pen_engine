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
#include "jni/jni.h"
#include "../../../../../../src/state/state.h"

#ifdef __PEN_MOBILE__

extern "C" {
    namespace pen {
        namespace platforms {
            namespace android {
                static void AppLog(const char* message) {
                    /*Sends message to Java for logging*/
                    JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                    jclass logClass = env->FindClass("com/jamar/pen_engine/MainActivity");
                    if (logClass != nullptr) {
                        jmethodID methodID = env->GetStaticMethodID(logClass, "appLog",
                            "(Ljava/lang/String;)V");
                        if (methodID != nullptr) {
                            jstring javaMessage = env->NewStringUTF(message);

                            env->CallStaticVoidMethod(logClass, methodID, javaMessage);

                            env->DeleteLocalRef(javaMessage);
                        }
                    }
                }
            }
        }
    }
}

#endif