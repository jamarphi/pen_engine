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
#include "vec4.h"

namespace pen {
	Vec4::Vec4(float objX, float objY, float objZ, float objW) : x(objX), y(objY), z(objZ), w(objW) {}

	Vec4::~Vec4() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	Vec4 Vec4::Normalize() {
		float mag = pen::op::Sqrt(x * x + y * y + z * z + w * w);
		float factor = 1.0f / mag;
		return pen::Vec4(factor * x, factor * y, factor * z, factor * w);
	}

	bool Vec4::operator==(Vec4& vecB) {
		return (x == vecB.x && y == vecB.y && z == vecB.z && w == vecB.w);
	}

	Vec4 Vec4::operator+(Vec4& vecB) {
		return Vec4(x + vecB.x, y + vecB.y, z + vecB.z, w + vecB.w);
	}

	Vec4 Vec4::operator+(Vec4&& vecB) {
		return Vec4(x + vecB.x, y + vecB.y, z + vecB.z, w + vecB.w);
	}

	Vec4 Vec4::operator+(float& val) {
		return Vec4(x + val, y + val, z + val, w + val);
	}

	Vec4 Vec4::operator+(float&& val) {
		return Vec4(x + val, y + val, z + val, w + val);
	}

	Vec4 Vec4::operator-(Vec4& vecB) {
		return Vec4(x - vecB.x, y - vecB.y, z - vecB.z, w - vecB.w);
	}

	Vec4 Vec4::operator-(float& val) {
		return Vec4(x - val, y - val, z - val, w - val);
	}

	Vec4 Vec4::operator-(float&& val) {
		return Vec4(x - val, y - val, z - val, w - val);
	}

	Vec4 Vec4::operator*(Vec4& vecB) {
		return Vec4(x * vecB.x, y * vecB.y, z * vecB.z, w * vecB.w);
	}

	Vec4 Vec4::operator*(float& val) {
		return Vec4(x * val, y * val, z * val, w * val);
	}

	Vec4 Vec4::operator*(float&& val) {
		return Vec4(x * val, y * val, z * val, w * val);
	}

	Vec4 Vec4::operator/(Vec4& vecB) {
		return Vec4(x / vecB.x, y / vecB.y, z / vecB.z, w / vecB.w);
	}

	Vec4 Vec4::operator/(float& val) {
		return Vec4(x / val, y / val, z / val, w / val);
	}

	Vec4 Vec4::operator/(float&& val) {
		return Vec4(x / val, y / val, z / val, w / val);
	}

	void Vec4::operator+=(Vec4& vecB) {
		x += vecB.x;
		y += vecB.y;
		z += vecB.z;
		w += vecB.w;
	}

	void Vec4::operator+=(float& val) {
		x += val;
		y += val;
		z += val;
		w += val;
	}

	void Vec4::operator+=(float&& val) {
		x += val;
		y += val;
		z += val;
		w += val;
	}

	void Vec4::operator-=(Vec4& vecB) {
		x -= vecB.x;
		y -= vecB.y;
		z -= vecB.z;
		w -= vecB.w;
	}

	void Vec4::operator-=(float& val) {
		x -= val;
		y -= val;
		z -= val;
		w -= val;
	}

	void Vec4::operator-=(float&& val) {
		x -= val;
		y -= val;
		z -= val;
		w -= val;
	}

	void Vec4::operator*=(Vec4& vecB) {
		x *= vecB.x;
		y *= vecB.y;
		z *= vecB.z;
		w *= vecB.w;
	}

	void Vec4::operator*=(float& val) {
		x *= val;
		y *= val;
		z *= val;
		w *= val;
	}

	void Vec4::operator*=(float&& val) {
		x *= val;
		y *= val;
		z *= val;
		w *= val;
	}

	void Vec4::operator/=(Vec4& vecB) {
		x /= vecB.x;
		y /= vecB.y;
		z /= vecB.z;
		w /= vecB.w;
	}

	void Vec4::operator/=(float& val) {
		x /= val;
		y /= val;
		z /= val;
		w /= val;
	}

	void Vec4::operator/=(float&& val) {
		x /= val;
		y /= val;
		z /= val;
		w /= val;
	}
}