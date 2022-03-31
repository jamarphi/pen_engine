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
		
		std::string fileName = assetPath;

		if (fileName[2] == '/' && fileName[3] == '/') {
			fileName = fileName.substr(4);
		}
		else if (fileName[0] == '/') {
			fileName = fileName.substr(1);
		}

		path = Asset::ParsePath(fileName);
		root = fileName.substr(0, fileName.find(path));
		data = nullptr;
	}

	Asset::~Asset() {}

	void Asset::Add(Asset asset) {
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
}