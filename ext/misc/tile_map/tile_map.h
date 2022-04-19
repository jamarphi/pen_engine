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
#include "../../../src/state/pixel.h"
#include <iostream>
#include <fstream>

namespace pen {
	namespace tiles {
		struct Tiles {
			/*Container for tile map image and collision data*/
			pen::Item* data;
			char* tiles;
			int count;
			int length;
		};

		static void Save(pen::tiles::Tiles* tileMap, const std::string& path) {
			/*Save the tile map collision data*/
			std::string fileName = path;
			if (tileMap->tiles != nullptr) {

				if(fileName.find(".") != std::string::npos) {
					fileName = fileName.substr(0, fileName.find(".")) + ".pentm";
				}
				else {
					fileName += ".pentm";
				}

				/*The first char of the buffer is the number of tiles*/
				std::ofstream tileFile(fileName, std::ios::out | std::ios::binary);
				if (tileFile.is_open()) {
					int tileCount = tileMap->count + 1;
					for (int i = 0; i < tileCount; i++) {
						if (i == 0) {
							tileFile.write((char*)&tileMap->length, sizeof(char));
						}
						else {
							tileFile.write((char*)&tileMap->tiles[i], sizeof(char));
						}
						
					}
					tileFile.close();
				}
			}
		}

		static pen::tiles::Tiles* Load(const std::string& path) {
			/*Load in tile map image and collision data*/
			std::string fileName = path;
			if (fileName.find(".") != std::string::npos) {
				fileName = fileName.substr(0, fileName.find(".")) + ".png";
			}
			else {
				fileName += ".png";
			}
			pen::Item* item = pen::CreateSprite(0, 0, pen::PixelBufferWidth(), pen::PixelBufferHeight(), path);

			char* tiles = nullptr;

			if (fileName.find(".") != std::string::npos) fileName = fileName.substr(0, fileName.length() - 4) + ".pentm";

			std::ifstream tileFile(fileName, std::ios::out | std::ios::binary);
			if (tileFile.is_open()) {
				char tileCountValue = (char)1000;
				int tileCount = 1;
				for (int i = 0; i < tileCount; i++) {
					if (i == 0) {
						tileFile.read((char*)&tileCountValue, sizeof(char));
						tileCount = (int)tileCountValue * (int)tileCountValue;
						tiles = new char[tileCount];
					}
					else {
						tileFile.read((char*)&tiles[i], sizeof(char));
					}
				}
				tileFile.close();

				if (tiles != nullptr) {
					return new pen::tiles::Tiles{ item, tiles, tileCount, (int)tileCountValue };
				}
			}
			return nullptr;
		}
	}
}