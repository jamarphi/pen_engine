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
#include "../matrices/mat4x4.h"
#include "../matrices/mat3x3.h"
#include "../vectors/vec3.h"
#include "../vectors/vec4.h"
#include "trig.h"

namespace pen {
	namespace op {
		static float Floor(float decimal) {
			/*Round a decimal down to a whole number*/
			return (float)((int)decimal);
		}

		static float Ceil(float decimal) {
			/*Round a decimal up to a whole number*/
			return (float)((int)decimal + 1.0f);
		}

		static void Translate(pen::Mat4x4* matrix, const pen::Vec3& vec) {
			/*Shift the matrix*/
			matrix->matrix[3][0] = vec.x;
			matrix->matrix[3][1] = vec.y;
			matrix->matrix[3][2] = vec.z;
		}

		static pen::Mat4x4 Ortho(const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar) {
			/*Scale the points from screen space to normalized space*/
			pen::Mat4x4 result = pen::Mat4x4(1.0f);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					result.matrix[i][j] = 0.0f;
				}
			}

			result.matrix[0][0] = 1.0f;
			result.matrix[1][1] = 1.0f;
			result.matrix[2][2] = 1.0f;
			result.matrix[3][3] = 1.0f;

			result.matrix[0][0] = 2.0f / (right - left);
			result.matrix[1][1] = 2.0f / (top - bottom);
			result.matrix[2][2] = -2.0f / (zFar - zNear);
			result.matrix[3][0] = -(right + left) / (right - left);
			result.matrix[3][1] = -(top + bottom) / (top - bottom);
			result.matrix[3][2] = -(zFar + zNear) / (zFar - zNear);
			return result;
		}

		static pen::Mat4x4 Perspective(const int& fov, const float& aspectRatio, const float& zNear, const float& zFar) {
			/*Gives a perspective projection matrix for 3d*/
			float radFov = fov * 3.14159f / 180.0f;
			float tanHalfFovy = pen::op::Tan(radFov / 2.0f);

			pen::Mat4x4 result = pen::Mat4x4(0.0f);
			/*----Normal vector offset mode----*/
			//result.matrix[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
			//result.matrix[1][1] = 1.0f / tanHalfFovy;
			//result.matrix[2][2] = -(zFar + zNear) / (zFar - zNear);
			//result.matrix[2][3] = -1.0f;
			//result.matrix[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
			/*----Normal vector offset mode----*/

			/*----Z offset mode----*/
			result.matrix[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
			result.matrix[1][1] = 1.0f / (tanHalfFovy);
			result.matrix[2][2] = zFar / (zNear - zFar);
			result.matrix[2][3] = -1.0f;
			result.matrix[3][2] = -(zFar * zNear) / (zFar - zNear);

			/*Added constants here for translation to work and to render*/
			result.matrix[3][0] = 1.0f;
			result.matrix[3][1] = 1.0f;
			result.matrix[3][3] = 1.0f;
			/*----Z offset mode----*/

			return result;
		}

		static pen::Vec3 CrossProduct(pen::Vec3 a, pen::Vec3 b) {
			/*Cross product for two given vectors*/
			return pen::Vec3( a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y);
		}

		static float DotProduct(pen::Vec3 a, pen::Vec3 b) {
			/*Dot product for two given vectors*/
			return (a.x * b.x + a.y * b.y + a.z * b.z);
		}

		static float DotProductVec4(pen::Vec4 a, pen::Vec4 b) {
			/*Dot product for two given vectors*/
			return (a.x * b.x + a.y * b.y + a.z * b.z);
		}

		static pen::Mat4x4 Look(pen::Vec3 viewOrigin, pen::Vec3 center, pen::Vec3 at) {
			/*Aim at a given direction*/
			pen::Vec3 f = (center - viewOrigin).Normalize();
			pen::Vec3 s = CrossProduct(f, at).Normalize();
			pen::Vec3 u = CrossProduct(s, f);

			pen::Mat4x4 result = pen::Mat4x4(0.0f);
			result.matrix[0][0] = 1.0f;
			result.matrix[1][1] = 1.0f;
			result.matrix[2][2] = 1.0f;
			result.matrix[3][3] = 1.0f;


			result.matrix[0][0] = s.x;
			result.matrix[1][0] = s.y;
			result.matrix[2][0] = s.z;
			result.matrix[0][1] = u.x;
			result.matrix[1][1] = u.y;
			result.matrix[2][1] = u.z;
			result.matrix[0][2] = -f.x;
			result.matrix[1][2] = -f.y;
			result.matrix[2][2] = -f.z;
			result.matrix[3][0] = -DotProduct(s, viewOrigin);
			result.matrix[3][1] = -DotProduct(u, viewOrigin);
			result.matrix[3][2] = DotProduct(f, viewOrigin);
			return result;
		}

		static pen::Vec3 Mat4x4MultVec3(pen::Mat4x4 mat, pen::Vec3 vec, bool rotation) {
			/*Multiply a 4 x 4 matrix with a 3 x 3 vector*/
			pen::Vec3 result = pen::Vec3(0.0f, 0.0f, 0.0f);
			float w = 0.0f;
			if (!rotation) w = 1.0f;

			result.x = (mat.matrix[0][0] * vec.x) + (mat.matrix[1][0] * vec.y) + (mat.matrix[2][0] * vec.z) + (mat.matrix[3][0] * w);
			result.y = (mat.matrix[0][1] * vec.x) + (mat.matrix[1][1] * vec.y) + (mat.matrix[2][1] * vec.z) + (mat.matrix[3][1] * w);
			result.z = (mat.matrix[0][2] * vec.x) + (mat.matrix[1][2] * vec.y) + (mat.matrix[2][2] * vec.z) + (mat.matrix[3][2] * w);

			return result;
		}

		static pen::Vec4 Mat4x4MultVec4(pen::Mat4x4 mat, pen::Vec4 vec, bool rotation) {
			/*Multiply a 4 x 4 matrix with a 4 x 4 vector*/
			pen::Vec4 result = pen::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
			if (!rotation) vec.w = 1.0f;

			result.x = (mat.matrix[0][0] * vec.x) + (mat.matrix[1][0] * vec.y) + (mat.matrix[2][0] * vec.z) + (mat.matrix[3][0] * vec.w);
			result.y = (mat.matrix[0][1] * vec.x) + (mat.matrix[1][1] * vec.y) + (mat.matrix[2][1] * vec.z) + (mat.matrix[3][1] * vec.w);
			result.z = (mat.matrix[0][2] * vec.x) + (mat.matrix[1][2] * vec.y) + (mat.matrix[2][2] * vec.z) + (mat.matrix[3][2] * vec.w);
			result.w = (mat.matrix[0][3] * vec.x) + (mat.matrix[1][3] * vec.y) + (mat.matrix[2][3] * vec.z) + (mat.matrix[3][3] * vec.w);

			return result;
		}

		static pen::Mat4x4 RotateMatrix(const pen::Mat4x4& mat, int angle, pen::Vec3 vec) {
			/*Return a rotated matrix*/
			pen::Mat4x4 rotateMatrix;
			pen::Mat4x4 result;
			float a = angle * 3.14159f / 180.0f;
			float c = pen::op::Cos(a);
			float s = pen::op::Sin(a);

			pen::Vec3 axis = vec.Normalize();
			pen::Vec3 temp = (axis * (1.0f - c));

			pen::Vec4 vec1;
			pen::Vec4 vec2;
			pen::Vec4 vec3;
			pen::Vec4 resultingVec;
			
			rotateMatrix.matrix[0][0] = temp.x * axis.x + c;
			rotateMatrix.matrix[0][1] = temp.x * axis.y + axis.z * s;
			rotateMatrix.matrix[0][2] = temp.x * axis.z - axis.y * s;

			rotateMatrix.matrix[1][0] = temp.y * axis.x - axis.z * s;
			rotateMatrix.matrix[1][1] = temp.y * axis.y + c;
			rotateMatrix.matrix[1][2] = temp.y * axis.z + axis.x * s;

			rotateMatrix.matrix[2][0] = temp.z * axis.x + axis.y * s;
			rotateMatrix.matrix[2][1] = temp.z * axis.y - axis.x * s;
			rotateMatrix.matrix[2][2] = temp.z * axis.z + c;

			/*----First row----*/
			vec1.x = mat.matrix[0][0] * rotateMatrix.matrix[0][0];
			vec1.y = mat.matrix[0][1] * rotateMatrix.matrix[0][0];
			vec1.z = mat.matrix[0][2] * rotateMatrix.matrix[0][0];
			vec1.w = mat.matrix[0][3] * rotateMatrix.matrix[0][0];

			vec2.x = mat.matrix[1][0] * rotateMatrix.matrix[0][1];
			vec2.y = mat.matrix[1][1] * rotateMatrix.matrix[0][1];
			vec2.z = mat.matrix[1][2] * rotateMatrix.matrix[0][1];
			vec2.w = mat.matrix[1][3] * rotateMatrix.matrix[0][1];

			vec3.x = mat.matrix[2][0] * rotateMatrix.matrix[0][2];
			vec3.y = mat.matrix[2][1] * rotateMatrix.matrix[0][2];
			vec3.z = mat.matrix[2][2] * rotateMatrix.matrix[0][2];
			vec3.w = mat.matrix[2][3] * rotateMatrix.matrix[0][2];
			
			resultingVec = vec1 + vec2 + vec3;
			result.matrix[0][0] = resultingVec.x;
			result.matrix[0][1] = resultingVec.y;
			result.matrix[0][2] = resultingVec.z;
			result.matrix[0][3] = resultingVec.w;
			/*----First row----*/

			/*----Second row----*/
			vec1.x = mat.matrix[0][0] * rotateMatrix.matrix[1][0];
			vec1.y = mat.matrix[0][1] * rotateMatrix.matrix[1][0];
			vec1.z = mat.matrix[0][2] * rotateMatrix.matrix[1][0];
			vec1.w = mat.matrix[0][3] * rotateMatrix.matrix[1][0];
															
			vec2.x = mat.matrix[1][0] * rotateMatrix.matrix[1][1];
			vec2.y = mat.matrix[1][1] * rotateMatrix.matrix[1][1];
			vec2.z = mat.matrix[1][2] * rotateMatrix.matrix[1][1];
			vec2.w = mat.matrix[1][3] * rotateMatrix.matrix[1][1];
															
			vec3.x = mat.matrix[2][0] * rotateMatrix.matrix[1][2];
			vec3.y = mat.matrix[2][1] * rotateMatrix.matrix[1][2];
			vec3.z = mat.matrix[2][2] * rotateMatrix.matrix[1][2];
			vec3.w = mat.matrix[2][3] * rotateMatrix.matrix[1][2];

			resultingVec = vec1 + vec2 + vec3;
			result.matrix[1][0] = resultingVec.x;
			result.matrix[1][1] = resultingVec.y;
			result.matrix[1][2] = resultingVec.z;
			result.matrix[1][3] = resultingVec.w;
			/*----Second row----*/

			/*----Third row----*/
			vec1.x = mat.matrix[0][0] * rotateMatrix.matrix[2][0];
			vec1.y = mat.matrix[0][1] * rotateMatrix.matrix[2][0];
			vec1.z = mat.matrix[0][2] * rotateMatrix.matrix[2][0];
			vec1.w = mat.matrix[0][3] * rotateMatrix.matrix[2][0];
															
			vec2.x = mat.matrix[1][0] * rotateMatrix.matrix[2][1];
			vec2.y = mat.matrix[1][1] * rotateMatrix.matrix[2][1];
			vec2.z = mat.matrix[1][2] * rotateMatrix.matrix[2][1];
			vec2.w = mat.matrix[1][3] * rotateMatrix.matrix[2][1];
															
			vec3.x = mat.matrix[2][0] * rotateMatrix.matrix[2][2];
			vec3.y = mat.matrix[2][1] * rotateMatrix.matrix[2][2];
			vec3.z = mat.matrix[2][2] * rotateMatrix.matrix[2][2];
			vec3.w = mat.matrix[2][3] * rotateMatrix.matrix[2][2];

			resultingVec = vec1 + vec2 + vec3;
			result.matrix[2][0] = resultingVec.x;
			result.matrix[2][1] = resultingVec.y;
			result.matrix[2][2] = resultingVec.z;
			result.matrix[2][3] = resultingVec.w;
			/*----Third row----*/

			/*----Fourth row----*/
			result.matrix[3][0] = mat.matrix[3][0];
			result.matrix[3][1] = mat.matrix[3][1];
			result.matrix[3][2] = mat.matrix[3][2];
			result.matrix[3][3] = mat.matrix[3][3];
			/*----Fourth row----*/
			
			return result;
		}

		static pen::Vec3 RotateVec(const pen::Vec3& initVec, int angle, pen::Vec3 axis) {
			/*Rotate a vector*/
			pen::Mat4x4 mat = pen::Mat4x4(0.0f);
			mat.matrix[0][0] = 1.0f;
			mat.matrix[1][1] = 1.0f;
			mat.matrix[2][2] = 1.0f;
			mat.matrix[3][3] = 1.0f;

			pen::Mat4x4 rotationMatrix = RotateMatrix(mat, angle, axis);

			return Mat4x4MultVec3(rotationMatrix, initVec, true);
		}

		static float Min(float a, float b) {
			/*Returns b if greater than a*/
			return (b < a) ? b : a;
		}

		static float Max(float a, float b) {
			/*Returns a if less than b*/
			return (a < b) ? b : a;
		}

		static float Clamp(float x, float min, float max) {
			/*Clamp the vector*/
			return Min(Max(x, min), max);
		}
		
		static float AngleBetween(const pen::Vec3& a, const pen::Vec3& b) {
			/*Returns angle between two vectors in radians*/
			return pen::op::ACos(Clamp(DotProduct(a, b), -1.0f, 1.0f));
		}

		static void Invert(const pen::Mat3x3& mat, pen::Mat3x3& invMat, bool& invert) {
			/*Get the 3x3 inversion matrix for a given 3x3 matrix*/
			if (invert)
			{
				float det = mat.matrix[0][0] * (mat.matrix[1][1] * mat.matrix[2][2] - mat.matrix[1][2] * mat.matrix[2][1]) -
					mat.matrix[1][0] * (mat.matrix[0][1] * mat.matrix[2][2] - mat.matrix[2][1] * mat.matrix[0][2]) +
					mat.matrix[2][0] * (mat.matrix[0][1] * mat.matrix[1][2] - mat.matrix[1][1] * mat.matrix[0][2]);

				float idet = 1.0f / det;
				invMat.matrix[0][0] = (mat.matrix[1][1] * mat.matrix[2][2] - mat.matrix[1][2] * mat.matrix[2][1]) * idet;
				invMat.matrix[1][0] = (mat.matrix[2][0] * mat.matrix[1][2] - mat.matrix[1][0] * mat.matrix[2][2]) * idet;
				invMat.matrix[2][0] = (mat.matrix[1][0] * mat.matrix[2][1] - mat.matrix[2][0] * mat.matrix[1][1]) * idet;
				invMat.matrix[0][1] = (mat.matrix[2][1] * mat.matrix[0][2] - mat.matrix[0][1] * mat.matrix[2][2]) * idet;
				invMat.matrix[1][1] = (mat.matrix[0][0] * mat.matrix[2][2] - mat.matrix[2][0] * mat.matrix[0][2]) * idet;
				invMat.matrix[2][1] = (mat.matrix[0][1] * mat.matrix[2][0] - mat.matrix[0][0] * mat.matrix[2][1]) * idet;
				invMat.matrix[0][2] = (mat.matrix[0][1] * mat.matrix[1][2] - mat.matrix[0][2] * mat.matrix[1][1]) * idet;
				invMat.matrix[1][2] = (mat.matrix[0][2] * mat.matrix[1][0] - mat.matrix[0][0] * mat.matrix[1][2]) * idet;
				invMat.matrix[2][2] = (mat.matrix[0][0] * mat.matrix[1][1] - mat.matrix[0][1] * mat.matrix[1][0]) * idet;
				invert = false;
			}
		}
	}
}