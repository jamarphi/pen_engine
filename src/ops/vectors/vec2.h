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
#include <iostream>
#include <cstdio>
#include "../operations/trig.h"

namespace pen {
	class Vec2 {
	public:
		float x, y;
		Vec2() = default;
		Vec2(float objX, float objY);
		~Vec2();
		Vec2 Normalize();

		bool operator==(Vec2& vecB);
		Vec2 operator+(Vec2& vecB);
		Vec2 operator-(Vec2& vecB);
		Vec2 operator*(Vec2& vecB);
		Vec2 operator/(Vec2& vecB);
		void operator+=(Vec2& vecB);
		void operator-=(Vec2& vecB);
		void operator*=(Vec2& vecB);
		void operator/=(Vec2& vecB);
	};
}