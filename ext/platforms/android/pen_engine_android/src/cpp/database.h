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
#include <android/asset_manager_jni.h>
#include <iostream>
#include "../../../../../../src/state/config.h"
#include "../../../../../../src/state/state.h"

#ifdef __PEN_MOBILE__

extern "C" {
	namespace pen {
		namespace android {
			namespace db {
				bool AndroidDBStart(const char* dbName, const char* tableName) {
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

							bool success = env->CallStaticBooleanMethod(
								localStorageClass, methodID, javaDBName, javaTableName);

							env->DeleteLocalRef(javaDBName);
							env->DeleteLocalRef(javaTableName);

							return success;
						}
						return false;
					}
				}

				void AndroidDBClose() {
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

				void AndroidDBSetItem(const char* key, const char* value) {
					/*Adds or updates key value in database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"setItem",
							"(Ljava/lang/String;Ljava/lang/String;)V");
						if (methodID != nullptr) {
							jstring javaKey = env->NewStringUTF(key);
							jstring javaValue = env->NewStringUTF(value);

							env->CallStaticVoidMethod(localStorageClass, methodID, javaKey,
								javaValue);

							env->DeleteLocalRef(javaKey);
							env->DeleteLocalRef(javaValue);
						}
					}
				}

				std::string AndroidDBGetItem(const char* key) {
					/*Retrieve item from database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"getItem",
							"(Ljava/lang/String;)V");
						if (methodID != nullptr) {
							jstring javaKey = env->NewStringUTF(key);

							jstring javaValue = (jstring)env->CallStaticObjectMethod(
								localStorageClass, methodID, javaKey);
							const char* javaValueStrArr = env->GetStringUTFChars(javaValue,
								0);

							env->DeleteLocalRef(javaKey);
							env->DeleteLocalRef(javaValue);

							std::string javaValueStr = javaValueStrArr;
							return javaValueStr;
						}
						return "";
					}
					return "";
				}

				void AndroidDBRemoveItem(const char* key) {
					/*Remove key from database*/
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"removeItem",
							"(Ljava/lang/String;)V");
						if (methodID != nullptr) {
							jstring javaKey = env->NewStringUTF(key);

							env->CallStaticVoidMethod(localStorageClass, methodID, javaKey);

							env->DeleteLocalRef(javaKey);
						}
					}
				}

				void AndroidDBDeleteTable() {
					JNIEnv* env = (JNIEnv*)pen::State::Get()->javaEnv;
					/*Delete the table from database*/
					jclass localStorageClass = env->FindClass(
						"com/jamar/penengine/PenLocalStorage");
					if (localStorageClass != nullptr) {
						jmethodID methodID = env->GetStaticMethodID(localStorageClass,
							"clear", "()V");
						if (methodID != nullptr) {
							env->CallStaticVoidMethod(localStorageClass, methodID);
						}
					}
				}
			}
		}
		
	}
}
#endif