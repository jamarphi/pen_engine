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
	uint16_t Asset::nextId = 0;

	Asset::Asset() {

	}

	Asset::Asset(const std::string& assetPath) {
		/*Creates a new asset and assigns it with an id*/
		id = Asset::nextId;
		Asset::nextId++;
		
		std::string fileName = assetPath;

		if (fileName[2] == '/' && fileName[3] == '/') {
			fileName = fileName.substr(4);
		}
		else if (fileName[0] == '/') {
			fileName = fileName.substr(1);
		}

		name = Asset::ParsePath(fileName);
		root = fileName.substr(0, fileName.find(name));
		data = nullptr;
	}

	Asset::~Asset() {}

	void Asset::Add(Asset asset) {
		/*This adds the asset to memory*/
		Asset::assetMap.Insert(asset.id,asset);
	}

	std::string Asset::Find(const unsigned int& queryId) {
		/*Finds an asset based on its assigned id, this is used for determining which assets should be in which asset group*/
	    return (Asset::assetMap.Find(queryId) != nullptr) ? Asset::assetMap.Find(queryId)->second.name : ASSET_REGEX;
	}

	unsigned int Asset::GetId(const std::string& name) {
		/*Grabs the id of an asset based on the given name*/
		for (int i = 0; i < pen::Asset::assetMap.Size(); i++) {
			if (pen::Asset::assetMap.items[i].second.name == name) return pen::Asset::assetMap.items[i].second.id;
		}
	}

	std::string Asset::Split(const std::string& line, const char& character, const unsigned int& section) {
		/*Split a line by a given character*/
		int counter = 0;
		int previousSectionIdx = 0;
		bool keepGoing = true;
		if (line.find(character) != std::string::npos) {
			while (keepGoing) {
				keepGoing = false;
				if (line.find(character, previousSectionIdx + 1) != std::string::npos) {
					if (counter == section) {
						if (counter > 0) {
							int startIdx = previousSectionIdx + 1;
							int endIdx = line.find(character, previousSectionIdx + 1);

							return line.substr(startIdx, endIdx - startIdx);
						}
						else {
							return line.substr(0, line.find(character));
						}
					}
					else {
						previousSectionIdx = line.find(character, previousSectionIdx + 1);
						counter++;
						keepGoing = true;
					}
				}
				else {
					if(counter > 0) return line.substr(previousSectionIdx + 1);
				}
			}
			return "";
		}
		else {
			return line;
		}
	}

	std::string Asset::ParsePath(std::string fileName) {
		/*Parses the file name for displaying*/
		std::vector<std::string> pathList;
		std::string path = "";
		std::string prevPath = "";
		bool keepGoing = true;
		int counter = 0;

		while (keepGoing) {
			path = Asset::Split(fileName, '/', counter);

			if (path == prevPath) {
				keepGoing = false;
				break;
			}

			if (path == "") {
				keepGoing = false;
				break;
			}

			counter++;

			/*Load in each path*/
			prevPath = path;

			pathList.push_back(path);
		}

		std::string pathName = pathList[pathList.size() - 1];
		return pathName;
	}

	Asset Asset::Load(std::string file, char* (*onLoad)(const char* path, long* fileLength)) {
		/*Returns an asset after loading in file*/
		char* fileData = nullptr;
		std::string fileName = "";
		std::string fileRoot = "";
		long length = 0;
#ifndef __PEN_MOBILE__
		/*File name gets parsed after loading in file since file should initially have full path*/
		fileData = (*onLoad)(file.c_str(), &length);
		if (file[2] == '/' && file[3] == '/') {
			file = file.substr(4);
		}
		else if (fileName[0] == '/') {
			file = file.substr(1);
		}
		fileName = Asset::ParsePath(file);
		fileRoot = fileName.substr(0, file.find(fileName));
#else
		/*File name gets parsed prior to loading for android since only the name of the file is needed*/
		if (file[2] == '/' && file[3] == '/') {
			file = file.substr(4);
		}
		else if (fileName[0] == '/') {
			file = file.substr(1);
		}
		fileName = Asset::ParsePath(file);
		fileRoot = fileName.substr(0, file.find(fileName));
		fileData = AndroidLoad(pen::State::Get()->androidAssetManager, fileName, &length);
#endif
		Asset asset = Asset();
		asset.id = Asset::nextId;
		Asset::nextId++;
		asset.name = fileName;
		asset.root = fileRoot;
		asset.data = fileData;
		asset.length = length;
		Asset::assetMap.Insert(asset.id, asset);
		return asset;
	}

#ifdef __PEN_MOBILE__
	Asset* Asset::LoadMobileDir(std::string dirPath, int* assetCount) {
		/*Load in a sub directory in the android assets directory*/
		std::vector<std::string> pathList = AndroidLoadDir(pen::State::Get()->androidAssetManager, dirPath);

		Asset* assets = new Asset[pathList.size()];
		*assetCount = pathList.size();
		for (int i = 0; i < pathList.size(); i++) {
			assets[i] = Asset::Load(pathList[i], nullptr);
		}
		return assets;
	}
#endif
}