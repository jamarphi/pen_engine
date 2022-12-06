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
#include "../state/config.h"
#ifndef __PEN_MAC_IOS__
#include "../../dependencies/glad/glad.h"
#endif
#ifndef __PEN_MOBILE__
#include "../../dependencies/glfw/include/glfw3.h"
#endif
#include "../ops/vectors/vec3.h"
#include "../ops/matrices/mat4x4.h"
#include "../state/state.h"
#include "../core/shader.h"
#include "../ops/operations/operations.h"
#include "../state/keys.h"
#include "../state/layer_manager.h"
#include "../../ext/platforms/mac_ios/mac_ios_cpp_objective_c_mapping.h"
#ifdef __PEN_MAC_IOS__
#include <TargetConditionals.h>
#endif

namespace pen {
	class Camera {
	public:
		unsigned int screenWidth;
		unsigned int screenHeight;
		bool firstDrag = true;
		bool firstMove = true;
		float cameraSpeed = 1.0f;
		pen::Vec3 cameraPosition;
		pen::Vec3 viewOrientation = pen::Vec3(0.0f, 0.0f, -1.0f);
		pen::Vec3 up = pen::Vec3(0.0f, 1.0f, 0.0f);
		pen::Vec3 at = pen::Vec3(0.0f, 1.0f, 0.0f);
		pen::Vec3 right = pen::Vec3(0.0f, 1.0f, 0.0f);
		float cameraFov;
		float cameraZNear;
		float cameraZFar;
		float pitchX = -90.0f;
		float pitchY = 0.0f;

		Camera();
		Camera(pen::Vec3 viewPosition, int viewWidth, int viewHeight, float speed = 0.1f);
		~Camera();
		void Update();
		void Update(float zNear, float zFar, pen::Mat4x4* view, pen::Mat4x4* proj, pen::Layer* layer);
		void Update(float zNear, float zFar, pen::Mat4x4* view, pen::Mat4x4* proj);
#ifndef __PEN_MAC_IOS__
        bool HandleInput(void* cameraWindow, bool isLayerCamera);
#else
        bool HandleInput(int key, int action, bool isLayerCamera);
#endif
	private:
		void UpdateViewOrientation();
	};
}
