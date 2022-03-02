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
#include "vec3.h"

namespace pen {
	Vec3::Vec3(float objX, float objY, float objZ) : x(objX), y(objY), z(objZ) {}

	Vec3::~Vec3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vec3 Vec3::Normalize() {
		float mag = pen::op::Sqrt(x * x + y * y + z * z);
		float factor = 1.0f / mag;
		return pen::Vec3(factor * x, factor * y, factor * z);
	}

	bool Vec3::operator==(Vec3& vecB) {
		return (x == vecB.x && y == vecB.y && z == vecB.z);
	}

	Vec3 Vec3::operator+(Vec3& vecB) {
		return Vec3(x + vecB.x, y + vecB.y, z + vecB.z);
	}

	Vec3 Vec3::operator+(Vec3&& vecB) {
		return Vec3(x + vecB.x, y + vecB.y, z + vecB.z);
	}

	Vec3 Vec3::operator+(float& val) {
		return Vec3(x + val, y + val, z + val);
	}

	Vec3 Vec3::operator+(float&& val) {
		return Vec3(x + val, y + val, z + val);
	}

	Vec3 Vec3::operator-(Vec3& vecB) {
		return Vec3(x - vecB.x, y - vecB.y, z - vecB.z);
	}

	Vec3 Vec3::operator-(Vec3&& vecB) {
		return Vec3(x - vecB.x, y - vecB.y, z - vecB.z);
	}

	Vec3 Vec3::operator-(float& val) {
		return Vec3(x - val, y - val, z - val);
	}

	Vec3 Vec3::operator-(float&& val) {
		return Vec3(x - val, y - val, z - val);
	}

	Vec3 Vec3::operator*(Vec3& vecB) {
		return Vec3(x * vecB.x, y * vecB.y, z * vecB.z);
	}

	Vec3 Vec3::operator*(Vec3&& vecB) {
		return Vec3(x * vecB.x, y * vecB.y, z * vecB.z);
	}

	Vec3 Vec3::operator*(float& val) {
		return Vec3(x * val, y * val, z * val);
	}

	Vec3 Vec3::operator*(float&& val) {
		return Vec3(x * val, y * val, z * val);
	}

	Vec3 Vec3::operator/(Vec3& vecB) {
		return Vec3(x / vecB.x, y / vecB.y, z / vecB.z);
	}

	Vec3 Vec3::operator/(Vec3&& vecB) {
		return Vec3(x / vecB.x, y / vecB.y, z / vecB.z);
	}

	Vec3 Vec3::operator/(float& val) {
		return Vec3(x / val, y / val, z / val);
	}

	Vec3 Vec3::operator/(float&& val) {
		return Vec3(x / val, y / val, z / val);
	}

	void Vec3::operator+=(Vec3& vecB) {
		x += vecB.x;
		y += vecB.y; 
		z += vecB.z;
	}

	void Vec3::operator+=(Vec3&& vecB) {
		x += vecB.x;
		y += vecB.y;
		z += vecB.z;
	}

	void Vec3::operator+=(float& val) {
		x += val;
		y += val;
		z += val;
	}

	void Vec3::operator+=(float&& val) {
		x += val;
		y += val;
		z += val;
	}

	void Vec3::operator-=(Vec3& vecB) {
		x -= vecB.x;
		y -= vecB.y; 
		z -= vecB.z;
	}

	void Vec3::operator-=(Vec3&& vecB) {
		x -= vecB.x;
		y -= vecB.y;
		z -= vecB.z;
	}

	void Vec3::operator-=(float& val) {
		x -= val;
		y -= val;
		z -= val;
	}

	void Vec3::operator-=(float&& val) {
		x -= val;
		y -= val;
		z -= val;
	}

	void Vec3::operator*=(Vec3& vecB) {
		x *= vecB.x;
		y *= vecB.y;
		z *= vecB.z;
	}

	void Vec3::operator*=(Vec3&& vecB) {
		x *= vecB.x;
		y *= vecB.y;
		z *= vecB.z;
	}

	void Vec3::operator*=(float& val) {
		x *= val;
		y *= val;
		z *= val;
	}

	void Vec3::operator*=(float&& val) {
		x *= val;
		y *= val;
		z *= val;
	}

	void Vec3::operator/=(Vec3& vecB) {
		x /= vecB.x;
		y /= vecB.y;
		z /= vecB.z;
	}

	void Vec3::operator/=(Vec3&& vecB) {
		x /= vecB.x;
		y /= vecB.y;
		z /= vecB.z;
	}

	void Vec3::operator/=(float& val) {
		x /= val;
		y /= val;
		z /= val;
	}

	void Vec3::operator/=(float&& val) {
		x /= val;
		y /= val;
		z /= val;
	}
}