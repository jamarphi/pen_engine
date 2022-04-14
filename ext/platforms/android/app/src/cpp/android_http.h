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
                namespace http {
                    static pen::Map<std::string,std::string> Get(const char* route, pen::Map<std::string, std::string> parameterMap, pen::Map<std::string, std::string> headerMap) {
                        /*Sends an http get request*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass httpClass = env->FindClass("com/jamar/penengine/PenHttp");
                        if (httpClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(httpClass, "httpGet", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
                            if (methodID != nullptr) {
                                /*Parse parameters*/
                                pen::Map<std::string, std::string> responseMap;
                                std::string parameters = "";
                                for (int i = 0; i < parameterMap.Size(); i++) {
                                    parameters += (parameterMap.items[i].first + "^" + parameterMap.items[i].second + ";");
                                }
                                parameters.substr(0, parameters.length() - 1);

                                /*Parse headers*/
                                std::string headers = "";
                                for (int i = 0; i < headerMap.Size(); i++) {
                                    headers += (headerMap.items[i].first + "^" + headerMap.items[i].second + ";");
                                }
                                headers.substr(0, headers.length() - 1);

                                jstring javaRoute = env->NewStringUTF(route);
                                jstring javaParameters = env->NewStringUTF(parameters.c_str());
                                jstring javaHeaders = env->NewStringUTF(headers.c_str());
                                jvalue args[3];
                                args[0].l = javaRoute;
                                args[1].l = javaParameters;
                                args[2].l = javaHeaders;
                                jstring javaResponse = (jstring)env->CallStaticObjectMethodA(httpClass, methodID, args);
                                const char* responseStr = env->GetStringUTFChars(javaResponse, 0);
                                std::string response(responseStr);

                                /*Parse the response*/
                                std::string::iterator it;
                                std::string key = "";
                                std::string value = "";
                                bool printKey = true;
                                for (it = response.begin(); it != response.end(); it++) {
                                    if (*it == '\n') {
                                        responseMap.Insert(key, value);
                                        key = "";
                                        value = "";
                                        printKey = true;
                                    }
                                    else if (*it == ':') {
                                        printKey = false;
                                    }
                                    else {
                                        if (printKey) {
                                            key += *it;
                                        }
                                        else {
                                            value += *it;
                                        }
                                    }
                                }
                                env->DeleteLocalRef(javaRoute);
                                env->DeleteLocalRef(javaParameters);
                                env->DeleteLocalRef(javaHeaders);
                                env->DeleteLocalRef(javaResponse);
                                env->DeleteLocalRef((jobject)args);
                                delete[] responseStr;
                                return responseMap;
                            }
                        }
#endif
                        return pen::Map<std::string, std::string>();
                    }

                    static pen::Map<std::string, std::string> Post(const char* route, const char* jsonStr, pen::Map<std::string, std::string> headerMap) {
                        /*Sends an http post request*/
#ifdef __PEN_MOBILE__
                        JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                        jclass httpClass = env->FindClass("com/jamar/penengine/PenHttp");
                        if (httpClass != nullptr) {
                            jmethodID methodID = env->GetStaticMethodID(httpClass, "httpPost", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
                            if (methodID != nullptr) {
                                /*Parse headers*/
                                pen::Map<std::string, std::string> responseMap;
                                std::string headers = "";
                                for (int i = 0; i < headerMap.Size(); i++) {
                                    headers += (headerMap.items[i].first + "^" + headerMap.items[i].second + ";");
                                }
                                headers.substr(0, headers.length() - 1);

                                jstring javaRoute = env->NewStringUTF(route);
                                jstring javaJson = env->NewStringUTF(jsonStr);
                                jstring javaHeaders = env->NewStringUTF(headers.c_str());
                                jvalue args[3];
                                args[0].l = javaRoute;
                                args[1].l = javaJson;
                                args[2].l = javaHeaders;
                                jstring javaResponse = (jstring)env->CallStaticObjectMethodA(httpClass, methodID, args);
                                const char* responseStr = env->GetStringUTFChars(javaResponse, 0);
                                std::string response(responseStr);

                                /*Parse the response*/
                                std::string::iterator it;
                                std::string key = "";
                                std::string value = "";
                                bool printKey = true;
                                for (it = response.begin(); it != response.end(); it++) {
                                    if (*it == '\n') {
                                        responseMap.Insert(key, value);
                                        key = "";
                                        value = "";
                                        printKey = true;
                                    }
                                    else if (*it == ':') {
                                        printKey = false;
                                    }
                                    else {
                                        if (printKey) {
                                            key += *it;
                                        }
                                        else {
                                            value += *it;
                                        }
                                    }
                                }
                                env->DeleteLocalRef(javaRoute);
                                env->DeleteLocalRef(javaJson);
                                env->DeleteLocalRef(javaHeaders);
                                env->DeleteLocalRef(javaResponse);
                                env->DeleteLocalRef((jobject)args);
                                delete[] responseStr;
                                return responseMap;
                            }
                        }
#endif
                        return pen::Map<std::string, std::string>();
                    }
                }
            }
        }

    }
}