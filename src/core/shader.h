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
#ifndef __PEN_MAC_IOS__
#include "../../dependencies/glad/glad.h"
#endif
#include "../ops/vectors/vec2.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec4.h"
#include "../ops/matrices/mat4x4.h"
#include "../ops/matrices/mat2x4.h"
#include "../objects/containers/map.h"
#include "../state/state.h"
#include "../../ext/platforms/mac_ios/mac_ios_cpp_objective_c_mapping.h"

namespace pen {
	class Shader {
	public:
		unsigned int rendererId;		
#ifndef __PEN_MAC_IOS__
        pen::Map<std::string, GLint> uniformLocationCache;
#endif

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
			"uniform sampler2D uTextures[32];\n"
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
			"uniform sampler2D uTextures[32];\n"
			"void main() {\n"
			"int index = int(texIndex);\n"
			"FragColor = (texture(uTextures[index], texCoord) * color);\n"
			"}\n\0";
#else
#ifndef __PEN_MAC_IOS__
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

				struct v2f
				{
					float4 position [[position]];
					float4 color;
					float2 texCoord;
					float texId;
                    float layerId;
                    float isInstanced;
				};

				struct BatchVertexData
				{
					float3 position;
					float4 color;
					float2 texCoord;
					float texId;
                    float layerId;
                    float isInstanced;
				};

				struct IOSUniformData
				{
					float4x4 uMVP;
				};

				struct IOSInstanceData
				{
					float3 uInstancedOffsets;
				};

				v2f vertex vertexMain(
									   device const BatchVertexData* vertexData [[buffer(0)]],
									   device const IOSUniformData* uniformData [[buffer(1)]],
									   device const IOSInstanceData* instanceData [[buffer(2)]],
										uint vertexId [[vertex_id]],
										uint instanceId [[instance_id]]
										)
				{
					v2f out;
					const device BatchVertexData& vd = vertexData[ vertexId ];
					float4 pos = float4( vd.position.xyz, 1.0 ) * uniformData[(int)vd.layerId].uMVP;
                    if(vd.isInstanced > 0.0){
                        float3 offset = float3(instanceData[instanceId].uInstancedOffsets.x, instanceData[instanceId].uInstancedOffsets.y, instanceData[instanceId].uInstancedOffsets.z);
                        pos.x = pos.x + offset.x;
                        pos.y = pos.y + offset.y;
                        pos.z = pos.z + offset.z;
                    }

					out.position = pos;
					out.color = vd.color;
					out.texCoord = float2(vd.texCoord.x, -vd.texCoord.y);
					out.texId = vd.texId;
                    out.layerId = vd.layerId;
                    out.isInstanced = vd.isInstanced;
					return out;
				}

				float4 fragment fragmentMain( v2f in [[stage_in]], array<texture2d<half>, 8> tex [[texture(0)]] )
				{
					constexpr sampler s( address::repeat, filter::linear );
					half4 texel = tex[in.texId].sample( s, in.texCoord ).rgba;

					float4 outColor = in.color * (float4)texel;
					return outColor;
				}
			)";
#endif /*__PEN_MAC_IOS__*/
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
#ifndef __PEN_MAC_IOS__
		GLint GetUniformLocation(const std::string& name);
#endif
        
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	};
}
