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
#include <cstring>
#include "config.h"
#ifndef __PEN_MAC_IOS__
#include "../../dependencies/glad/glad.h"
#endif
#ifndef __PEN_MOBILE__
#include "../../dependencies/glfw/include/glfw3.h"
#endif
#include "../core/character.h"
#include "../objects/containers/map.h"
#include "../ops/operations/trig.h"

#define DEBUG_ID 456789
#define PIXEL_DRAWING_ID 123456
#define ID_ANY 476890

namespace pen {
	struct Sprite {
		std::string name;
		unsigned char* data;
		int width;
		int height;
	};

	class State {
		static State* instance;
	public:
		void* appWindow;
		std::string penEngineDir;
		unsigned int textScaling;
		unsigned int screenWidth;
		unsigned int screenHeight;
		unsigned int actualScreenWidth;
		unsigned int actualScreenHeight;
		unsigned int textureUnits;
		float deltaTime = 1.0f;
		bool firstUpdateFrame = true;
		bool updateBatch = true;
		bool debug = false;
		bool handleGUIClickEvents = false;
		bool handleGUIDragEvents = false;
		bool handleGUIKeyEvents = false;
		bool handleCameraInput = false;
		void* draggableItem = nullptr;
		void* keyableItem = nullptr;

		/*----For pixel-by-pixel drawing----*/
		bool usingBuffer = false;
		unsigned char* pixelArray = nullptr;
		bool pixelDrawn = false;
		int pixelBufferWidth = 1280;
		int pixelBufferHeight = 720;
		pen::Map <std::string, pen::Sprite> pixelSprites;
		/*----For pixel-by-pixel drawing----*/
		
		/*This map is used for text character to unsigned char conversion*/
		pen::Map<std::string, unsigned char> asciiMap;

		/*This map is used to contain the metadata for each character being rendered.  This includes things like position and offset data*/
		pen::Map<char, Character> characters;

		/*----Mobile----*/
		bool mobileActive = false;
		bool mobileHorizontal = false;
		double mobileMouseX = 0.0;
		double mobileMouseY = 0.0;
		void (*mobileOnKeyCallback)(char) = nullptr;
		void (*mobileOnClickCallback)(double, double) = nullptr;
		void (*mobileOnRenderCallback)() = nullptr;
		void (*mobileOnResumeCallback)() = nullptr;
		void (*mobileOnPauseCallback)() = nullptr;
		void (*mobileOnTiltCallback)(double, double, double, double) = nullptr;
		void (*mobileAndroidOnBluetoothCallback)() = nullptr;
        void (*mobileMacIosOnBluetoothCallback)(char*, long) = nullptr;

		/*----Android----*/
		void* javaEnv = nullptr;
		std::vector<std::string> mobileTextureNameList;
		void* androidAssetManager = nullptr;
		/*----Android----*/
		/*----Mobile----*/
	public:
		static State* Get() {
			if (!instance)
				instance = new State();
			return instance;
		}

		static double MouseScrollXOffset();
		static double MouseScrollYOffset();
		static void SetDraggable(void* item);
		static void SetKeyable(void* item);
		
		/*Loads the character data in pen::Pen::CreateApplication()*/
		static void LoadCharacters();
	};
}
