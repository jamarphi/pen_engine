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
#include "state.h"
#include "layer_manager.h"
#include "../ops/matrices/mat3x3.h"
#include "../ops/operations/operations.h"
#include "../../ext/platforms/mac_ios/mac_ios_cpp_objective_c_mapping.h"

#ifdef __PEN_ANDROID__
#include "../../ext/platforms/android/pen_engine_android/src/cpp/android_pixel.h"
#endif

namespace pen {

	static void InitializePixels() {
		pen::ui::InitializePixelBuffer();
	}

	static unsigned char* PixelBuffer() {
		/*Returns a pointer to the pixel buffer*/
#ifndef __PEN_ANDROID__
		return pen::State::Get()->pixelArray;
#else
		return nullptr;
#endif
	}

	static int PixelBufferWidth() {
		/*Returns pixel buffer width*/
		return pen::State::Get()->pixelBufferWidth;
	}

	static int PixelBufferHeight() {
		/*Returns pixel buffer height*/
		return pen::State::Get()->pixelBufferHeight;
	}

	static void ScreenToPixel(float* x, float* y) {
		/*Converts screen coordinates to pixel buffer coordinates*/
		pen::State* inst = pen::State::Get();
		*x = (*x * (float)pen::PixelBufferWidth()) / (float)inst->screenWidth;
		*y = (*y * (float)pen::PixelBufferHeight()) / (float)inst->screenHeight;
	}

	static void PixelToScreen(float* x, float* y) {
		/*Converts pixel buffer coordinates to screen coordinates*/
		pen::State* inst = pen::State::Get();
		*x = (*x * (float)inst->screenWidth) / (float)pen::PixelBufferWidth();
		*y = (*y * (float)inst->screenHeight) / (float)pen::PixelBufferHeight();
	}

	static void Pan(float panX, float panY, bool reset = false) {
		/*Pan the pixel buffer based on screen dimensions*/

		/*The distance is normalized and then scaled to be in the range of -1 to 1*/
		pen::ui::LM::pixelLayer->translation.x += panX / pen::State::Get()->screenWidth * -2.0f;
		pen::ui::LM::pixelLayer->translation.y += panY / pen::State::Get()->screenHeight * -2.0f;

		if (reset) {
			pen::ui::LM::pixelLayer->translation.x = 1.0f;
			pen::ui::LM::pixelLayer->translation.y = 1.0f;
		}
	}

	static void Zoom(float mag) {
		/*Zoom in or out on the pixel buffer based on screen dimensions*/
		if (mag >= 0.0f) {
			pen::ui::Item* item = pen::ui::LM::pixelLayer->layerItems[0];
			float newWidth = pen::State::Get()->screenWidth * mag;
			float newHeight = pen::State::Get()->screenHeight * mag;
			item->size.x = newWidth;
			item->size.y = newHeight;
			pen::ui::Submit();
		}
	}

	static void Draw(int x, int y, pen::Vec4 color, bool mask = true) {
		/*Draw a given pixel on the screen*/
		if (!(mask && color.w == 0.0f) && (x > 0 && x < 1280 && y > 0 && y < 720)) {
			pen::State* inst = pen::State::Get();
			inst->pixelDrawn = true;

#ifndef __PEN_ANDROID__
#ifndef __PEN_MAC_IOS__
			inst->pixelArray[y * 5120 + (4 * x)] = (unsigned char)(255 * color.x);
			inst->pixelArray[y * 5120 + (4 * x) + 1] = (unsigned char)(255 * color.y);
			inst->pixelArray[y * 5120 + (4 * x) + 2] = (unsigned char)(255 * color.z);
			inst->pixelArray[y * 5120 + (4 * x) + 3] = (unsigned char)(255 * color.w);
#else
            inst->pixelArray[(720 - y) * 5120 + (4 * x)] = (unsigned char)(255 * color.x);
            inst->pixelArray[(720 - y) * 5120 + (4 * x) + 1] = (unsigned char)(255 * color.y);
            inst->pixelArray[(720 - y) * 5120 + (4 * x) + 2] = (unsigned char)(255 * color.z);
            inst->pixelArray[(720 - y) * 5120 + (4 * x) + 3] = (unsigned char)(255 * color.w);
#endif
#else
        AndroidDrawPixel(x, y, color.x, color.y, color.z, color.w);
#endif
		}
	}

	static void Flush() {
		/*Clear the pixel buffer*/
        pen::State* inst = pen::State::Get();
        inst->pixelDrawn = true;
#ifndef __PEN_ANDROID__
		std::memset(pen::State::Get()->pixelArray, 0, 3686400);
#else
    AndroidPixelFlush();
#endif
	}

	struct Item {
		/*The width and height are in pixels*/
		unsigned char* data;
		int width;
		int height;
		/*The x and y coordinates are in pixel buffer space*/
		int x;
		int y;
		pen::Vec4 color = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/*The matrix is in column-row format*/
		pen::Mat3x3 matrix = pen::Mat3x3(1.0f, true);
		pen::Mat3x3 invMatrix = pen::Mat3x3(1.0f, true);
		bool invert = true;
		/*This callback is used to load a sprite as a math function*/
		float (*displayFunction)(int, int, float) = nullptr;

		void Draw() {
			/*Draws the sprite to the pixel buffer*/

			/*Discovers the bounding box of the pixel sprite*/
			float ex = 0.0f;
			float ey = 0.0f;
			float sx = 0.0f;
			float sy = 0.0f;
			float px = 0.0f;
			float py = 0.0f;

			/*Bottom left of bounding box*/
			sx = 0.0f * matrix.matrix[0][0] + 0.0f * matrix.matrix[1][0] + matrix.matrix[2][0];
			sy = 0.0f * matrix.matrix[0][1] + 0.0f * matrix.matrix[1][1] + matrix.matrix[2][1];
			float sz = 0.0f * matrix.matrix[0][2] + 0.0f * matrix.matrix[1][2] + matrix.matrix[2][2];
			if (sz != 0)
			{
				sx /= sz;
				sy /= sz;
			}
			px = sx;
			py = sy;
			sx = pen::op::Min(sx, px);
			sy = pen::op::Min(sy, py);
			ex = pen::op::Max(ex, px);
			ey = pen::op::Max(ey, py);

			/*Top right of bounding box*/
			px = width * matrix.matrix[0][0] + height * matrix.matrix[1][0] + matrix.matrix[2][0];
			py = width * matrix.matrix[0][1] + height * matrix.matrix[1][1] + matrix.matrix[2][1];
			float pz = width * matrix.matrix[0][2] + height * matrix.matrix[1][2] + matrix.matrix[2][2];
			if (pz != 0)
			{
				px /= pz;
				py /= pz;
			}
			sx = pen::op::Min(sx, px);
			sy = pen::op::Min(sy, py);
			ex = pen::op::Max(ex, px);
			ey = pen::op::Max(ey, py);

			/*Top left of bounding box*/
			px = 0.0f * matrix.matrix[0][0] + height * matrix.matrix[1][0] + matrix.matrix[2][0];
			py = 0.0f * matrix.matrix[0][1] + height * matrix.matrix[1][1] + matrix.matrix[2][1];
			pz = 0.0f * matrix.matrix[0][2] + height * matrix.matrix[1][2] + matrix.matrix[2][2];
			if (pz != 0)
			{
				px /= pz;
				py /= pz;
			}
			sx = pen::op::Min(sx, px);
			sy = pen::op::Min(sy, py);
			ex = pen::op::Max(ex, px);
			ey = pen::op::Max(ey, py);

			/*Bottom right of bounding box*/
			px = width * matrix.matrix[0][0] + 0.0f * matrix.matrix[1][0] + matrix.matrix[2][0];
			py = width * matrix.matrix[0][1] + 0.0f * matrix.matrix[1][1] + matrix.matrix[2][1];
			pz = width * matrix.matrix[0][2] + 0.0f * matrix.matrix[1][2] + matrix.matrix[2][2];
			if (pz != 0)
			{
				px /= pz;
				py /= pz;
			}
			sx = pen::op::Min(sx, px);
			sy = pen::op::Min(sy, py);
			ex = pen::op::Max(ex, px);
			ey = pen::op::Max(ey, py);

			pen::op::Invert(matrix, invMatrix, invert);

			if (ex < sx) {
				float tempEx = ex;
				ex = sx;
				sx = tempEx;
			}

			if (ey < sy) {
				float tempEy = ey;
				ey = sy;
				sy = tempEy;
			}

			int counter = 0;
			for (float j = sy; j < ey; j++) {
				for (float i = sx; i < ex; i++) {
					float ox = 0.0f;
					float oy = 0.0f;

					/*Uses pixel sprite's inversion matrix to get output coordinate*/
					ox = i * invMatrix.matrix[0][0] + j * invMatrix.matrix[1][0] + invMatrix.matrix[2][0];
					oy = i * invMatrix.matrix[0][1] + j * invMatrix.matrix[1][1] + invMatrix.matrix[2][1];
					float oz = i * invMatrix.matrix[0][2] + j * invMatrix.matrix[1][2] + invMatrix.matrix[2][2];
					if (oz != 0)
					{
						ox /= oz;
						oy /= oz;
					}

					float outY = (displayFunction == nullptr) ? j : (*displayFunction)(width, height, i);

					if ((int)(oy + 0.5f) == height) {
						break;
						break;
					}

					/*This 0.5 is added on for the correct pixel to be selected*/
					pen::Draw(x + (int)i, y + (int)outY, pen::Vec4((float)(data[(int)(((int)(oy + 0.5f)) * (width * 4) + (((int)(ox + 0.5f)) * 4))] / 255.0f) * color.x,
						(float)(data[(int)(((int)(oy + 0.5f)) * (width * 4) + (((int)(ox + 0.5f)) * 4) + 1)] / 255.0f) * color.y,
						(float)(data[(int)(((int)(oy + 0.5f)) * (width * 4) + (((int)(ox + 0.5f)) * 4) + 2)] / 255.0f) * color.z,
						(float)(data[(int)(((int)(oy + 0.5f)) * (width * 4) + (((int)(ox + 0.5f)) * 4) + 3)] / 255.0f) * color.w));
					counter++;
				}
			}
		}
	};

	static void DeleteItem(pen::Item* item) {
		/*Deletes a pixel sprite*/
		delete[] item->data;
		delete item;
	}

	static void Scale(pen::Item* item, float sx, float sy) {
		/*Scale a pixel sprite*/
		if (sx != 0.0f && sy != 0.0f) {
			pen::Mat3x3 scaleMat = pen::Mat3x3(1.0f, true);
			pen::Mat3x3 outputMat = pen::Mat3x3(1.0f, true);
			scaleMat.matrix[0][0] = sx;
			scaleMat.matrix[1][1] = sy;

			/*Multiplies scaling matrix with the item matrix*/
			for (int j = 0; j < 3; j++) {
				for (int i = 0; i < 3; i++) {
					outputMat.matrix[j][i] = scaleMat.matrix[0][i] * item->matrix.matrix[j][0] + scaleMat.matrix[1][i] * item->matrix.matrix[j][1]
						+ scaleMat.matrix[2][i] * item->matrix.matrix[j][2];
				}
			}
			item->matrix = outputMat;
			item->invert = true;
		}
	}

	static void Rotate(pen::Item* item, float theta) {
		/*Rotate a pixel sprite*/
		pen::Mat3x3 rotationMat = pen::Mat3x3(1.0f, true);
		pen::Mat3x3 outputMat = pen::Mat3x3(1.0f, true);

		rotationMat.matrix[0][0] = pen::op::Cos(theta);
		rotationMat.matrix[1][0] = pen::op::Sin(theta);
		rotationMat.matrix[0][1] = -1.0f * pen::op::Sin(theta);
		rotationMat.matrix[1][1] = pen::op::Cos(theta);
		rotationMat.matrix[2][2] = 1.0f;

		/*Multiply rotation matrix with the item matrix*/
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				outputMat.matrix[j][i] = rotationMat.matrix[0][i] * item->matrix.matrix[j][0] + rotationMat.matrix[1][i] * item->matrix.matrix[j][1]
					+ rotationMat.matrix[2][i] * item->matrix.matrix[j][2];
			}
		}
		item->matrix = outputMat;
		item->invert = true;
	}

	static void Translate(pen::Item* item, float tx, float ty) {
		/*Translate a pixel sprite*/
		pen::Mat3x3 translationMat = pen::Mat3x3(1.0f, true);
		pen::Mat3x3 outputMat = pen::Mat3x3(1.0f, true);

		translationMat.matrix[0][0] = 1.0f;
		translationMat.matrix[2][0] = tx;
		translationMat.matrix[1][1] = 1.0f;
		translationMat.matrix[2][1] = ty;
		translationMat.matrix[2][2] = 1.0f;

		/*Multiply translation matrix with the item matrix*/
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				outputMat.matrix[j][i] = translationMat.matrix[0][i] * item->matrix.matrix[j][0] + translationMat.matrix[1][i] * item->matrix.matrix[j][1]
					+ translationMat.matrix[2][i] * item->matrix.matrix[j][2];
			}
		}
		item->matrix = outputMat;
		item->invert = true;
	}

	static void UpdateSprite(unsigned char* data, int x, int y, int width, int height, pen::Vec4 color) {
		/*Updates the buffer of a sprite*/
		if (!(x < 0 || x > width - 1 ||  y < 0 || y > height - 1)) {
			data[y * (width * 4) + (x * 4)] = (unsigned char)(255 * color.x);
			data[y * (width * 4) + (x * 4) + 1] = (unsigned char)(255 * color.y);
			data[y * (width * 4) + (x * 4) + 2] = (unsigned char)(255 * color.z);
			data[y * (width * 4) + (x * 4) + 3] = (unsigned char)(255 * color.w);
		}
	}

	static pen::Item* DrawCircle(int x, int y, int radius, pen::Vec4 color, bool fill = true) {
		/*Create a circle to be loaded in as a sprite*/
		int width = radius * 2 + 2;
		int height = width;
		unsigned char* circle = new unsigned char[width * height * 4];
		std::memset(circle, 0, width * height * 4);

		for (float rad = 0.0f; rad < 2.0f * 3.14159f; rad += 0.01f) {
			int xOffset = radius + radius * pen::op::Sin(rad);
			int yOffset = radius + 1 + radius * pen::op::Cos(rad);

			pen::UpdateSprite(circle, xOffset, yOffset, width, height, color);

			if (fill && rad <= 3.14259f) {
				/*Fill the circle via a vertical reflection*/
				int oppositeXOffset = radius - radius * pen::op::Sin(3.14159f - rad);
				int oppositeYOffset = radius + 1 + radius * pen::op::Cos(3.14159f - rad);
				int leftBound = xOffset < oppositeXOffset ? xOffset : oppositeXOffset;
				int rightBound = xOffset > oppositeXOffset ? xOffset : oppositeXOffset;
				int bottomBound = yOffset < oppositeYOffset ? yOffset : oppositeYOffset;
				int topBound = yOffset > oppositeYOffset ? yOffset : oppositeYOffset;

				for (int fillX = leftBound; fillX < rightBound; fillX++) {
					for (int fillY = bottomBound; fillY < topBound; fillY++) {
						pen::UpdateSprite(circle, fillX, fillY, width, height, color);
					}
				}
			}
		}

		return new pen::Item{ circle, width, height, x, y };
	}

	static pen::Item* DrawLine(int startX, int startY, int endX, int endY, pen::Vec4 color, unsigned char* parentBuffer = nullptr, int parentWidth = 0, int parentHeight = 0) {
		/*Create a line segment to be loaded in as a sprite*/
		int width = (parentBuffer != nullptr) ? parentWidth : pen::op::Sqrt(pen::op::Pow(endX - startX, 2) + pen::op::Pow(endY - startY, 2)) + 1;
		int height = (parentBuffer != nullptr) ? parentHeight : endY - startY + 1;
		float changeY = endY - startY;
		float changeX = endX - startX;
		if (changeX == 0.0f) changeX = 0.0001f;
		float slope = changeY / changeX;

		if (startX > endX) {
			int tempX = startX;
			int tempY = startY;
			startX = endX;
			startY = endY;
			endX = tempX;
			endY = tempY;
		}

		unsigned char* line = (parentBuffer != nullptr) ? parentBuffer : new unsigned char[width * height * 4];
		if(parentBuffer == nullptr) std::memset(line, 0, width * height * 4);

		/*This is for the triangle case where things get translated in that function before the bounds get drawn*/
		int startBoundX = parentBuffer != nullptr ? startX : 0;
		int startBoundY = parentBuffer != nullptr || slope < 0.0f ? startY : 0;

		for (int x = startBoundX; x < width; x++) {
			int y = (slope * x) - (slope * startBoundX) + (startBoundY);
			pen::UpdateSprite(line, x, y, width, height, color);

			/*Slope thresholds to make the line look more connected*/
			if (pen::op::Abs(slope) > 4.4f) {
				pen::UpdateSprite(line, x, y + 1, width, height, color);
				pen::UpdateSprite(line, x, y - 1, width, height, color);
				pen::UpdateSprite(line, x, y + 2, width, height, color);
				pen::UpdateSprite(line, x, y + 3, width, height, color);
			}
			else if (pen::op::Abs(slope) > 3.5f) {
				pen::UpdateSprite(line, x, y + 1, width, height, color);
				pen::UpdateSprite(line, x, y - 1, width, height, color);
				pen::UpdateSprite(line, x, y + 2, width, height, color);
			}
			else if (pen::op::Abs(slope) > 2.4f) {
				pen::UpdateSprite(line, x, y + 1, width, height, color);
				pen::UpdateSprite(line, x, y - 1, width, height, color);
			}
			else if (pen::op::Abs(slope) > 1.1f) {
				pen::UpdateSprite(line, x, y + 1, width, height, color);
				pen::UpdateSprite(line, x, y + 2, width, height, color);
			}
		}

		return new pen::Item{ line, width, height, startX, startY };
	}

	static pen::Item* DrawTri(int aX, int aY, int bX, int bY, int cX, int cY, pen::Vec4 color, bool fill = true) {
		/*Create a triangle to be loaded in as a sprite*/
		int startX = aX;
		int startY = aY;
		if (aX > 0) {
			cX -= aX;
			bX -= aX;
			aX = 0;

			cY -= aY;
			bY -= aY;
			aY = 0;
		}

		if (bY < 0) {
			aY += bY;
			cY += bY;
			bY = 0;
		}
		else if (cY < 0) {
			aY += cY;
			bY += cY;
			cY = 0;
		}

		int height = pen::op::Abs(cY - aY) + 1;
		if (bX < aX) {
			int tempX = aX;
			int tempY = aY;
			aX = bX;
			aY = bY;
			bX = tempX;
			bY = tempY;

			if (aY < bY && bY < cY) {
				int tempX2 = bX;
				int tempY2 = bY;
				bX = cX;
				bY = cY;
				cX = tempX2;
				cY = tempY2;
				height = pen::op::Abs(bY - aY) + 1;
			}
		}	

		int leftBound = (aX < bX&& aX < cX) ? aX : ((bX < aX&& bX < cX) ? bX : cX);
		int rightBound = (aX > bX && aX > cX) ? aX : ((bX > aX && bX > cX) ? bX : cX);
		int width = (rightBound - leftBound + 1);

		unsigned char* tri = new unsigned char[width * height * 4];
		std::memset(tri, 0, width * height * 4);
		pen::DrawLine(aX, aY, bX, bY, color, tri, width, height);
		pen::DrawLine(bX, bY, cX, cY, color, tri, width, height);
		pen::DrawLine(cX, cY, aX, aY, color, tri, width, height);

		if (fill) {
			float changeY = bY - aY;
			float changeX = bX - aX;
			if (changeX == 0.0f) changeX = 0.0001f;
			float slopeA = changeY / changeX;

			changeY = cY - bY;
			changeX = cX - bX;
			if (changeX == 0.0f) changeX = 0.0001f;
			float slopeB = changeY / changeX;

			changeY = aY - cY;
			changeX = aX - cX;
			if (changeX == 0.0f) changeX = 0.0001f;
			float slopeC = changeY / changeX;
			int changePoint = 0;

			if (cX < aX) {
				changePoint = aX;
			}
			else if (cX > bX) {
				changePoint = bX;
			}
			else {
				changePoint = cX;
			}

			for (int x = leftBound; x < rightBound; x++) {
				int funcA = (slopeA * x) - (slopeA * aX) + aY;
				int funcB = (slopeB * x) - (slopeB * bX) + bY;
				int funcC = (slopeC * x) - (slopeC * cX) + cY;

				/*The orientation of the triangle goes from A (bottom-left) to B to C counter-clockwise*/
				int lowBound = 0, highBound = 0, func1 = 0, func2 = 0, func3 = 0;

				if (cY < aY) {
					/*Triangle is upside down*/
					lowBound = (cY < bY ? cY : bY);
					highBound = (aY > bY ? aY : bY);
					func1 = funcA;
					func2 = funcB;
					func3 = funcC;

					if (x <= changePoint) {
						for (int y = lowBound; y < highBound; y++) {
							if (changePoint == cX) {
								if (y <= func1 && y >= func3) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else if (changePoint == aX) {
								if (y <= func3 && y >= func2) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else {
								if (y <= func1 && y >= func3) pen::UpdateSprite(tri, x, y, width, height, color);
							}
						}
					}
					else {
						for (int y = lowBound; y < highBound; y++) {
							if (changePoint == cX) {
								if (y <= func1 && y >= func2) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else if (changePoint == aX) {
								if (y <= func1 && y >= func2) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else {
								if (y <= func2 && y >= func3) pen::UpdateSprite(tri, x, y, width, height, color);
							}
						}
					}
				}
				else {
					lowBound = (aY < bY ? aY : bY);
					highBound = (cY > bY ? cY : bY);
					func1 = funcA;
					func2 = funcB;
					func3 = funcC;

					if (x <= changePoint) {
						for (int y = lowBound; y < highBound; y++) {
							if (changePoint == cX) {
								if (y <= func3 && y >= func1) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else if (changePoint == aX) {
								if (y <= func2 && y >= func3) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else {
								if (y <= func3 && y >= func1) pen::UpdateSprite(tri, x, y, width, height, color);
							}
						}
					}
					else {
						for (int y = lowBound; y < highBound; y++) {
							if (changePoint == cX) {
								if (y <= func2 && y >= func1) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else if (changePoint == aX) {
								if (y <= func2 && y >= func1) pen::UpdateSprite(tri, x, y, width, height, color);
							}
							else {
								if (y <= func3 && y >= func2) pen::UpdateSprite(tri, x, y, width, height, color);
							}
						}
					}
				}
			}
		}
		return new pen::Item{ tri, width, height, startX, startY };
	}

	static pen::Item* DrawRect(int startX, int startY, int length, int height, pen::Vec4 color, bool fill = true) {
		/*Create a rectangle to be loaded in as a sprite*/
		unsigned char* rect = new unsigned char[length * height * 4];
		std::memset(rect, 0, length * height * 4);
		if (fill) {
			for (int x = 0; x < length; x++) {
				for (int y = 0; y < height; y++) {
					pen::UpdateSprite(rect, x, y, length, height, color);
				}
			}
		}
		else {
			for (int x = 0; x < length; x++) {
				pen::UpdateSprite(rect, x, 0, length, height, color);
				pen::UpdateSprite(rect, x, height - 1, length, height, color);

				if (x == 0 || x == length - 1) {
					for (int y = 0; y < height; y++) {
						pen::UpdateSprite(rect, x, y, length, height, color);
					}
				}
			}
		}
		return new pen::Item{ rect, length, height, startX, startY };
	}

	static pen::Item* CreateSprite(int startX, int startY, int width, int height, const std::string& path,
		float spriteTexCoordStartX = 0.0f, float spriteTexCoordStartY = 0.0f, float spriteTexCoordEndX = 1.0f, float spriteTexCoordEndY = 1.0f,
		bool compress = false, float (*userDisplayFunction)(int, int, float) = nullptr) {
		/*Create a sprite for the pixel buffer*/
		unsigned char* spriteData = nullptr;
		int texWidth = 0, texHeight = 0, texBPP = 0;
		pen::State* inst = pen::State::Get();

		pen::Pair<std::string, pen::Sprite>* data = pen::State::Get()->pixelSprites.Find(path);
		if (data != nullptr) {
			spriteData = data->second.data;
			texWidth = data->second.width;
			texHeight = data->second.height;
		}
		else {
#ifndef __PEN_MOBILE__
			stbi_set_flip_vertically_on_load(1);
			bool defaultPath = false;
#ifndef __PEN_CMAKE__
			defaultPath = true;
#endif
			std::string tempPath = (path.find(":") != std::string::npos) ? path : (defaultPath ? pen::State::Get()->penEngineDir + GENERAL_TEXTURE_SOURCE + path
				: (""));

			/*This adds the cmake absolute path for resources*/
			if (tempPath == "") tempPath = tempPath + ROOT_DIR + "res/textures/" + path;

			unsigned char* localBuffer = stbi_load(tempPath.c_str(), &texWidth, &texHeight, &texBPP, 4);
			spriteData = localBuffer;
#else
            std::string mobileFilePath = path;
            if (mobileFilePath[2] == '/' && mobileFilePath[3] == '/') {
                mobileFilePath = mobileFilePath.substr(4);
            }
            else if (mobileFilePath[0] == '/') {
                mobileFilePath = mobileFilePath.substr(1);
            }
            std::string mobileFileName = Asset::ParsePath(mobileFilePath);
	#ifdef __PEN_ANDROID__
			/*When loading from res/drawable mimetypes have to be removed*/
			mobileFileName = mobileFileName.substr(0, mobileFileName.find("."));
			spriteData = AndroidLoadSprite(mobileFileName.c_str(), texWidth, texHeight);
    #else
            /*
             MapMacIOSLoadAsset is called directly so a pen::Asset is not created since the pixel
             buffer uses sprites loaded in memory rather than on the GPU as textures
             */
            spriteData = (unsigned char*) MapMacIOSLoadAsset(mobileFileName.substr(0, mobileFileName.find(".")).c_str(), mobileFileName.substr(mobileFileName.find(".") + 1).c_str());
	#endif
            texBPP = 4;
#endif
			pen::State::Get()->pixelSprites.Insert(path, { path, spriteData, texWidth, texHeight });
		}

		pen::Item* item = nullptr;
		int widthBound = 0;
		int heightBound = 0;
		if (width >= texWidth || height >= texHeight) compress = false;
		if (!compress) {
			widthBound = texWidth * spriteTexCoordEndX;
			heightBound = texHeight * spriteTexCoordEndY;
			int rowOffsetX = 0;
			int rowOffsetY = 0;
			int counter = 0;
			if (spriteTexCoordStartX > 0.0f) rowOffsetX = (int)(texWidth * spriteTexCoordStartX);
			if (spriteTexCoordStartY > 0.0f) rowOffsetY = (int)(texHeight * spriteTexCoordStartY);

			/*Creates a new buffer for the sprite*/
			unsigned char* pixelItemData = new unsigned char[(widthBound - rowOffsetX) * (heightBound - rowOffsetY) * 4];
			item = new pen::Item{ pixelItemData, widthBound - rowOffsetX, heightBound - rowOffsetY, startX, startY, pen::PEN_WHITE, pen::Mat3x3(1.0f, true), pen::Mat3x3(1.0f, true), true, userDisplayFunction };
			for (int j = rowOffsetY; j < heightBound; j++) {
				for (int i = rowOffsetX; i < widthBound; i++) {
					pixelItemData[counter] = spriteData[j * (texWidth * 4) + (i * 4)];
					pixelItemData[counter + 1] = spriteData[j * (texWidth * 4) + (i * 4) + 1];
					pixelItemData[counter + 2] = spriteData[j * (texWidth * 4) + (i * 4) + 2];
					pixelItemData[counter + 3] = spriteData[j * (texWidth * 4) + (i * 4) + 3];
					counter += 4;
				}
			}
		}
		else {
			/*Interpolates this rgba data into a buffer with the specified dimensions*/
			widthBound = (spriteTexCoordStartX != 0.0f ? (int)(spriteTexCoordStartX * width) : width);
			heightBound = (spriteTexCoordStartY != 0.0f ? (int)(spriteTexCoordStartY * height) : height);
			int rowOffset = (int)(texWidth * spriteTexCoordStartX * 4);
			if (rowOffset % 4 != 0) rowOffset = rowOffset - (rowOffset % 4);

			unsigned char* pixelItemData = new unsigned char[widthBound * heightBound * 4];
			item = new pen::Item{ pixelItemData, widthBound, heightBound, startX, startY, pen::PEN_WHITE, pen::Mat3x3(1.0f, true), pen::Mat3x3(1.0f, true), true, userDisplayFunction };
			for (int j = 0; j < heightBound; j++) {
				for (int i = 0; i < widthBound; i++) {
					double px = i * (spriteTexCoordEndX * texWidth) / float(width);
					double py = j * (spriteTexCoordEndY * texHeight) / float(height);
					int pxi = int(px);
					int pyi = int(py);

					pen::Vec4 c00 = pen::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
					pen::Vec4 c10 = pen::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
					pen::Vec4 c01 = pen::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
					pen::Vec4 c11 = pen::Vec4(0.0f, 0.0f, 0.0f, 0.0f);

					c00.x = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4)];
					c00.y = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4) + 1];
					c00.z = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4) + 2];
					c00.w = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4) + 3];

					c10.x = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4 + 1)];
					c10.y = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 1];
					c10.z = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 2];
					c10.w = spriteData[((int)(spriteTexCoordStartY * texHeight) + pyi) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 3];

					c01.x = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4)];
					c01.y = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4) + 1];
					c01.z = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4) + 2];
					c01.w = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4) + 3];

					c11.x = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4 + 1)];
					c11.y = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 1];
					c11.z = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 2];
					c11.w = spriteData[(((int)(spriteTexCoordStartY * texHeight) + pyi) + 1) * (texWidth * 4) + (rowOffset + pxi * 4 + 1) + 3];

					float cx = px - pxi;
					float cy = py - pyi;

					float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
					a = (1 - cx) * (1 - cy);
					b = cx * (1 - cy);
					c = (1 - cx) * cy;
					d = cx * cy;
					pen::Vec4 aRes = pen::Vec4(c00.x * a, c00.y * a, c00.z * a, c00.w * a);
					pen::Vec4 bRes = pen::Vec4(c10.x * b, c10.y * b, c10.z * b, c10.w * b);
					pen::Vec4 cRes = pen::Vec4(c01.x * c, c01.y * c, c01.z * c, c01.w * c);
					pen::Vec4 dRes = pen::Vec4(c11.x * d, c11.y * d, c11.z * d, c11.w * d);
					pen::Vec4 resColor = aRes + bRes + cRes + dRes;

					if (texBPP == 3) resColor.w = 255.0f;

					pixelItemData[j * (widthBound * 4) + (i * 4)] = (unsigned char)(resColor.x);
					pixelItemData[j * (widthBound * 4) + (i * 4 + 1)] = (unsigned char)(resColor.y);
					pixelItemData[j * (widthBound * 4) + (i * 4 + 2)] = (unsigned char)(resColor.z);
					pixelItemData[j * (widthBound * 4) + (i * 4 + 3)] = (unsigned char)(resColor.w);
				}
			}
		}

		if (widthBound != width || heightBound != height) {
			pen::Scale(item, ((float)width / (float)widthBound), ((float)height / (float)heightBound));
		}

		return item;
	}

	static void Animate(pen::Item* item, const std::string& path,
		float spriteTexCoordStartX = 0.0f, float spriteTexCoordStartY = 0.0f, float spriteTexCoordEndX = 1.0f, float spriteTexCoordEndY = 1.0f) {
		/*Swaps animations for an item*/
		pen::Item* newItem = pen::CreateSprite(item->x, item->y, item->width, item->height, path, spriteTexCoordStartX, spriteTexCoordStartY, spriteTexCoordEndX, spriteTexCoordEndY);
		newItem->color = item->color;
		newItem->matrix = item->matrix;
		newItem->invMatrix = item->invMatrix;
		newItem->invert = item->invert;
		newItem->displayFunction = item->displayFunction;
		pen::DeleteItem(item);
		item = newItem;
	}
}
