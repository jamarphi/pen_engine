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

		struct Triangle {
			pen::Vec4 point[3];
			pen::Vec3 textureCoord[3];
			pen::Vec4 color[3];
			std::string texture;
		};
	}

	namespace ui {
		namespace _3d {
			static void Scale(pen::GraphicallyAcceleratedItem3D* item, float sx, float sy, float sz) {
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

			static void Rotate(pen::GraphicallyAcceleratedItem3D* item, float theta, int axis) {
				/*Rotate a 3d item*/
				pen::Mat4x4 rotationMat = pen::Mat4x4(1.0f, true);
				pen::Mat4x4 outputMat = pen::Mat4x4(1.0f, true);

				switch (axis) {
				case pen::_3d::AXIS::X:
					rotationMat.matrix[1][1] = pen::op::Cos(theta);
					rotationMat.matrix[1][2] = pen::op::Sin(theta);
					rotationMat.matrix[2][1] = -1.0f * pen::op::Sin(theta);
					rotationMat.matrix[2][2] = pen::op::Cos(theta);
					break;
				case pen::_3d::AXIS::Y:
					rotationMat.matrix[0][0] = pen::op::Cos(theta);
					rotationMat.matrix[0][2] = pen::op::Sin(theta);
					rotationMat.matrix[2][0] = -1.0f * pen::op::Sin(theta);
					rotationMat.matrix[2][2] = pen::op::Cos(theta);
					break;
				case pen::_3d::AXIS::Z:
					rotationMat.matrix[0][0] = pen::op::Cos(theta);
					rotationMat.matrix[0][1] = pen::op::Sin(theta);
					rotationMat.matrix[1][0] = -1.0f * pen::op::Sin(theta);
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

			static void Translate(pen::GraphicallyAcceleratedItem3D* item, float tx, float ty, float tz) {
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

	namespace op {
		static pen::Vec4 VectorIntersectPlane(pen::Vec4& planeA, pen::Vec4& planeB, pen::Vec4& lineStart, pen::Vec4& lineEnd, float& tangent)
		{
			/*Computes the vector of intersection with a plane*/
			pen::Vec4 intersectVector = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			planeA.w = 1.0f;
			lineStart.w = 1.0f;
			lineEnd.w = 1.0f;
			planeB = planeB.Normalize();
			planeB.w = 1.0f;
			float planeC = pen::op::DotProductVec4(planeB, planeA);
			float ad = pen::op::DotProductVec4(lineStart, planeB);
			float bd = pen::op::DotProductVec4(lineEnd, planeB);
			tangent = (-planeC - ad) / (bd - ad);
			pen::Vec4 lineStartToEnd = lineEnd - lineStart;
			pen::Vec4 lineToIntersect = lineStartToEnd * tangent;
			return lineStart + lineToIntersect;
		}

		static int ClipAgainstPlane(pen::Vec4& planeA, pen::Vec4& planeB, pen::_3d::Triangle& inputTriangle, pen::_3d::Triangle& outputTriangle1, pen::_3d::Triangle& outputTriangle2)
		{
			/*Checks if a triangle needs to be clipped*/
			planeA.w = 1.0f;
			planeB = planeB.Normalize();
			planeB.w = 1.0f;

			outputTriangle1.textureCoord[0] = inputTriangle.textureCoord[0];
			outputTriangle2.textureCoord[0] = inputTriangle.textureCoord[0];
			outputTriangle1.textureCoord[1] = inputTriangle.textureCoord[1];
			outputTriangle2.textureCoord[1] = inputTriangle.textureCoord[1];
			outputTriangle1.textureCoord[2] = inputTriangle.textureCoord[2];
			outputTriangle2.textureCoord[2] = inputTriangle.textureCoord[2];

			auto distance = [&](pen::Vec4& point)
			{
				return (planeB.x * point.x + planeB.y * point.y + planeB.z * point.z - pen::op::DotProductVec4(planeB, planeA));
			};

			pen::Vec4* insidePoints[3];  
			int insidePointCount = 0;
			pen::Vec4* outsidePoints[3]; 
			int outsidePointCount = 0;
			pen::Vec3* insideTexture[3]; 
			int insideTexCount = 0;
			pen::Vec3* outsideTexture[3]; 
			int outsideTextureCount = 0;


			float d0 = distance(inputTriangle.point[0]);
			float d1 = distance(inputTriangle.point[1]);
			float d2 = distance(inputTriangle.point[2]);

			if (d0 >= 0) { insidePoints[insidePointCount++] = &inputTriangle.point[0]; insideTexture[insideTexCount++] = &inputTriangle.textureCoord[0]; }
			else {
				outsidePoints[outsidePointCount++] = &inputTriangle.point[0]; outsideTexture[outsideTextureCount++] = &inputTriangle.textureCoord[0];
			}
			if (d1 >= 0) {
				insidePoints[insidePointCount++] = &inputTriangle.point[1]; insideTexture[insideTexCount++] = &inputTriangle.textureCoord[1];
			}
			else {
				outsidePoints[outsidePointCount++] = &inputTriangle.point[1];  outsideTexture[outsideTextureCount++] = &inputTriangle.textureCoord[1];
			}
			if (d2 >= 0) {
				insidePoints[insidePointCount++] = &inputTriangle.point[2]; insideTexture[insideTexCount++] = &inputTriangle.textureCoord[2];
			}
			else {
				outsidePoints[outsidePointCount++] = &inputTriangle.point[2];  outsideTexture[outsideTextureCount++] = &inputTriangle.textureCoord[2];
			}

			if (insidePointCount == 0)
			{
				return 0;
			}

			if (insidePointCount == 3)
			{
				outputTriangle1 = inputTriangle;
				return 1;
			}

			if (insidePointCount == 1 && outsidePointCount == 2)
			{
				outputTriangle1.color[0] = inputTriangle.color[0];
				outputTriangle1.color[1] = inputTriangle.color[1];
				outputTriangle1.color[2] = inputTriangle.color[2];

				outputTriangle1.point[0] = *insidePoints[0];
				outputTriangle1.textureCoord[0] = *insideTexture[0];

				float t;
				outputTriangle1.point[1] = pen::op::VectorIntersectPlane(planeA, planeB, *insidePoints[0], *outsidePoints[0], t);
				outputTriangle1.textureCoord[1].x = t * (outsideTexture[0]->x - insideTexture[0]->x) + insideTexture[0]->x;
				outputTriangle1.textureCoord[1].y = t * (outsideTexture[0]->y - insideTexture[0]->y) + insideTexture[0]->y;
				outputTriangle1.textureCoord[1].z = t * (outsideTexture[0]->z - insideTexture[0]->z) + insideTexture[0]->z;

				outputTriangle1.point[2] = pen::op::VectorIntersectPlane(planeA, planeB, *insidePoints[0], *outsidePoints[1], t);
				outputTriangle1.textureCoord[2].x = t * (outsideTexture[1]->x - insideTexture[0]->x) + insideTexture[0]->x;
				outputTriangle1.textureCoord[2].y = t * (outsideTexture[1]->y - insideTexture[0]->y) + insideTexture[0]->y;
				outputTriangle1.textureCoord[2].z = t * (outsideTexture[1]->z - insideTexture[0]->z) + insideTexture[0]->z;

				return 1;
			}

			if (insidePointCount == 2 && outsidePointCount == 1)
			{
				outputTriangle1.color[0] = inputTriangle.color[0];
				outputTriangle2.color[0] = inputTriangle.color[0];
				outputTriangle1.color[1] = inputTriangle.color[1];
				outputTriangle2.color[1] = inputTriangle.color[1];
				outputTriangle1.color[2] = inputTriangle.color[2];
				outputTriangle2.color[2] = inputTriangle.color[2];

				outputTriangle1.point[0] = *insidePoints[0];
				outputTriangle1.textureCoord[0] = *insideTexture[0];

				outputTriangle1.point[1] = *insidePoints[1];
				outputTriangle1.textureCoord[1] = *insideTexture[1];

				float t;
				outputTriangle1.point[2] = pen::op::VectorIntersectPlane(planeA, planeB, *insidePoints[0], *outsidePoints[0], t);
				outputTriangle1.textureCoord[2].x = t * (outsideTexture[0]->x - insideTexture[0]->x) + insideTexture[0]->x;
				outputTriangle1.textureCoord[2].y = t * (outsideTexture[0]->y - insideTexture[0]->y) + insideTexture[0]->y;
				outputTriangle1.textureCoord[2].z = t * (outsideTexture[0]->z - insideTexture[0]->z) + insideTexture[0]->z;
				outputTriangle2.point[1] = *insidePoints[1];
				outputTriangle2.textureCoord[1] = *insideTexture[1];
				outputTriangle2.point[0] = outputTriangle1.point[2];
				outputTriangle2.textureCoord[0] = outputTriangle1.textureCoord[2];
				outputTriangle2.point[2] = pen::op::VectorIntersectPlane(planeA, planeB, *insidePoints[1], *outsidePoints[0], t);
				outputTriangle2.textureCoord[2].x = t * (outsideTexture[0]->x - insideTexture[1]->x) + insideTexture[1]->x;
				outputTriangle2.textureCoord[2].y = t * (outsideTexture[0]->y - insideTexture[1]->y) + insideTexture[1]->y;
				outputTriangle2.textureCoord[2].z = t * (outsideTexture[0]->z - insideTexture[1]->z) + insideTexture[1]->z;
				return 2;
			}

			return 0;
		}
	}
}