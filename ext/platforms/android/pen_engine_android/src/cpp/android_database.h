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
#include <iostream>
#include "../../../../../../src/state/config.h"
#include "../../../../../../src/state/state.h"

#ifdef __PEN_ANDROID__
#include <android/asset_manager_jni.h>

extern "C" {
	namespace pen {
		namespace android {
			namespace db {
				void Start(const char* dbName, const char* tableName) {
					/*Initialize the database on the Java end*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"init",
							"(Ljava/lang/String;Ljava/lang/String;)B");
						if (methodID != nullptr) {
							jstring javaDBName = env->NewStringUTF(dbName);
							jstring javaTableName = env->NewStringUTF(tableName);

							env->CallStaticVoidMethod(
								localStorageClass, methodID, javaDBName, javaTableName);

							env->DeleteLocalRef(javaDBName);
							env->DeleteLocalRef(javaTableName);
						}
					}
				}
            
                void CreateTable(const char* tableName) {
                    /*Initialize the database on the Java end*/
                    JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                    jclass localStorageClass = env->FindClass(
                        "com/jamar/penengine/PenLocalStorage");
                    if (localStorageClass != nullptr) {
                        jmethodID methodID = env->GetStaticMethodID(localStorageClass,
                            "createTable",
                            "(Ljava/lang/String;)V");
                        if (methodID != nullptr) {
                            jstring javaDBName = env->NewStringUTF(dbName);
                            jstring javaTableName = env->NewStringUTF(tableName);

                            env->CallStaticVoidMethod(
                                localStorageClass, methodID, javaDBName, javaTableName);

                            env->DeleteLocalRef(javaDBName);
                            env->DeleteLocalRef(javaTableName);
                        }
                    }
                }

				void Close() {
					/*Close the database connection*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"destroy", "()V");
						if (methodID != nullptr) {
							env->CallStaticVoidMethod(localStorageClass, methodID);
						}
					}
				}

				void Insert(const char* table, const char* key, const char* value) {
					/*Adds or updates key value in database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"setItem",
							"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
						if (methodID != nullptr) {
                            jstring javaTable = env->NewStringUTF(table);
							jstring javaKey = env->NewStringUTF(key);
							jstring javaValue = env->NewStringUTF(value);

							env->CallStaticVoidMethod(localStorageClass, methodID, javaTable, javaKey,
								javaValue);

                            env->DeleteLocalRef(javaTable);
							env->DeleteLocalRef(javaKey);
							env->DeleteLocalRef(javaValue);
						}
					}
				}

				std::string Get(const char* table, const char* key) {
					/*Retrieve item from database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"getItem",
							"(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
						if (methodID != nullptr) {
                            jstring javaTable = env->NewStringUTF(table);
							jstring javaKey = env->NewStringUTF(key);

							jstring javaValue = (jstring)env->CallStaticObjectMethod(
								localStorageClass, methodID, javaTable, javaKey);
							const char* javaValueStrArr = env->GetStringUTFChars(javaValue,
								0);

                            env->DeleteLocalRef(javaTable);
							env->DeleteLocalRef(javaKey);
							env->DeleteLocalRef(javaValue);

							std::string javaValueStr = javaValueStrArr;
							return javaValueStr;
						}
						return "";
					}
					return "";
				}

				void Delete(const char* table, const char* key) {
					/*Remove key from database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"removeItem",
							"(Ljava/lang/String;)V");
						if (methodID != nullptr) {
                            jstring javaTable = env->NewStringUTF(table);
							jstring javaKey = env->NewStringUTF(key);

							env->CallStaticVoidMethod(localStorageClass, methodID, javaTable, javaKey);

                            env->DeleteLocalRef(javaTable);
							env->DeleteLocalRef(javaKey);
						}
					}
				}
            
                void Clear(const char* table) {
                    JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
                    /*Delete everythingfrom the table*/
                    jclass localStorageClass = env->FindClass(
                        "com/jamar/penengine/PenLocalStorage");
                    if (localStorageClass != nullptr) {
                        jmethodID methodID = env->GetStaticMethodID(localStorageClass,
                            "clearTable", "(Ljava/lang/String;)V");
                        if (methodID != nullptr) {
                            jstring javaTable = env->NewStringUTF(table);
                            
                            env->CallStaticVoidMethod(localStorageClass, methodID, table);
                        }
                    }
                }

				void Drop(const char* table) {
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					/*Delete the table from the database*/
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"dropTable", "(Ljava/lang/String;)V");
						if (methodID != nullptr) {
                            jstring javaTable = env->NewStringUTF(table);
                            
							env->CallStaticVoidMethod(localStorageClass, methodID, table);
						}
					}
				}
			}
		}
		
	}
}
#endif
