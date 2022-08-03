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
#include <iostream>
#include "../state/config.h"
#ifndef __PEN_IOS__
#include "../../dependencies/glad/glad.h"
#endif
#include "../ops/vectors/vec2.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec4.h"
#include "../ops/matrices/mat4x4.h"
#include "../ops/matrices/mat2x4.h"
#include "../objects/containers/map.h"

namespace pen {
	class Shader {
	public:
		unsigned int rendererId;
		pen::Map<std::string, GLint> uniformLocationCache;

#ifndef __PEN_ES__
		const char* shaderProgram = "#version 400 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec4 inColor;\n"
			"layout(location = 2) in vec2 inTexCoord;\n"
			"layout(location = 3) in float inTexIndex;\n"
			"out vec4 color;\n"
			"out vec2 texCoord;\n"
			"out float texIndex;\n"
			"uniform mat4 uMVP;\n"
			"void main() {\n"
			"color = inColor;\n"
			"texCoord = inTexCoord;\n"
			"texIndex = inTexIndex;\n"
			"gl_Position = uMVP * vec4(position, 1.0);\n"
			"}\0";

		const char* fragmentProgram = "#version 400 core\n"
			"out vec4 FragColor;\n"
			"in vec4 color;\n"
			"in vec2 texCoord;\n"
			"in float texIndex;\n"
#ifdef __APPLE__
			"uniform sampler2D uTextures[13];\n"
#else
			"uniform sampler2D uTextures[32];\n"
#endif
			"void main() {\n"
			"int index = int(texIndex);\n"
			"FragColor = (texture(uTextures[index], texCoord) * color);\n"
			"}\n\0";

		const char* instancedShaderProgram = "#version 400 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec4 inColor;\n"
			"layout(location = 2) in vec2 inTexCoord;\n"
			"layout(location = 3) in float inTexIndex;\n"
			"out vec4 color;\n"
			"out vec2 texCoord;\n"
			"out float texIndex;\n"
			"uniform mat4 uMVP;\n"
			"uniform vec4 uInstancedOffsets[400];\n"
			"void main() {\n"
			"color = inColor;\n"
			"texCoord = inTexCoord;\n"
			"texIndex = inTexIndex;\n"
			"vec3 offset = vec3(uInstancedOffsets[gl_InstanceID].x, uInstancedOffsets[gl_InstanceID].y, uInstancedOffsets[gl_InstanceID].z);\n"
			"gl_Position = uMVP * vec4(position.x + offset.x, position.y + offset.y, position.z + offset.z, 1.0);\n"
			"}\0";

		const char* instancedFragmentProgram = "#version 400 core\n"
			"out vec4 FragColor;\n"
			"in vec4 color;\n"
			"in vec2 texCoord;\n"
			"in float texIndex;\n"
#ifdef __APPLE__
			"uniform sampler2D uTextures[13];\n"
#else
			"uniform sampler2D uTextures[32];\n"
#endif
			"void main() {\n"
			"int index = int(texIndex);\n"
			"FragColor = (texture(uTextures[index], texCoord) * color);\n"
			"}\n\0";
#else
#ifndef __PEN_IOS__
		const char* shaderProgram =
			"attribute vec3 position;\n"
			"attribute vec4 inColor;\n"
			"attribute vec2 inTexCoord;\n"
			"attribute float inTexIndex;\n"
			"varying vec4 color;\n"
			"varying vec2 texCoord;\n"
			"varying float texIndex;\n"
			"uniform mat4 uMVP;\n"
			"void main() {\n"
			"color = inColor;\n"
			"texCoord = inTexCoord;\n"
			"texIndex = inTexIndex;\n"
			"gl_Position = uMVP * vec4(position, 1.0);\n"
			"}\0";

		/*Default precision definition is required for OpenGL ES*/
		const char* fragmentProgram =
			"precision mediump float;\n"
			"varying vec4 color;\n"
			"varying vec2 texCoord;\n"
			"varying float texIndex;\n"
			"uniform sampler2D uTextures[32];\n"
			"void main() {\n"
			"int index = int(texIndex);\n"
			"gl_FragColor = (texture2D(uTextures[index], texCoord) * color);\n"
			"}\n\0";

		const char* instancedShaderProgram =
			"attribute vec3 position;\n"
			"attribute vec4 inColor;\n"
			"attribute vec2 inTexCoord;\n"
			"attribute float inTexIndex;\n"
			"varying vec4 color;\n"
			"varying vec2 texCoord;\n"
			"varying float texIndex;\n"
			"uniform mat4 uMVP;\n"
			"uniform sampler2D uTextures[32];\n"
			"uniform vec4 uInstancedOffsets[400];\n"
			"void main() {\n"
			"color = inColor;\n"
			"texCoord = inTexCoord;\n"
			"texIndex = inTexIndex;\n"
			"vec3 offset = vec3(uInstancedOffsets[gl_InstanceID].x, uInstancedOffsets[gl_InstanceID].y, uInstancedOffsets[gl_InstanceID].z);\n"
			"gl_Position = uMVP * vec4(position.x + offset.x, position.y + offset.y, position.z + offset.z, 1.0);\n"
			"}\0";

		/*Default precision definition is required for OpenGL ES*/
		const char* instancedFragmentProgram =
			"precision mediump float;\n"
			"varying vec4 color;\n"
			"varying vec2 texCoord;\n"
			"varying float texIndex;\n"
			"void main() {\n"
			"int index = int(texIndex);\n"
			"gl_FragColor = (texture2D(uTextures[index], texCoord) * color);;\n"
			"}\n\0";
#else 
		/*The shader and fragment programs are combined here for Metal*/
		const char* shaderProgram = R"(
				#include <metal_stdlib>
				using namespace metal;

				struct VertexData
				{
					float4 position [[position]];
					half4 color;
					float2 texCoord [[texCoord]];
					float texIndex;
				};

				VertexData vertex vertexMain( uint vertexId [[vertex_id]],
									   device const float3* positions [[buffer(0)]],
									   device const float3* colors [[buffer(1)]],
									   device const float2* texCoords [[buffer(2)]],
									   device const float* texIndices [[buffer(3)]]
										)
				{
					VertexData out;
					out.position = float4( positions[ vertexId ], 1.0 );
					out.color = colors[ vertexId ];
					out.texCoord = texCoords[ vertexId ];
					out.texIndex = texIndices[ vertexId ];
					return out;
				}

				half4 fragment fragmentMain( VertexData in [[stage_in]] )
				{
					return in.color;
				}
			)";

		const char* instancedShaderProgram = R"(
				#include <metal_stdlib>
				using namespace metal;

				struct VertexData
				{
					float4 position [[position]];
					half4 color;
					float2 texCoord [[texCoord]];
					float texIndex;
				};

				uniform uInstancedOffsets[400];

				VertexData vertex vertexMain( uint vertexId [[vertex_id]],
									   device const float3* positions [[buffer(0)]],
									   device const float3* colors [[buffer(1)]],
									   device const float2* texCoords [[buffer(2)]],
									   device const float* texIndices [[buffer(3)]]
										)
				{
					VertexData out;
					float3 offset = vec3(uInstancedOffsets[gl_InstanceID].x, uInstancedOffsets[gl_InstanceID].y, uInstancedOffsets[gl_InstanceID].z);
					out.position = float4( positions[ vertexId ].x + offset.x, positions[ vertexId ].y + offset.y, positions[ vertexId ].z + offset.z, 1.0 );
					out.color = colors[ vertexId ];
					out.texCoord = texCoords[ vertexId ];
					out.texIndex = texIndices[ vertexId ];
					return out;
				}

				half4 fragment fragmentMain( VertexData in [[stage_in]] )
				{
					return in.color;
				}
			)";
#endif /*__PEN_IOS__*/
#endif /*__PEN_ES__*/


		Shader();
		Shader(const int& init);
		~Shader();

		void Bind() const;
		void Unbind() const;
		void Destroy();
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1iv(const std::string& name, const unsigned int count, int* value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform2f(const std::string& name, pen::Vec2* value);
		void SetUniform3f(const std::string& name, pen::Vec3* value);
		void SetUniform4f(const std::string& name, pen::Vec4* value);
		void SetUniformMat4x4f(const std::string& name, const pen::Mat4x4& matrix);
		void SetUniformMat2x4f(const std::string& name, pen::Mat2x4* matrix);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		GLint GetUniformLocation(const std::string& name);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	};
}