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
#include <vector>
#include <string>
#include "../objects/item.h"
#include "../objects/layer.h"
#include "renderer.h"
#include "../core/color.h"
#include "../state/asset.h"
#include "../core/texture.h"
#include "../ops/matrices/mat4x4.h"
#include "../ops/operations/operations.h"
#include "camera.h"
#include "../../ext/platforms/ios/ios_cpp_objective_c_mapping.h"

namespace pen {
	class Render {
		static Render* instance;
	public:
		bool firstTime = true;
		pen::Shader appShader;
		pen::Shader instancedShader;
		pen::Mat4x4 appOrthoProj;
		pen::Mat4x4 appPerspectiveProj;
		pen::Mat4x4 appOrthoView;
		pen::Mat4x4 appPerspectiveView;
		std::vector<float> appOrthoCoord;
		pen::Camera camera;
		unsigned int textScaling;
		int indexCount = 0;

		static Render* Get() {
			if (!instance)
				instance = new Render();
			return instance;
		}

	public:
		static void Background(pen::Vec4 color);
		static void RenderLayer(pen::Layer *layer);

	private:
		static void TextureSet();
		static void UpdatedInstancedUniforms(pen::Layer* layer);
	};
}
