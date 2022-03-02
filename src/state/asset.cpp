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
#include "asset.h"

namespace pen {
	pen::Map<unsigned int, Asset> Asset::assetMap = {};
	uint16_t Asset::id = 0;

	Asset::Asset(const std::string& assetPath) {
		/*Creates a new asset and assigns it with an id*/
		personalId = Asset::id;
		Asset::id++;
		path = assetPath;
	}

	Asset::~Asset() {}

	void Asset::AddAsset(Asset asset) {
		/*This adds the asset to memory*/
		Asset::assetMap.Insert(asset.personalId,asset);
	}

	std::string Asset::Find(const unsigned int& queryId) {
		/*Finds an asset based on its assigned id, this is used for determining which assets should be in which asset group*/
	    return (Asset::assetMap.Find(queryId) != nullptr) ? Asset::assetMap.Find(queryId)->second.path : ASSET_REGEX;
	}

	unsigned int Asset::GetId(const std::string& name) {
		/*Grabs the id of an asset based on the given name*/
		for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
			if (pen::Asset::assetMap.items[i].second.path == name) return pen::Asset::assetMap.items[i].second.personalId;
		}
	}

	void Asset::Activate(const std::string& queryId) {
		/*Activates an asset so it does not get created each render as a texture*/
		for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
			if (pen::Asset::assetMap.items[i].second.path == queryId) {
				pen::Asset::assetMap.items[i].second.active = true;
				break;
			}
		}
	}

	void Asset::Deactivate(const int& assetGroupId) {
		/*Deactivates all assets that are not part of the current asset group to be rendered*/
		pen::State* inst = pen::State::Get();
		unsigned int counter = 0;
		unsigned int minBound = assetGroupId * inst->textureUnits;
		unsigned int maxBound = (assetGroupId + 1) * inst->textureUnits > Asset::id ? (assetGroupId * inst->textureUnits + (Asset::id
			- (assetGroupId * inst->textureUnits))) : (assetGroupId + 1) * inst->textureUnits;

		for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
			if (counter < minBound || counter >= maxBound) pen::Asset::assetMap.items[i].second.active = false;
			counter++;	
		}
	}
}