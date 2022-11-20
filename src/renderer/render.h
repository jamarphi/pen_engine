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
#include "../../ext/platforms/mac_ios/mac_ios_cpp_objective_c_mapping.h"

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
#ifndef __PEN_MAC_IOS__
		static void RenderLayer(pen::Layer *layer);
#else
        static void RenderLayer();
#endif

	private:
		static void TextureSet();
		static void UpdatedInstancedUniforms(pen::Layer* layer);
	};

	static void PanLayerCamera(float x, float y, float z) {
		/*Pans the layer camera*/
		pen::Render* inst = pen::Render::Get();
		if (x != 0.0f) inst->camera.cameraPosition += ((pen::op::CrossProduct(inst->camera.viewOrientation, inst->camera.at).Normalize()) * inst->camera.cameraSpeed * x);
		if (y != 0.0f) inst->camera.cameraPosition += (inst->camera.at * inst->camera.cameraSpeed * y);
		if (z != 0.0f) inst->camera.cameraPosition += (inst->camera.viewOrientation * inst->camera.cameraSpeed * z);
	}

	static void LookLayerCamera(pen::Vec3 direction) {
		/*Aims the layer camera*/
		pen::Render* inst = pen::Render::Get();
		float angleX = pen::op::ACos(pen::op::DotProduct(pen::Vec3(inst->camera.viewOrientation.x, 0.0f, 0.0f), pen::Vec3(direction.x, 0.0f, 0.0f)) / (inst->camera.viewOrientation.x * direction.x));
		float angleY = pen::op::ACos(pen::op::DotProduct(pen::Vec3(inst->camera.viewOrientation.y, 0.0f, 0.0f), pen::Vec3(direction.y, 0.0f, 0.0f)) / (inst->camera.viewOrientation.y * direction.y));
		float angleZ = pen::op::ACos(pen::op::DotProduct(pen::Vec3(inst->camera.viewOrientation.z, 0.0f, 0.0f), pen::Vec3(direction.z, 0.0f, 0.0f)) / (inst->camera.viewOrientation.z * direction.z));
		pen::Vec3 axisX = pen::Vec3(inst->camera.at.x, 0.0f, 0.0f);
		pen::Vec3 axisY = pen::Vec3(inst->camera.at.y, 0.0f, 0.0f);
		pen::Vec3 axisZ = pen::Vec3(inst->camera.at.z, 0.0f, 0.0f);

		pen::Vec3 newOrientation = pen::op::RotateVec(inst->camera.viewOrientation, -1.0f * angleX,
			(pen::op::CrossProduct(inst->camera.viewOrientation, axisX).Normalize()));

		if (!(pen::op::AngleBetween(newOrientation, axisX) <= 5.0f * 3.14159f / 180.0f
			|| pen::op::AngleBetween(newOrientation, axisX * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
			inst->camera.viewOrientation = newOrientation;
		}

		newOrientation = pen::op::RotateVec(inst->camera.viewOrientation, -1.0f * angleY,
			(pen::op::CrossProduct(inst->camera.viewOrientation, axisY).Normalize()));

		if (!(pen::op::AngleBetween(newOrientation, axisY) <= 5.0f * 3.14159f / 180.0f
			|| pen::op::AngleBetween(newOrientation, axisY * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
			inst->camera.viewOrientation = newOrientation;
		}

		inst->camera.viewOrientation = pen::op::RotateVec(inst->camera.viewOrientation, -1.0f * angleZ,
			(pen::op::CrossProduct(inst->camera.viewOrientation, axisZ).Normalize()));
	}
}
