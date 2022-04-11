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
#include "pen_helper.h"

#ifdef __PEN_MOBILE__

static const std::string className = "com.jamar.penengine.pen_helper";

static void* s_ctx = nullptr;

static int __deviceSampleRate = 44100;
static int __deviceAudioBufferSizeInFrames = 192;

static std::string g_apkPath;

extern "C" {

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenHelper_nativeSetContext(JNIEnv* env, jobject obj, jobject context, jobject assetManager) {
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenHelper_nativeSetAudioDeviceInfo(JNIEnv* env, jobject obj, jboolean isSupportLowLatency, jint deviceSampleRate, jint deviceAudioBufferSizeInFrames) {
        __deviceSampleRate = deviceSampleRate;
        __deviceAudioBufferSizeInFrames = deviceAudioBufferSizeInFrames;
    }

}
#endif
