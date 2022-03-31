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
#include "jni/jni.h"

#ifdef __PEN_MOBILE__

#include <cstring>
#include <vector>
#include <android/asset_manager.h>

extern "C" {
    static char* AndroidLoad(void* assetManager, const char* path) {
        /*Load in a file from assets directory*/
        AAset* javaAsset = AAsetManager_open((AAssetManager*)assetManager, path, AASET_MODE_UNKNOWN);
        int byteCount = AAsset_getLength(javaAsset);
        char* data = new char[byteCount];
        AAsset_read(javaAsset, &data, byteCount);
        return data;
    }

    static std::vector<std::string> AndroidLoadDir(void* assetManager, const char* dirPath) {
        /*Load in a directory from assets directory*/
        std::vector<std::string> pathList;
        AAssetDir* dir = AAssetManager_openDir((AAssetManager*)assetManager, dirPath);
        char* input = nullptr;
        std::string inputStr = "";
        while (true) {
            input = AAssetDir_getNextFileName(dir);
            if (input == nullptr) {
                break;
            }
            inputStr = input;
            pathList.push_back(inputStr);
        }
        AAssetDir_close(dir);
    }
}
#endif