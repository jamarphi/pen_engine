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
#include "pixel.h"
#include "../ops/operations/3d.h"
#include "../renderer/camera.h"
#include <list>

namespace pen {
	namespace _3d {
		static void RasterizeTriangle(int x1, int y1, float u1, float v1, float w1, pen::Vec4 c1,
			int x2, int y2, float u2, float v2, float w2, pen::Vec4 c2,
			int x3, int y3, float u3, float v3, float w3, pen::Vec4 c3,
			const std::string& path)
		{
			/*Rasterizes triangle and draws its texture*/
			pen::State* inst = pen::State::Get();
			int depthBufferSize = pen::PixelBufferWidth() * pen::PixelBufferHeight();
			if (y2 < y1)
			{
				std::swap(y1, y2); std::swap(x1, x2); std::swap(u1, u2); std::swap(v1, v2);	std::swap(w1, w2); std::swap(c1, c2);
			}

			if (y3 < y1)
			{
				std::swap(y1, y3); std::swap(x1, x3); std::swap(u1, u3); std::swap(v1, v3); std::swap(w1, w3); std::swap(c1, c3);
			}

			if (y3 < y2)
			{
				std::swap(y2, y3); std::swap(x2, x3); std::swap(u2, u3); std::swap(v2, v3); std::swap(w2, w3); std::swap(c2, c3);
			}

			int dy1 = y2 - y1;
			int dx1 = x2 - x1;
			float dv1 = v2 - v1;
			float du1 = u2 - u1;
			float dw1 = w2 - w1;
			int dcr1 = (c2.x - c1.x) * 255.0f;
			int dcg1 = (c2.y - c1.y) * 255.0f;
			int dcb1 = (c2.z - c1.z) * 255.0f;
			int dca1 = (c2.w - c1.w) * 255.0f;

			int dy2 = y3 - y1;
			int dx2 = x3 - x1;
			float dv2 = v3 - v1;
			float du2 = u3 - u1;
			float dw2 = w3 - w1;
			int dcr2 = (c3.x - c1.x) * 255.0f;
			int dcg2 = (c3.y - c1.y) * 255.0f;
			int dcb2 = (c3.z - c1.z) * 255.0f;
			int dca2 = (c3.w - c1.w) * 255.0f;

			float texU, texV, texW;
			float colR, colG, colB, colA;

			float daxStep = 0, dbxStep = 0,
				du1Step = 0, dv1Step = 0,
				du2Step = 0, dv2Step = 0,
				dw1Step = 0, dw2Step = 0,
				dcr1Step = 0, dcr2Step = 0,
				dcg1Step = 0, dcg2Step = 0,
				dcb1Step = 0, dcb2Step = 0,
				dca1Step = 0, dca2Step = 0;

			if (dy1) daxStep = dx1 / pen::op::Abs(dy1);
			if (dy2) dbxStep = dx2 / pen::op::Abs(dy2);

			if (dy1) du1Step = du1 / pen::op::Abs(dy1);
			if (dy1) dv1Step = dv1 / pen::op::Abs(dy1);
			if (dy1) dw1Step = dw1 / pen::op::Abs(dy1);

			if (dy2) du2Step = du2 / pen::op::Abs(dy2);
			if (dy2) dv2Step = dv2 / pen::op::Abs(dy2);
			if (dy2) dw2Step = dw2 / pen::op::Abs(dy2);

			if (dy1) dcr1Step = dcr1 / pen::op::Abs(dy1);
			if (dy1) dcg1Step = dcg1 / pen::op::Abs(dy1);
			if (dy1) dcb1Step = dcb1 / pen::op::Abs(dy1);
			if (dy1) dca1Step = dca1 / pen::op::Abs(dy1);

			if (dy2) dcr2Step = dcr2 / pen::op::Abs(dy2);
			if (dy2) dcg2Step = dcg2 / pen::op::Abs(dy2);
			if (dy2) dcb2Step = dcb2 / pen::op::Abs(dy2);
			if (dy2) dca2Step = dca2 / pen::op::Abs(dy2);

			float pixelR = 0.0f;
			float pixelG = 0.0f;
			float pixelB = 0.0f;
			float pixelA = 1.0f;

			if (dy1)
			{
				for (int i = y1; i <= y2; i++)
				{
					int ax = int(x1 + (float)(i - y1) * daxStep);
					int bx = int(x1 + (float)(i - y1) * dbxStep);

					float texSu = u1 + (float)(i - y1) * du1Step;
					float texSv = v1 + (float)(i - y1) * dv1Step;
					float texSw = pen::op::Abs(w1) + (float)(i - y1) * dw1Step;

					float texEu = u1 + (float)(i - y1) * du2Step;
					float texEv = v1 + (float)(i - y1) * dv2Step;
					float texEw = pen::op::Abs(w1) + (float)(i - y1) * dw2Step;

					float colSr = (c1.x * 255.0f) + (float)(i - y1) * dcr1Step;
					float colSg = (c1.y * 255.0f) + (float)(i - y1) * dcg1Step;
					float colSb = (c1.z * 255.0f) + (float)(i - y1) * dcb1Step;
					float colSa = (c1.w * 255.0f) + (float)(i - y1) * dca1Step;
								  	    
					float colEr = (c1.x * 255.0f) + (float)(i - y1) * dcr2Step;
					float colEg = (c1.y * 255.0f) + (float)(i - y1) * dcg2Step;
					float colEb = (c1.z * 255.0f) + (float)(i - y1) * dcb2Step;
					float colEa = (c1.w * 255.0f) + (float)(i - y1) * dca2Step;

					if (ax > bx)
					{
						std::swap(ax, bx);
						std::swap(texSu, texEu);
						std::swap(texSv, texEv);
						std::swap(texSw, texEw);
						std::swap(colSr, colEr);
						std::swap(colSg, colEg);
						std::swap(colSb, colEb);
						std::swap(colSa, colEa);
					}

					texU = texSu;
					texV = texSv;
					texW = texSw;
					colR = colSr;
					colG = colSg;
					colB = colSb;
					colA = colSa;

					float tStep = 1.0f / pen::op::Max(bx - ax, 10.0f);
					float t = 0.0f;

					for (int j = ax; j < bx; j++)
					{
						texU = (1.0f - t) * texSu + t * texEu;
						texV = (1.0f - t) * texSv + t * texEv;
						texW = (1.0f - t) * texSw + t * texEw;
						colR = (1.0f - t) * colSr + t * colEr;
						colG = (1.0f - t) * colSg + t * colEg;
						colB = (1.0f - t) * colSb + t * colEb;
						colA = (1.0f - t) * colSa + t * colEa;

						pixelR = colR / 255.0f;
						pixelG = colG / 255.0f;
						pixelB = colB / 255.0f;
						pixelA = colA / 255.0f;

						if (path != "")
						{
							unsigned char* spriteData = nullptr;
							int texWidth = 0, texHeight = 0, texBPP = 0;
							pen::State* inst = pen::State::Get();
							pen::LoadSprite(path, spriteData, &texWidth, &texHeight, &texBPP);

							if (spriteData != nullptr)
							{
								int sx = pen::op::Min(((texU / texW) * (float)texWidth), texWidth - 1);
								int sy = pen::op::Min(((texV / texW) * (float)texHeight), texHeight - 1);								

								pixelR *= (((float)spriteData[sy * texWidth + (sx * 4)]) / 255.0f);
								pixelG *= (((float)spriteData[sy * texWidth + (sx * 4)] + 1) / 255.0f);
								pixelB *= (((float)spriteData[sy * texWidth + (sx * 4)] + 2) / 255.0f);
								pixelA *= (((float)spriteData[sy * texWidth + (sx * 4)] + 3) / 255.0f);
							}
						}

						int depthBufferPixel = (int)pen::op::Max(-1.0f, i * pen::PixelBufferWidth() + j);
						if (depthBufferPixel > -1 && depthBufferPixel < depthBufferSize) {
							if (texW >= inst->depthBuffer[depthBufferPixel]) {
								pen::Draw(j, i, pen::Vec4(pixelR, pixelG, pixelB, pixelA));
								inst->depthBuffer[depthBufferPixel] = texW;
							}
						}
						else {
							pen::Draw(j, i, pen::Vec4(pixelR, pixelG, pixelB, pixelA));
						}

						t += tStep;
					}
				}
			}

			dy1 = y3 - y2;
			dx1 = x3 - x2;
			dv1 = v3 - v2;
			du1 = u3 - u2;
			dw1 = w3 - w2;
			dcr1 = (c3.x - c2.x) * 255.0f;
			dcg1 = (c3.y - c2.y) * 255.0f;
			dcb1 = (c3.z - c2.z) * 255.0f;
			dca1 = (c3.w - c2.w) * 255.0f;

			if (dy1) daxStep = dx1 / pen::op::Abs(dy1);
			if (dy2) dbxStep = dx2 / pen::op::Abs(dy2);

			du1Step = 0; dv1Step = 0;
			if (dy1) du1Step = du1 / pen::op::Abs(dy1);
			if (dy1) dv1Step = dv1 / pen::op::Abs(dy1);
			if (dy1) dw1Step = dw1 / pen::op::Abs(dy1);

			dcr1Step = 0; dcg1Step = 0; dcb1Step = 0; dca1Step = 0;
			if (dy1) dcr1Step = dcr1 / pen::op::Abs(dy1);
			if (dy1) dcg1Step = dcg1 / pen::op::Abs(dy1);
			if (dy1) dcb1Step = dcb1 / pen::op::Abs(dy1);
			if (dy1) dca1Step = dca1 / pen::op::Abs(dy1);

			if (dy1)
			{
				for (int i = y2; i <= y3; i++)
				{
					int ax = int(x2 + (float)(i - y2) * daxStep);
					int bx = int(x1 + (float)(i - y1) * dbxStep);

					float texSu = u2 + (float)(i - y2) * du1Step;
					float texSv = v2 + (float)(i - y2) * dv1Step;
					float texSw = w2 + (float)(i - y2) * dw1Step;

					float texEu = u1 + (float)(i - y1) * du2Step;
					float texEv = v1 + (float)(i - y1) * dv2Step;
					float texEw = w1 + (float)(i - y1) * dw2Step;

					float colSr = (c2.x * 255.0f) + (float)(i - y2) * dcr1Step;
					float colSg = (c2.y * 255.0f) + (float)(i - y2) * dcg1Step;
					float colSb = (c2.z * 255.0f) + (float)(i - y2) * dcb1Step;
					float colSa = (c2.w * 255.0f) + (float)(i - y2) * dca1Step;
								  
					float colEr = (c1.x * 255.0f) + (float)(i - y1) * dcr2Step;
					float colEg = (c1.y * 255.0f) + (float)(i - y1) * dcg2Step;
					float colEb = (c1.z * 255.0f) + (float)(i - y1) * dcb2Step;
					float colEa = (c1.w * 255.0f) + (float)(i - y1) * dca2Step;

					if (ax > bx)
					{
						std::swap(ax, bx);
						std::swap(texSu, texEu);
						std::swap(texSv, texEv);
						std::swap(texSw, texEw);
						std::swap(colSr, colEr);
						std::swap(colSg, colEg);
						std::swap(colSb, colEb);
						std::swap(colSa, colEa);
					}

					texU = texSu;
					texV = texSv;
					texW = texSw;
					colR = colSr;
					colG = colSg;
					colB = colSb;
					colA = colSa;

					float tStep = 1.0f / ((float)(bx - ax));
					float t = 0.0f;

					for (int j = ax; j < bx; j++)
					{
						texU = (1.0f - t) * texSu + t * texEu;
						texV = (1.0f - t) * texSv + t * texEv;
						texW = (1.0f - t) * texSw + t * texEw;
						colR = (1.0f - t) * colSr + t * colEr;
						colG = (1.0f - t) * colSg + t * colEg;
						colB = (1.0f - t) * colSb + t * colEb;
						colA = (1.0f - t) * colSa + t * colEa;

						pixelR = colR / 255.0f;
						pixelG = colG / 255.0f;
						pixelB = colB / 255.0f;
						pixelA = colA / 255.0f;

						if (path != "")
						{
							unsigned char* spriteData = nullptr;
							int texWidth = 0, texHeight = 0, texBPP = 0;
							pen::State* inst = pen::State::Get();
							pen::LoadSprite(path, spriteData, &texWidth, &texHeight, &texBPP);

							if (spriteData != nullptr)
							{
								int sx = pen::op::Min(((texU / texW) * (float)texWidth), texWidth - 1);
								int sy = pen::op::Min(((texV / texW) * (float)texHeight), texHeight - 1);

								pixelR *= (((float)spriteData[sy * texWidth + (sx * 4)]) / 255.0f);
								pixelG *= (((float)spriteData[sy * texWidth + (sx * 4)] + 1) / 255.0f);
								pixelB *= (((float)spriteData[sy * texWidth + (sx * 4)] + 2) / 255.0f);
								pixelA *= (((float)spriteData[sy * texWidth + (sx * 4)] + 3) / 255.0f);
							}
						}

						int depthBufferPixel = (int)pen::op::Max(-1.0f, i * pen::PixelBufferWidth() + j);
						if (depthBufferPixel > -1 && depthBufferPixel < depthBufferSize) {
							if (texW >= inst->depthBuffer[depthBufferPixel]) {
								pen::Draw(j, i, pen::Vec4(pixelR, pixelG, pixelB, pixelA));
								inst->depthBuffer[depthBufferPixel] = texW;
							}
						}
						else {
							pen::Draw(j, i, pen::Vec4(pixelR, pixelG, pixelB, pixelA));
						}

						t += tStep;
					}
				}
			}
		}
	}

	struct Item3D {
		/*3D pixel buffer item*/
		std::vector<pen::_3d::Triangle*>* triangles;
		bool isWireframe = false;
		pen::Mat4x4 matrix = pen::Mat4x4(1.0f);
		static pen::Camera camera;

		void Draw() {
			/*Draws the 3d item to the pixel buffer*/
			pen::State* inst = pen::State::Get();

			for (int tx = 0; tx < triangles->size(); tx++)
			{
				pen::_3d::Triangle* tri = triangles->at(tx);
				tri->point[0].w = 1.0f;
				tri->point[1].w = 1.0f;
				tri->point[2].w = 1.0f;
				pen::_3d::Triangle triTransformed;

				triTransformed.textureCoord[0] = pen::Vec3(tri->textureCoord[0].x, tri->textureCoord[0].y, tri->textureCoord[0].z);
				triTransformed.textureCoord[1] = pen::Vec3(tri->textureCoord[1].x, tri->textureCoord[1].y, tri->textureCoord[1].z);
				triTransformed.textureCoord[2] = pen::Vec3(tri->textureCoord[2].x, tri->textureCoord[2].y, tri->textureCoord[2].z);

				triTransformed.color[0] = tri->color[0];
				triTransformed.color[1] = tri->color[1];
				triTransformed.color[2] = tri->color[2];

				pen::Mat4x4 mv = matrix * inst->pixel3DView;
				triTransformed.point[0] = pen::op::Mat4x4MultVec4(mv, tri->point[0], false);
				triTransformed.point[1] = pen::op::Mat4x4MultVec4(mv, tri->point[1], false);
				triTransformed.point[2] = pen::op::Mat4x4MultVec4(mv, tri->point[2], false);
				
				pen::Vec4 normal;
				pen::Vec4 line1;
				pen::Vec4 line2;
				line1 = triTransformed.point[1] - triTransformed.point[0];
				line2 = triTransformed.point[2] - triTransformed.point[0];
				pen::Vec3 normalRes = pen::op::CrossProduct(pen::Vec3(line1.x, line1.y, line1.z), pen::Vec3(line2.x, line2.y, line2.z)).Normalize();
				normal = pen::Vec4(normalRes.x, normalRes.y, normalRes.z, 1.0f);
				if (pen::op::DotProductVec4(normal, triTransformed.point[0]) < 0.0f) continue;

				pen::Vec4 lightDirection = pen::Vec4(0.0f, 0.0f, -1.0f, 1.0f);
				float light = pen::op::DotProductVec4(normal, lightDirection);
				light = pen::op::Abs(light);
				pen::Vec4 lightVector = pen::Vec4(light * pen::PEN_WHITE.x, light * pen::PEN_WHITE.y, light * pen::PEN_WHITE.z, light * pen::PEN_WHITE.w);
				triTransformed.color[0] = lightVector * pen::op::Min(1.0f / triTransformed.point[0].z, 1.0f) * triTransformed.color[0];
				triTransformed.color[1] = lightVector * pen::op::Min(1.0f / triTransformed.point[1].z, 1.0f) * triTransformed.color[1];
				triTransformed.color[2] = lightVector * pen::op::Min(1.0f / triTransformed.point[2].z, 1.0f) * triTransformed.color[2];

				int clippedTriangles = 0;
				pen::_3d::Triangle clipped[2];
				pen::Vec4 nearPlane = pen::Vec4(0.0f, 0.0f, 0.1f, 1.0f);
				pen::Vec4 farPlane = pen::Vec4(0.0f, 0.0f, 1000.0f, 1.0f);
				clippedTriangles = pen::op::ClipAgainstPlane(nearPlane, farPlane, triTransformed, clipped[0], clipped[1]);

				for (int n = 0; n < clippedTriangles; n++)
				{
					pen::_3d::Triangle triProjected = clipped[n];

					triProjected.point[0] = pen::op::Mat4x4MultVec4(inst->pixel3DProjection, clipped[n].point[0], false);
					triProjected.point[1] = pen::op::Mat4x4MultVec4(inst->pixel3DProjection, clipped[n].point[1], false);
					triProjected.point[2] = pen::op::Mat4x4MultVec4(inst->pixel3DProjection, clipped[n].point[2], false);

					pen::_3d::Triangle sClipped[2];
					std::list<pen::_3d::Triangle> listTriangles;


					// Add initial triangle
					listTriangles.push_back(triProjected);
					int newTriangles = 1;
					
					for (int plane = 0; plane < 4; plane++)
					{
						int trisToAdd = 0;
						while (newTriangles > 0)
						{
							pen::_3d::Triangle test = listTriangles.front();
							listTriangles.pop_front();
							newTriangles--;

							switch (plane)
							{
							case 0:	
								nearPlane = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
								farPlane = pen::Vec4(0.0f, pen::PixelBufferHeight(), 0.0f, 1.0f);
								trisToAdd = pen::op::ClipAgainstPlane(nearPlane, farPlane, test, sClipped[0], sClipped[1]); 
								break;
							case 1:	
								nearPlane = pen::Vec4(0.0f, pen::PixelBufferHeight(), 0.0f, 1.0f);
								farPlane = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
								trisToAdd = pen::op::ClipAgainstPlane(nearPlane, farPlane, test, sClipped[0], sClipped[1]); 
								break;
							case 2:	
								nearPlane = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
								farPlane = pen::Vec4(pen::PixelBufferWidth(), 0.0f, 0.0f, 1.0f);
								trisToAdd = pen::op::ClipAgainstPlane(nearPlane, farPlane, test, sClipped[0], sClipped[1]); 
								break;
							case 3:	
								nearPlane = pen::Vec4(pen::PixelBufferWidth(), 0.0f, 0.0f, 1.0f);
								farPlane = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
								trisToAdd = pen::op::ClipAgainstPlane(nearPlane, farPlane, test, sClipped[0], sClipped[1]); 
								break;
							default:
								break;
							}
							
							for (int w = 0; w < trisToAdd; w++) {
								listTriangles.push_back(sClipped[w]);
							}
						}
						newTriangles = listTriangles.size();
					}

					for (int i = 0; i < listTriangles.size(); i++)
					{
						pen::Vec4 offsetView = pen::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
						pen::_3d::Triangle triRaster = listTriangles.front();
						listTriangles.pop_front();
						triRaster.point[0] = triRaster.point[0] + offsetView;
						triRaster.point[1] = triRaster.point[1] + offsetView;
						triRaster.point[2] = triRaster.point[2] + offsetView;

						if (isWireframe)
						{
							pen::DrawTri(triRaster.point[0].x, triRaster.point[0].y, triRaster.point[1].x, triRaster.point[1].y, triRaster.point[2].x, triRaster.point[2].y, triRaster.color[0], false, true);
						}
						else
						{
							pen::_3d::RasterizeTriangle(
								(int)triRaster.point[0].x, (int)triRaster.point[0].y, triRaster.textureCoord[0].x, triRaster.textureCoord[0].y, triRaster.textureCoord[0].z, triRaster.color[0],
								(int)triRaster.point[1].x, (int)triRaster.point[1].y, triRaster.textureCoord[1].x, triRaster.textureCoord[1].y, triRaster.textureCoord[1].z, triRaster.color[1],
								(int)triRaster.point[2].x, (int)triRaster.point[2].y, triRaster.textureCoord[2].x, triRaster.textureCoord[2].y, triRaster.textureCoord[2].z, triRaster.color[2],
								triRaster.texture);
						}
					}
				}
			}
		}
	};

	static pen::Camera* GetPixelCamera() {
		/*Returns the camera for 3D pixel items*/
		return &pen::Item3D::camera;
	}

	static pen::Mat4x4* GetPixelView() {
		/*Returns the view matrix for 3D pixel items*/
		return &pen::State::Get()->pixel3DView;
	}

	static pen::Mat4x4* GetPixelProjection() {
		/*Returns the projection matrix for 3D pixel items*/
		return &pen::State::Get()->pixel3DProjection;
	}

	static void Pan(float x, float y, float z) {
		/*Pans the pixel camera*/
		if (x != 0.0f) pen::Item3D::camera.cameraPosition += ((pen::op::CrossProduct(pen::Item3D::camera.viewOrientation, pen::Item3D::camera.at).Normalize()) * pen::Item3D::camera.cameraSpeed * x);
		if (y != 0.0f) pen::Item3D::camera.cameraPosition += (pen::Item3D::camera.at * pen::Item3D::camera.cameraSpeed * y);
		if (z != 0.0f) pen::Item3D::camera.cameraPosition += (pen::Item3D::camera.viewOrientation * pen::Item3D::camera.cameraSpeed * z);
	}

	static void Look(pen::Vec3 direction) {
		/*Aims the pixel camera*/
		float angleX = pen::op::ACos(pen::op::DotProduct(pen::Vec3(pen::Item3D::camera.viewOrientation.x, 0.0f, 0.0f), pen::Vec3(direction.x, 0.0f, 0.0f)) / (pen::Item3D::camera.viewOrientation.x * direction.x));
		float angleY = pen::op::ACos(pen::op::DotProduct(pen::Vec3(pen::Item3D::camera.viewOrientation.y, 0.0f, 0.0f), pen::Vec3(direction.y, 0.0f, 0.0f)) / (pen::Item3D::camera.viewOrientation.y * direction.y));
		float angleZ = pen::op::ACos(pen::op::DotProduct(pen::Vec3(pen::Item3D::camera.viewOrientation.z, 0.0f, 0.0f), pen::Vec3(direction.z, 0.0f, 0.0f)) / (pen::Item3D::camera.viewOrientation.z * direction.z));
		pen::Vec3 axisX = pen::Vec3(pen::Item3D::camera.at.x, 0.0f, 0.0f);
		pen::Vec3 axisY = pen::Vec3(pen::Item3D::camera.at.y, 0.0f, 0.0f);
		pen::Vec3 axisZ = pen::Vec3(pen::Item3D::camera.at.z, 0.0f, 0.0f);

		pen::Vec3 newOrientation = pen::op::RotateVec(pen::Item3D::camera.viewOrientation, -1.0f * angleX,
			(pen::op::CrossProduct(pen::Item3D::camera.viewOrientation, axisX).Normalize()));

		if (!(pen::op::AngleBetween(newOrientation, axisX) <= 5.0f * 3.14159f / 180.0f
			|| pen::op::AngleBetween(newOrientation, axisX * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
			pen::Item3D::camera.viewOrientation = newOrientation;
		}

		newOrientation = pen::op::RotateVec(pen::Item3D::camera.viewOrientation, -1.0f * angleY,
			(pen::op::CrossProduct(pen::Item3D::camera.viewOrientation, axisY).Normalize()));

		if (!(pen::op::AngleBetween(newOrientation, axisY) <= 5.0f * 3.14159f / 180.0f
			|| pen::op::AngleBetween(newOrientation, axisY * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
			pen::Item3D::camera.viewOrientation = newOrientation;
		}

		pen::Item3D::camera.viewOrientation = pen::op::RotateVec(pen::Item3D::camera.viewOrientation, -1.0f * angleZ,
			(pen::op::CrossProduct(pen::Item3D::camera.viewOrientation, axisZ).Normalize()));
	}

	namespace _3d {
		static void LoadMtl(const std::string& path) {
			/*Loads in a .mtl file*/
			pen::State* inst = pen::State::Get();
			bool mtlLoaded = false;
			std::string tempPath = (path.find(":") != std::string::npos) ? path : GENERAL_MODEL_SOURCE + path;
			for (int i = 0; i < inst->pixel3DMtlList->size(); i++) {
				if (inst->pixel3DMtlList->at(i)->file == path) {
					mtlLoaded = true;
					break;
				}
			}

			if (!mtlLoaded) {
#ifndef __PEN_MOBILE__
				std::ifstream mtlFile;
				mtlFile.open(tempPath);
				if (mtlFile.is_open()) {
#else
				pen::Asset mobileMtl = pen::Asset::Load(tempPath, nullptr);
				if (mobileMtl.data != nullptr) {
#endif
					std::string material;
					std::string texture;
					pen::Vec4 color = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
#ifndef __PEN_MOBILE__
					while (!mtlFile.eof()) {
#else
					unsigned int fileOffset = 0;
					std::string mobileMtlText(mobileMtl.data);
					while (true) {
#endif

#ifndef __PEN_MOBILE__
						char fileLine[150];
						mtlFile.getline(fileLine, 150);
#else
						std::string fileLine = pen::ui::ReadLine(mobileMtlText, &fileOffset);
						if (fileLine == "") {
							break;
						}
#endif

						std::stringstream stream;
						std::string firstElement;
						stream << fileLine;

						if (fileLine[0] == 'n' && fileLine[1] == 'e') {
							/*Name of material*/
							stream >> firstElement >> material;
						}
						else if (fileLine[0] == 'k' && fileLine[1] == 'd') {
							/*Color data*/
							stream >> firstElement >> color.x >> color.y >> color.z;
						}
						else if (fileLine[1] == 'm') {
							/*Texture file*/
							stream >> firstElement >> texture;

							/*The map_Kd information is last line for each material*/
							inst->pixel3DMtlList->push_back(new pen::_3d::Pixel3DMtlData{ path, material, color, texture });
						}

					}

#ifndef __PEN_MOBILE__
					mtlFile.close();
#endif
				}
			}
		}

		static pen::Item3D* CreateItem3D(const std::string& path, const pen::Vec4& objectColor, const bool& isWireFrame) {
			/*Loads in an obj file for the vertices and indices*/
			std::string tempPath = (path.find(":") != std::string::npos) ? path : GENERAL_MODEL_SOURCE + path;
			std::vector<pen::_3d::Triangle*>* triangles = new std::vector<pen::_3d::Triangle*>();
			std::vector<pen::_3d::Pixel3DMtlData*>* mtlList = new std::vector<pen::_3d::Pixel3DMtlData*>();
			pen::State* inst = pen::State::Get();

#ifndef __PEN_MOBILE__
			std::ifstream modelFile;
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
#else
			pen::Asset mobileObj = pen::Asset::Load(tempPath, nullptr);
			int lineOffset = 0;
			if (mobileObj.data != nullptr) {
#endif
				/*Create the 3D layer and item*/
				unsigned int faceCounter = 0;
				bool quadFormatError = false;
				std::vector<pen::Vec4> vertices;
				std::vector<pen::Vec3> texCoords;
				pen::Vec4 vertex1 = pen::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
				bool firstVertex = true;
				bool normalize = false;
				std::string mtlPath;
				std::vector<pen::_3d::Pixel3DMtlData*>* materialList = new std::vector<pen::_3d::Pixel3DMtlData*>();
				pen::_3d::Pixel3DMtlData* material = nullptr;
				std::vector<pen::_3d::Pixel3DMtlData*> tempMaterialVector;


#ifndef __PEN_MOBILE__
				while (!modelFile.eof()) {
#else
				unsigned int fileOffset = 0;
				std::string mobileObjText(mobileObj.data);
				while (true) {
#endif
					/*It is assumed that the vertex points in the obj file will be normalized already*/

#ifndef __PEN_MOBILE__
					char fileLine[150];
					modelFile.getline(fileLine, 150);
#else
					std::string fileLine = pen::ui::ReadLine(mobileObjText, &fileOffset);
					if (fileLine == "") {
						break;
					}
#endif

					std::stringstream stream;
					std::string firstElement;
					std::string materialStr;
					std::string point1;
					std::string point2;
					std::string point3;
					std::string point4;
					int index1 = 0;
					int index2 = 0;
					int index3 = 0;
					int index4 = 0;
					stream << fileLine;

					if (fileLine[0] == 'm' && fileLine[1] == 't') {
						/*Select material file*/
						stream >> firstElement >> mtlPath;
						pen::_3d::LoadMtl(mtlPath);
						materialList->clear();
						for (int i = 0; i < inst->pixel3DMtlList->size(); i++) {
							if (inst->pixel3DMtlList->at(i)->file == mtlPath) {
								materialList->push_back(inst->pixel3DMtlList->at(i));
							}
						}
					}
					else if (fileLine[0] == 'u' && fileLine[1] == 's') {
						/*Material to select*/
						stream >> firstElement >> materialStr;
						for (int i = 0; i < materialList->size(); i++) {
							if (materialList->at(i)->name == materialStr) {
								material = materialList->at(i);
								break;
							}
						}
					}
					else if (fileLine[0] == 'v' && fileLine[1] == ' ') {
						/*Vertex point*/
						stream >> firstElement >> vertex1.x >> vertex1.y >> vertex1.z;

						if (firstVertex) {
							/*Divides all vertex points by 10 if one of the first vertex points is greater than 10*/
							if (pen::op::Abs(vertex1.x) > 10.0f || pen::op::Abs(vertex1.y) > 10.0f
								|| pen::op::Abs(vertex1.z) > 10.0f) {
								vertex1.x /= 10.0f;
								vertex1.y /= 10.0f;
								vertex1.z /= 10.0f;
								normalize = true;
								firstVertex = false;
							}
						}
						else {
							if (normalize) {
								vertex1.x /= 10.0f;
								vertex1.y /= 10.0f;
								vertex1.z /= 10.0f;
							}
						}

						/*Converts coordinates to pixel buffer space*/
						vertex1.x = vertex1.x * (float)pen::PixelBufferWidth() / (float)inst->screenWidth;
						vertex1.y = vertex1.y * (float)pen::PixelBufferHeight() / (float)inst->screenHeight;
						vertices.push_back(vertex1);
						tempMaterialVector.push_back(material);
					}
					else if (fileLine[0] == 'v' && fileLine[1] == 't') {
						/*Vertex point texture coordinates*/
						stream >> firstElement >> firstElement >> vertex1.x >> vertex1.y;
						texCoords.push_back(pen::Vec3(vertex1.x, vertex1.y, 1.0f));
					}
					else if (fileLine[0] == 'f') {
						/*Face data*/
						stream >> firstElement >> point1 >> point2 >> point3 >> point4;
						std::string faceIndex1 = pen::ui::Split(point1, '/', 0);
						std::string faceIndex2 = pen::ui::Split(point2, '/', 0);
						std::string faceIndex3 = pen::ui::Split(point3, '/', 0);
						std::string faceIndex4 = point4.size() > 0 ? pen::ui::Split(point4, '/', 0) : "";

						/*Obj files indices start at 1*/
						index1 = std::stoi(faceIndex1) - 1;
						index2 = std::stoi(faceIndex2) - 1;
						index3 = std::stoi(faceIndex3) - 1;
						index4 = faceIndex4.size() > 0 ? std::stoi(faceIndex4) - 1 : -1;

						/*Creates the triangles with given indices*/
						if (texCoords.size() > 0 && texCoords.size() == vertices.size()) {
							triangles->push_back(new pen::_3d::Triangle{
								{vertices[index1], vertices[index2], vertices[index3]},
								{texCoords[index1], texCoords[index2], texCoords[index3]},
								{objectColor, objectColor, objectColor},
								mtlPath
								});
						}
						else {
							triangles->push_back(new pen::_3d::Triangle{
								{vertices[index1], vertices[index2], vertices[index3]},
								{pen::Vec3(0.0f,0.0f,0.0f), pen::Vec3(0.0f,0.0f,0.0f), pen::Vec3(0.0f,0.0f,0.0f)},
								{objectColor, objectColor, objectColor},
								mtlPath
								});
						}

						if (index4 > -1) {
							/*Quad*/
							if (texCoords.size() > 0 && texCoords.size() == vertices.size()) {
								triangles->push_back(new pen::_3d::Triangle{
									{vertices[index3], vertices[index4], vertices[index2]},
									{texCoords[index3], texCoords[index4], texCoords[index2]},
									{objectColor, objectColor, objectColor},
									mtlPath
									});
							}
							else {
								triangles->push_back(new pen::_3d::Triangle{
									{vertices[index1], vertices[index2], vertices[index3]},
									{pen::Vec3(0.0f,0.0f,0.0f), pen::Vec3(0.0f,0.0f,0.0f), pen::Vec3(0.0f,0.0f,0.0f)},
									{objectColor, objectColor, objectColor},
									mtlPath
									});
							}
							faceCounter += 2;
						}
						else {
							faceCounter++;
						}
					}
				}

#ifndef __PEN_MOBILE__
				modelFile.close();
#endif
				if (triangles->size() > 0) return new pen::Item3D {triangles, isWireFrame};
			}
			return nullptr;
		}

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
						outputMat.matrix[j][i] = scaleMat.matrix[0][i] * item->matrix.matrix[j][0] + scaleMat.matrix[1][i] * item->matrix.matrix[j][1]
							+ scaleMat.matrix[2][i] * item->matrix.matrix[j][2] + scaleMat.matrix[3][i] * item->matrix.matrix[j][3];
					}
				}
				item->matrix = outputMat;
			}
		}

		static void Rotate(pen::Item3D* item, float theta, int axis) {
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
					outputMat.matrix[j][i] = rotationMat.matrix[0][i] * item->matrix.matrix[j][0] + rotationMat.matrix[1][i] * item->matrix.matrix[j][1]
						+ rotationMat.matrix[2][i] * item->matrix.matrix[j][2] + rotationMat.matrix[3][i] * item->matrix.matrix[j][3];
				}
			}
			item->matrix = outputMat;
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
					outputMat.matrix[j][i] = translationMat.matrix[0][i] * item->matrix.matrix[j][0] + translationMat.matrix[1][i] * item->matrix.matrix[j][1]
						+ translationMat.matrix[2][i] * item->matrix.matrix[j][2] + translationMat.matrix[3][i] * item->matrix.matrix[j][3];
				}
			}
			item->matrix = outputMat;
		}
	}
}