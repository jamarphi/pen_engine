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
#include "shader.h"

namespace pen {
	Shader::Shader() {
		/*The default shader constructor, this one is mainly for declaring Shader variables as member variables of another class*/
	}

	Shader::Shader(const int& init) : rendererId(0) {
		/*This constructor gets called when a shader will actually be created*/
		switch (init) {
		case 1:
			rendererId = CreateShader(shaderProgram, fragmentProgram);
			break;
		case 2:
			rendererId = CreateShader(instancedShaderProgram, instancedFragmentProgram);
			break;
		default:
			rendererId = CreateShader(shaderProgram, fragmentProgram);
			break;
		}
	}

	Shader::~Shader() {
	}

	void Shader::Destroy() {
		/*Removes the shader program from memory on the GPU*/
#ifndef __PEN_IOS__
		glDeleteProgram(rendererId);
#endif
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
		/*Creates a shader and links to program in context*/
#ifndef __PEN_IOS__
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
			std::cout << message << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
#else
		return 0;
#endif
	}

	void Shader::Bind() const {
		/*Binds the shader to be used for drawing*/
#ifndef __PEN_IOS__
		glUseProgram(rendererId);
#endif
	}

	void Shader::Unbind() const {
		/*Unbinds the shader*/
#ifndef __PEN_IOS__
		glUseProgram(0);
#endif
	}

	unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
		/*Creates the vertex and fragment shader for rendering*/
#ifndef __PEN_IOS__
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);
		return program;
#else
		using NS::StringEncoding::UTF8StringEncoding;

		NS::Error* pError = nullptr;
		MTL::Library* pLibrary = _pDevice->newLibrary(NS::String::string(shaderProgram, UTF8StringEncoding), nullptr, &pError);
		if (!pLibrary)
		{
			__builtin_printf("%s", pError->localizedDescription()->utf8String());
			assert(false);
		}

		MTL::Function* pVertexFn = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
		MTL::Function* pFragFn = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

		MTL::ArgumentEncoder* pArgEncoder = pVertexFn->newArgumentEncoder(0);
		pen::State::Get()->iosArgEncoder = pArgEncoder;

		MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
		pDesc->setVertexFunction(pVertexFn);
		pDesc->setFragmentFunction(pFragFn);
		pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

		_pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
		if (!_pPSO)
		{
			__builtin_printf("%s", pError->localizedDescription()->utf8String());
			assert(false);
		}

		pVertexFn->release();
		pFragFn->release();
		pDesc->release();
		pLibrary->release();
#endif
	}

	void Shader::SetUniform1i(const std::string& name, int value) {
		/*Sets an integer uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
#endif
	}

	void Shader::SetUniform1iv(const std::string& name, const unsigned int count, int* value) {
		/*Sets an integer array uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, value);
#endif
	}

	void Shader::SetUniform1f(const std::string& name, float value) {
		/*Sets a float uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
#endif
	}

	void Shader::SetUniform2f(const std::string& name, pen::Vec2* value) {
		/*Sets a vec2 float uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value->x, value->y);
#endif
	}

	void Shader::SetUniform3f(const std::string& name, pen::Vec3* value) {
		/*Sets a vec3 float uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value->x, value->y, value->z);
#endif
	}

	void Shader::SetUniform4f(const std::string& name, pen::Vec4* value) {
		/*Sets a vec4 float uniform in the shader*/
#ifndef __PEN_IOS__
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value->x, value->y, value->z, value->w);
#endif
	}

	void Shader::SetUniformMat4x4f(const std::string& name, const pen::Mat4x4& matrix) {
		/*Sets a mat4x4 uniform in the shader*/
#ifndef __PEN_IOS__
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix.matrix[0][0]);
#endif
	}

	void Shader::SetUniformMat2x4f(const std::string& name, pen::Mat2x4* matrix) {
		/*Sets a mat2x4 uniform in the shader*/
#ifndef __PEN_IOS__
		glUniformMatrix2x4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix->matrix[0][0]);
#endif
	}

	GLint Shader::GetUniformLocation(const std::string& name) {
		/*Looks for the uniform name and caches it if not found previously*/
#ifndef __PEN_IOS__
		if (uniformLocationCache.Find(name) != nullptr)
			return uniformLocationCache.Find(name)->second;

		GLint location = glGetUniformLocation(rendererId, name.c_str());
		if (location == -1) {
			std::cout << "Uniform '" << name << "' does not exist" << std::endl;
		}

		uniformLocationCache.Insert(name, location);

		return location;
#else
		return 0;
#endif
	}
}