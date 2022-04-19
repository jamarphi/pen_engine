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
                namespace bt {
                    static void Connect(const char* deviceName = "") {
                        /*Searches for a device to connect with*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass mainClass = env->FindClass("com/jamar/penengine/MainActivity");
                        if (mainClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(mainClass, "bluetoothSearch", "(Ljava/lang/String;)V");
                            if (methodID != nullptr) {
                                jstring javaDeviceName = env->NewStringUTF(deviceName);
                                env->CallStaticVoidMethod(mainClass, methodID, javaDeviceName);
                                env->DeleteLocalRef(javaDeviceName);
                            }
                        }
#endif
                    }

                    static char* Read(int* numBytes) {
                        /*Reads from a connected device*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass mainClass = env->FindClass("com/jamar/penengine/MainActivity");
                        if (mainClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(mainClass, "bluetoothRead", "()[B");
                            if (methodID != nullptr) {
                                jbyteArray data = (jbyteArray)env->CallStaticObjectMethod(mainClass, methodID);
                                jsize dataArraySize = env->GetArrayLength(data);

                                char* nativeData = new char[dataArraySize];
                                jbyte* dataItems = env->GetByteArrayElements(data, 0);
                                for (int i = 0; i < dataArraySize; i++) {
                                    nativeData[i] = (char)dataItems[i];
                                }

                                jmethodID numBytesMethodID = env->GetStaticMethodID(mainClass, "getBluetoothReadNumBytes", "()I");
                                if (numBytesMethodID != nullptr) {
                                    jint javaNumBytes = env->CallStaticIntMethod(mainClass, numBytesMethodID);
                                    *numBytes = (int)javaNumBytes;
                                    env->ReleaseByteArrayElements(data, dataItems, JNI_ABORT);
                                    return nativeData;
                                }
                            }
                        }
#endif
                        return nullptr;
                    }

                    static void Write(char* buffer, int numBytes) {
                        /*Writes data to a connected device*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass mainClass = env->FindClass("com/jamar/penengine/MainActivity");
                        if (mainClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(mainClass, "bluetoothWrite", "(BI)V");
                            if (methodID != nullptr) {
                                for (int i = 0; i < numBytes; i++) {
                                    if (i == numBytes - 1) {
                                        env->CallStaticObjectMethod(mainClass, methodID, (jbyte)buffer[i], numBytes, i, 1);
                                    }
                                    else {
                                        env->CallStaticObjectMethod(mainClass, methodID, (jbyte)buffer[i], numBytes, i, 0);
                                    }
                                }
                            }
                        }
#endif
                    }

                    static void Close() {
                        /*Closes the connection to a connected device*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass mainClass = env->FindClass("com/jamar/penengine/MainActivity");
                        if (mainClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(mainClass, "closeBluetoothConn", "()V");
                            if (methodID != nullptr) {
                                env->CallStaticObjectMethod(mainClass, methodID);
                            }
                        }
#endif
                    }
                }
            }
        }
        
    }
}