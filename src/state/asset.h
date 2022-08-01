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
#include "state.h"
#include "../objects/containers/map.h"
#ifdef __PEN_ANDROID__
#include "../../ext/platforms/android/pen_engine_android/src/cpp/android_asset.h"
#endif

#define ASSET_REGEX "#$#"

namespace pen {
	class Asset {
	public:
		static pen::Map<unsigned int, Asset> assetMap;
		static uint16_t nextId;
		uint16_t id;
		std::string root;
		std::string name;
		char* data;
		long length;
	public:
		Asset();
		Asset(const std::string& assetPath);
		~Asset();

		static void Add(Asset asset);
		static Asset Find(const std::string& fileName);
		static std::string FindById(const unsigned int& queryId);
		static unsigned int GetId(const std::string& name);
		static Asset Load(std::string file, char* (*onLoad)(const char* path, long* fileLength) = nullptr);
#ifdef __PEN_MOBILE__
		static Asset* LoadMobileDir(std::string dirPath, int* assetCount, bool loadNow = true);
#endif
		static std::string ParsePath(std::string fileName);
	private:
		static std::string Split(const std::string& line, const char& character, const unsigned int& section);
	};
}
