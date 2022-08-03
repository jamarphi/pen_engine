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
#ifndef __PEN_IOS__
	/*Regular textures*/
	/*Removes the previous texture that occupied this slot*/
	Destroy(Texture::Get()->texSlots.Find(slot)->second);

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

	if (path.compare("default") == 0) {
		/*Uses the color white to load a blank texture that can be turned into any color*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	}
	else if (path.compare("pixel") == 0) {
		/*Uses a buffer of color values to draw quad-like pixels to the screen*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, pen::State::Get()->pixelArray);
	}
	else {
		/*Uses the buffer data loaded to create a texture*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	}

	if (localBuffer)
		stbi_image_free(localBuffer);
#endif
}

void Texture::Destroy(unsigned int texRendererId) {
	/*Removes a texture from memory on the GPU*/
#ifndef __PEN_IOS__
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
#ifndef __PEN_MOBILE__
	if (pen::State::Get()->pixelDrawn) {
		pen::State::Get()->pixelDrawn = false;
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, Texture::Get()->texSlots.Find(2)->second);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1280, 720, GL_RGBA, GL_UNSIGNED_BYTE, pen::State::Get()->pixelArray);
	}
#endif
}