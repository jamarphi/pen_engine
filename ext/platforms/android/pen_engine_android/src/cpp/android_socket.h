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

#ifdef __PEN_MOBILE__
#include <iostream>
#include <android/asset_manager_jni.h>
#endif

extern "C" {
    namespace pen {
        namespace android {
            namespace conn {
                namespace socket {
                    static void Connect(const char* route, int port) {
                        /*Connects to a server hosting a socket*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass socketClass = env->FindClass("com/jamar/penengine/PenSocket");
                        if (socketClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(socketClass, "connect", "(Ljava/lang/String;I)V");
                            if (methodID != nullptr) {
                                jstring javaRoute = env->NewStringUTF(route);
                                env->CallStaticVoidMethod(socketClass, methodID, javaRoute, port);
                                env->DeleteLocalRef(javaRoute);
                            }
                        }
#endif
                    }

                    static std::string Send(const char* message) {
                        /*Sends a message to the server hosting the socket connection*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass socketClass = env->FindClass("com/jamar/penengine/PenSocket");
                        if (socketClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(socketClass, "send", "(Ljava/lang/String;)Ljava/lang/String;");
                            if (methodID != nullptr) {
                                jstring javaMessage = env->NewStringUTF(message);
                                jvalue args[1];
                                args[0].l = javaMessage;
                                jstring javaResponse = (jstring)env->CallStaticObjectMethodA(socketClass, methodID, args);
                                const char* responseStr = env->GetStringUTFChars(javaResponse, 0);
                                std::string response(responseStr);
                                env->DeleteLocalRef(javaMessage);
                                env->DeleteLocalRef(javaResponse);
                                env->DeleteLocalRef((jobject)args);
                                delete[] responseStr;
                                return response;
                            }
                        }
#endif
                        return "";
                    }

                    static void Close() {
                        /*Closes the connection to the server hosting the socket connection*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass socketClass = env->FindClass("com/jamar/penengine/PenSocket");
                        if (socketClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(socketClass, "close", "()V");
                            if (methodID != nullptr) {
                                env->CallStaticVoidMethod(socketClass, methodID);
                            }
                        }
#endif
                    }
                }
            }
        }
    }
}