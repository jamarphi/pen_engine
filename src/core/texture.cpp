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
#include "texture.h"

Texture* Texture::instance = nullptr;

void Texture::Initialize(const std::string& path, const unsigned int slot) {
	/*Regular textures*/
	/*Removes the previous texture that occupied this slot*/
#ifndef __PEN_MAC_IOS__
	Destroy(Texture::Get()->texSlots.Find(slot)->second);
#endif

	/*Loads in the file used for this texture, if it is a default object, then the color white will be used instead*/
	stbi_set_flip_vertically_on_load(1);
	bool defaultPath = false;
#ifndef __PEN_CMAKE__
	defaultPath = true;
#endif
	std::string tempPath = (path.find(":") != std::string::npos) ? path : (defaultPath ? pen::State::Get()->penEngineDir + GENERAL_TEXTURE_SOURCE + path
		: (""));

	/*This adds the cmake absolute path for resources*/
	if (tempPath == "") tempPath = tempPath + ROOT_DIR + "res/textures/" + path;

	int texWidth = 0, texHeight = 0, texBPP = 0;
	uint32_t color = 0xffffffff;
	unsigned char* localBuffer = (path.compare("default") != 0) ? stbi_load(tempPath.c_str(), &texWidth, &texHeight, &texBPP, 4) : nullptr;

#ifndef __PEN_MAC_IOS__
    GLuint texRendererId = 0;
    glGenTextures(1, (GLuint*)&texRendererId);
        
    /*Assigns the new texture id to the slot*/
    Texture::Get()->texSlots.Insert(slot, texRendererId);
    
	/*Bind the texture to a specific slot*/
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texRendererId);

	/*Sets the parameters for textures loaded in*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif

	if (path.compare("default") == 0) {
		/*Uses the color white to load a blank texture that can be turned into any color*/
#ifndef __PEN_MAC_IOS__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
#else
		Texture::InitializePenMacIOSTexture(path, 0, slot);
#endif
	}
	else if (path.compare("pixel") == 0) {
		/*Uses a buffer of color values to draw quad-like pixels to the screen*/
#ifndef __PEN_MAC_IOS__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, pen::State::Get()->pixelArray);
#else
		Texture::InitializePenMacIOSTexture(path, 1, slot);
#endif
	}
	else {
		/*Uses the buffer data loaded to create a texture*/
#ifndef __PEN_MAC_IOS__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
#else
        tempPath = path;
        if (tempPath[2] == '/' && tempPath[3] == '/') {
            tempPath = tempPath.substr(4);
        }
        else if (tempPath[0] == '/') {
            tempPath = tempPath.substr(1);
        }
        tempPath = pen::Asset::ParsePath(tempPath);
		Texture::InitializePenMacIOSTexture(tempPath, 2, slot);
#endif
	}
}

void Texture::Destroy(unsigned int texRendererId) {
	/*Removes a texture from memory on the GPU*/
#ifndef __PEN_MAC_IOS__
	glDeleteTextures(1, &texRendererId);
#endif
}

void Texture::LoadTexture(const std::string* textureList, const unsigned int& listSize) {
	/*Initialize textures for the first time*/

	/*
	The name default is loaded as an asset (no file just name) for objects that don't have textures.
	The name fonts/bitmap.png is loaded as an asset for objects that use text from the bitmap font.
	The name pixel is loaded as an asset for pixel-by-pixel drawing.
	*/
#ifndef __PEN_MOBILE__
	std::vector<std::string> pathList = { "default","fonts/bitmap.png","pixel" };

	/*Load texture files from user list*/
	for (int i = 0; i < listSize; i++) {
		pathList.push_back(textureList[i]);
	}

	for (int j = 0; j < pathList.size(); j++)
		pen::Asset::Add(pen::Asset(pathList[j]));

	pen::Asset::assetMap.Find(1)->second.name = "fonts/bitmap.png";

	pen::State* inst = pen::State::Get();

	/*Initialize texture slot map position to value*/
	for (int k = 0; k < inst->textureUnits; k++) {
		Texture::Get()->texSlots.Insert(k, 0);
	}
#endif
}

void Texture::UpdatePixels() {
	/*Sends new pixel buffer texture to GPU*/
#ifndef __PEN_ANDROID__
	if (pen::State::Get()->pixelDrawn) {
		pen::State::Get()->pixelDrawn = false;
#ifndef __PEN_MAC_IOS__
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, Texture::Get()->texSlots.Find(2)->second);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1280, 720, GL_RGBA, GL_UNSIGNED_BYTE, pen::State::Get()->pixelArray);
#else
        MapMacIOSUpdatePixels();
#endif
	}
#endif
}

#ifdef __PEN_MAC_IOS__
void Texture::InitializePenMacIOSTexture(const std::string& path, const unsigned int& type, const unsigned int& texSlot) {
	/*Initialize textures for Metal*/
	uint32_t texWidth = 0;
	uint32_t texHeight = 0;
	int texSize = 0;
	unsigned char* textureData = nullptr;

	switch (type) {
	case 0:
		/*The solid color default*/
		texWidth = 128;
		texHeight = 128;
		texSize = texWidth * texHeight * 4;

		textureData = new unsigned char[texSize];
		for (int i = 0; i < texSize; i++) {
			textureData[i] = 0xFF;
		}
        MapMacIOSInitializeTexture(texWidth, texHeight, type, texSlot, textureData);
		break;
	case 1:
		/*The pixel buffer*/
		texWidth = 1280;
		texHeight = 720;
		textureData = pen::State::Get()->pixelArray;
        MapMacIOSInitializeTexture(texWidth, texHeight, type, texSlot, textureData);
		break;
	case 2:
		/*Textures loaded from memory*/
        MapMacIOSLoadTexture(path.c_str(), texSlot);
		break;
	default:
		break;
	}
}
#endif
