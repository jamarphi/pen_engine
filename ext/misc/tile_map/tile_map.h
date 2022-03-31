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

namespace pen {
	namespace tiles {
		struct Tiles {
			/*Container for tile map image and collision data*/
			pen::Item* data;
			char* tiles;
		};

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
				char tileCount = 1000001;
				for (int i = 0; i < tileCount; i++) {
					if (i == 0) {
						tileFile.read((char*)&tileCount, sizeof(char));
						tiles = new char[(int)tileCount];
						tiles[0] = tileCount;
					}
					else {
						tileFile.read((char*)&tiles[i], sizeof(char));
					}
				}
				tileFile.close();
			}

			if (tiles != nullptr) {
				return new pen::tiles::Tiles{item, tiles};
			}
			return nullptr;
		}
	}
}