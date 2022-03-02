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
#include "mat2x4.h"

namespace pen {
	Mat2x4::Mat2x4(const float&& val) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 4; j++) {
				matrix[i][j] = val;
			}
		}
	}

	Mat2x4::~Mat2x4() {}

	float& Mat2x4::operator[](int val) {
		return matrix[val][val];
	}

	Mat2x4 Mat2x4::operator*(const Mat2x4& b) {
		Mat2x4 mat = Mat2x4(1.0f);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 4; j++) {
				mat.matrix[i][j] = matrix[i][j] *= b.matrix[i][j];
			}
		}

		return mat;
	}
}