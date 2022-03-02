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
#include "mat3x3.h"

namespace pen {
	Mat3x3::Mat3x3(const float&& val, bool identity) {
		if (identity) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					matrix[i][j] = 0.0f;
				}
			}

			matrix[0][0] = 1.0f;
			matrix[1][1] = 1.0f;
			matrix[2][2] = 1.0f;
		}
		else {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					matrix[i][j] = val;
				}
			}
		}
	}

	Mat3x3::~Mat3x3() {}

	float& Mat3x3::operator[](int val) {
		return matrix[val][val];
	}

	Mat3x3 Mat3x3::operator*(const Mat3x3& b) {
		Mat3x3 mat = Mat3x3(1.0f, false);

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				mat.matrix[i][j] = matrix[i][j] *= b.matrix[i][j];
			}
		}

		return mat;
	}
}