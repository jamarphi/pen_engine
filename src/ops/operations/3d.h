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
#include "../../objects/item_3d.h"

namespace pen {
	namespace _3d {
		enum AXIS {
			X = 0,
			Y = 1,
			Z = 2
		};

		static void Scale(pen::Item3D* item, float sx, float sy, float sz) {
			/*Scale a 3d item*/
			if (sx != 0.0f && sy != 0.0f && sz != 0.0f) {
				pen::Mat4x4 scaleMat = pen::Mat4x4(1.0f, true);
				pen::Mat4x4 outputMat = pen::Mat4x4(1.0f, true);
				scaleMat.matrix[0][0] = sx;
				scaleMat.matrix[1][1] = sy;
				scaleMat.matrix[2][2] = sz;

				/*Multiplies scaling matrix with the item matrix*/
				for (int j = 0; j < 4; j++) {
					for (int i = 0; i < 4; i++) {
						outputMat.matrix[j][i] = scaleMat.matrix[0][i] * item->model.matrix[j][0] + scaleMat.matrix[1][i] * item->model.matrix[j][1]
							+ scaleMat.matrix[2][i] * item->model.matrix[j][2] + scaleMat.matrix[3][i] * item->model.matrix[j][3];
					}
				}
				item->model = outputMat;
			}
		}

		static void Rotate(pen::Item3D* item, float theta, int axis) {
			/*Rotate a 3d item*/
			pen::Mat4x4 rotationMat = pen::Mat4x4(1.0f, true);
			pen::Mat4x4 outputMat = pen::Mat4x4(1.0f, true);

			switch (axis) {
			case pen::_3d::AXIS::X:
				rotationMat.matrix[1][0] = pen::op::Cos(theta);
				rotationMat.matrix[2][0] = pen::op::Sin(theta);
				rotationMat.matrix[1][1] = -1.0f * pen::op::Sin(theta);
				rotationMat.matrix[2][2] = pen::op::Cos(theta);
				break;
			case pen::_3d::AXIS::Y:
				rotationMat.matrix[0][0] = pen::op::Cos(theta);
				rotationMat.matrix[2][0] = pen::op::Sin(theta);
				rotationMat.matrix[0][1] = -1.0f * pen::op::Sin(theta);
				rotationMat.matrix[2][2] = pen::op::Cos(theta);
				break;
			case pen::_3d::AXIS::Z:
				rotationMat.matrix[0][0] = pen::op::Cos(theta);
				rotationMat.matrix[1][0] = pen::op::Sin(theta);
				rotationMat.matrix[0][1] = -1.0f * pen::op::Sin(theta);
				rotationMat.matrix[1][1] = pen::op::Cos(theta);
				break;
			default:
				break;
			}

			/*Multiply rotation matrix with the item matrix*/
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					outputMat.matrix[j][i] = rotationMat.matrix[0][i] * item->model.matrix[j][0] + rotationMat.matrix[1][i] * item->model.matrix[j][1]
						+ rotationMat.matrix[2][i] * item->model.matrix[j][2] + rotationMat.matrix[3][i] * item->model.matrix[j][3];
				}
			}
			item->model = outputMat;
		}

		static void Translate(pen::Item3D* item, float tx, float ty, float tz) {
			/*Translate a 3d item*/
			pen::Mat4x4 translationMat = pen::Mat4x4(1.0f, true);
			pen::Mat4x4 outputMat = pen::Mat4x4(1.0f, true);

			translationMat.matrix[3][0] = tx;
			translationMat.matrix[3][1] = ty;
			translationMat.matrix[3][2] = tz;

			/*Multiply translation matrix with the item matrix*/
			for (int j = 0; j < 3; j++) {
				for (int i = 0; i < 3; i++) {
					outputMat.matrix[j][i] = translationMat.matrix[0][i] * item->model.matrix[j][0] + translationMat.matrix[1][i] * item->model.matrix[j][1]
						+ translationMat.matrix[2][i] * item->model.matrix[j][2] + translationMat.matrix[3][i] * item->model.matrix[j][3];
				}
			}
			item->model = outputMat;
		}
	}
}