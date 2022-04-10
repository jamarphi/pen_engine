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
#include "../../dependencies/glad/glad.h"
#include "../ops/vectors/vec2.h"
#include "../ops/vectors/vec3.h"
#include "../ops/vectors/vec4.h"
#include "../ops/matrices/mat4x4.h"
#include "../ops/matrices/mat2x4.h"
#include "../objects/containers/map.h"

class Shader {
public:
	unsigned int textureUnits;
	std::string filePath;
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
		"uniform mat2x4 instancedOffsets[400];\n"
		"void main() {\n"
		"color = vec4(instancedOffsets[gl_InstanceID][1]);\n"
		"texCoord = inTexCoord;\n"
		"texIndex = inTexIndex;\n"
		"vec2 offset = vec2(instancedOffsets[gl_InstanceID][0].x, instancedOffsets[gl_InstanceID][0].y);\n"
		"gl_Position = vec4(offset.x, offset.y, 1.0);\n"
		"}\0";

	const char* instancedFragmentProgram = "#version 400 core\n"
		"out vec4 FragColor;\n"
		"in vec4 color;\n"
		"in vec2 texCoord;\n"
		"in float texIndex;\n"
		"void main() {\n"
		"FragColor = color;\n"
		"}\n\0";
#else
	const char* shaderProgram =
		"attribute vec4 position;\n"
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
		"gl_Position = uMVP * position;\n"
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
		"attribute vec4 position;\n"
		"attribute vec4 inColor;\n"
		"attribute vec2 inTexCoord;\n"
		"attribute float inTexIndex;\n"
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		"varying float texIndex;\n"
		"uniform mat2x4 instancedOffsets[400];\n"
		"void main() {\n"
		"color = vec4(instancedOffsets[gl_InstanceID][1]);\n"
		"texCoord = inTexCoord;\n"
		"texIndex = inTexIndex;\n"
		"vec2 offset = vec2(instancedOffsets[gl_InstanceID][0].x, instancedOffsets[gl_InstanceID][0].y);\n"
		"gl_Position = vec4(offset.x, offset.y, 1.0);\n"
		"}\0";

	/*Default precision definition is required for OpenGL ES*/
	const char* instancedFragmentProgram =
		"precision mediump float;\n"
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		"varying float texIndex;\n"
		"void main() {\n"
		"gl_FragColor = color;\n"
		"}\n\0";
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
	void SetUniform2f(const std::string& name, const pen::Vec2& value);
	void SetUniform3f(const std::string& name, const pen::Vec3& value);
	void SetUniform4f(const std::string& name, const pen::Vec4& value);
	void SetUniformMat4f(const std::string& name, const pen::Mat4x4& matrix);
	void SetUniformMat2x4fv(const std::string& name, const unsigned int count, const pen::Mat2x4& matrix);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	GLint GetUniformLocation(const std::string& name);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};
