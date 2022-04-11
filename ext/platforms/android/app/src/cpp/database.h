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
		namespace platforms {
			namespace android {
				namespace db {
					bool AndroidDBStart(const char* dbName, const char* tableName) {
						/*Initialize the database on the Java end*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"init",
								"(Ljava/lang/String;Ljava/lang/String;)B");
							if (methodID != nullptr) {
								jstring javaDBName = AndroidDB::env->NewStringUTF(dbName);
								jstring javaTableName = AndroidDB::env->NewStringUTF(tableName);

								bool success = AndroidDB::env->CallStaticBooleanMethod(
									localStorageClass, methodID, javaDBName, javaTableName);

								AndroidDB::env->DeleteLocalRef(javaDBName);
								AndroidDB::env->DeleteLocalRef(javaTableName);

								return success;
							}
							return false;
						}
					}

					void AndroidDBClose() {
						/*Close the database connection*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"destroy", "()V");
							if (methodID != nullptr) {
								AndroidDB::env->CallStaticVoidMethod(localStorageClass, methodID);
							}
						}
					}

					void AndroidDBSetItem(const char* key, const char* value) {
						/*Adds or updates key value in database*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"setItem",
								"(Ljava/lang/String;Ljava/lang/String;)V");
							if (methodID != nullptr) {
								jstring javaKey = AndroidDB::env->NewStringUTF(key);
								jstring javaValue = AndroidDB::env->NewStringUTF(value);

								AndroidDB::env->CallStaticVoidMethod(localStorageClass, methodID, javaKey,
									javaValue);

								AndroidDB::env->DeleteLocalRef(javaKey);
								AndroidDB::env->DeleteLocalRef(javaValue);
							}
						}
					}

					std::string AndroidDBGetItem(const char* key) {
						/*Retrieve item from database*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"getItem",
								"(Ljava/lang/String;)V");
							if (methodID != nullptr) {
								jstring javaKey = AndroidDB::env->NewStringUTF(key);

								jstring javaValue = (jstring)AndroidDB::env->CallStaticObjectMethod(
									localStorageClass, methodID, javaKey);
								const char* javaValueStrArr = AndroidDB::env->GetStringUTFChars(javaValue,
									0);

								AndroidDB::env->DeleteLocalRef(javaKey);
								AndroidDB::env->DeleteLocalRef(javaValue);

								std::string javaValueStr = javaValueStrArr;
								return javaValueStr;
							}
							return "";
						}
						return "";
					}

					void AndroidDBRemoveItem(const char* key) {
						/*Remove key from database*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"removeItem",
								"(Ljava/lang/String;)V");
							if (methodID != nullptr) {
								jstring javaKey = AndroidDB::env->NewStringUTF(key);

								AndroidDB::env->CallStaticVoidMethod(localStorageClass, methodID, javaKey);

								AndroidDB::env->DeleteLocalRef(javaKey);
							}
						}
					}

					void AndroidDBDeleteTable() {
						/*Delete the table from database*/
						jclass localStorageClass = AndroidDB::env->FindClass(
							"com/jamar/penengine/PenLocalStorage");
						if (localStorageClass != nullptr) {
							jmethodID methodID = AndroidDB::env->GetStaticMethodID(localStorageClass,
								"clear", "()V");
							if (methodID != nullptr) {
								AndroidDB::env->CallStaticVoidMethod(localStorageClass, methodID);
							}
						}
					}
				}
			}
		}
	}
}
#endif