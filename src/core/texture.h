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
#include "../renderer/renderer.h"
#include "../util/stb_image/stb_image.h"
#include <iostream>
#include "../state/state.h"
#include "../state/asset.h"
#include "character.h"
#include "../objects/containers/map.h"
#include "../state/config.h"

#ifdef __PEN_CMAKE__
#include <abs_asset_dir_config.h>
#endif

#define GENERAL_TEXTURE_SOURCE "/pen_engine/res/textures/"

class Texture {
	static Texture* instance;
public:
	pen::Map<unsigned int, GLuint> texSlots;
	pen::Map<char, Character> characters;

	static Texture* Get() {
		if (!instance)
			instance = new Texture();
		return instance;
	}

	static void Initialize(const std::string& path, const unsigned int slot);
	static void Destroy(unsigned int texRendererId);
	static void LoadTexture(const std::string* textureList, const unsigned int& listSize);
	static void UpdatePixels();
};
