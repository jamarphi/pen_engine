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

#include "../../../src/state/config.h"
#include "../../../src/state/state.h"
#include "../../../src/state/asset.h"
#include "mac_ios_cpp_objective_c_mapping.h"

namespace pen {
    namespace ios {
        namespace sound {
            static void Play(const char* file, unsigned int loopNumber = 0){
                /*Plays a sound*/
#ifndef __PEN_MAC_IOS__
                std::string fileName(file);
                if (fileName[2] == '/' && fileName[3] == '/') {
                    fileName = fileName.substr(4);
                }
                else if (fileName[0] == '/') {
                    fileName = fileName.substr(1);
                }
                fileName = pen::Asset::ParsePath(file);
                MapMacPenMacIOSSoundPlay(fileName.c_str(), loopNumber);
#endif
            }
        
            static void Pause(const char* file){
                /*Pauses a sound*/
#ifndef __PEN_MAC_IOS__
                std::string fileName(file);
                if (fileName[2] == '/' && fileName[3] == '/') {
                    fileName = fileName.substr(4);
                }
                else if (fileName[0] == '/') {
                    fileName = fileName.substr(1);
                }
                fileName = pen::Asset::ParsePath(file);
                MapMacPenMacIOSSoundPause(fileName.c_str());
#endif
            }
            
            static void Stop(const char* file){
                /*Stops a sound*/
#ifndef __PEN_MAC_IOS__
                std::string fileName(file);
                if (fileName[2] == '/' && fileName[3] == '/') {
                    fileName = fileName.substr(4);
                }
                else if (fileName[0] == '/') {
                    fileName = fileName.substr(1);
                }
                fileName = pen::Asset::ParsePath(file);
                MapMacPenMacIOSSoundStop(fileName.c_str());
#endif
            }
            
            static void Remove(const char* file){
                /*Removes a sound*/
#ifndef __PEN_MAC_IOS__
                std::string fileName(file);
                if (fileName[2] == '/' && fileName[3] == '/') {
                    fileName = fileName.substr(4);
                }
                else if (fileName[0] == '/') {
                    fileName = fileName.substr(1);
                }
                fileName = pen::Asset::ParsePath(file);
                MapMacPenMacIOSSoundRemove(fileName.c_str());
#endif
            }
        }
    }
}


