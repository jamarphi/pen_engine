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
#include "mat.h"

namespace pen {
	Mat::Mat() {
		matrix = nullptr;
		width = 0;
		height = 0;
	}

	Mat::Mat(float data, int userWidth, int userHeight) {
		width = userWidth;
		height = userHeight;
		matrix = new float* [userHeight];

		for (int j = 0; j < userHeight; j++) {
			float* row = new float[userWidth];
			for (int i = 0; i < userWidth; i++) {
				row[i] = data;
			}
			matrix[j] = row;
		}
	}

	Mat::Mat(float* data, int userWidth, int userHeight) {
		width = userWidth;
		height = userHeight;
		matrix = new float* [userHeight];

		for (int j = 0; j < userHeight; j++) {
			float* row = new float[userWidth];
			for (int i = 0; i < userWidth; i++) {
				row[i] = data[j * userWidth + i];
			}
			matrix[j] = row;
		}
	}

	Mat::Mat(float** data, int userWidth, int userHeight) {
		width = userWidth;
		height = userHeight;
		matrix = new float*[userHeight];

		for (int j = 0; j < userHeight; j++) {
			float* row = new float[userWidth];
			for (int i = 0; i < userWidth; i++) {
				row[i] = data[j][i];
			}
			matrix[j] = row;
		}
	}

	Mat::~Mat() {
		if (matrix != nullptr) {
			for (int i = 0; i < height; i++) {
				delete[] matrix[i];
			}
			delete[] matrix;
			matrix = nullptr;
		}
		width = 0;
		height = 0;
	}

	void Mat::Delete(Mat* a) {
		if (a->matrix != nullptr) {
			for (int i = 0; i < a->height; i++) {
				delete[] a->matrix[i];
			}
			delete[] a->matrix;
			a->matrix = nullptr;
		}
		delete a;
	}

	Mat Mat::T() {
		/*Returns a transpose of the matrix*/
		Mat mat = Mat();
		mat.width = height;
		mat.height = width;
		mat.matrix = new float* [width];
		for (int j = 0; j < mat.height; j++) {
			float* row = new float[mat.width];
			for (int i = 0; i < mat.width; i++) {
				row[i] = matrix[i][j];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Sqrt() {
		/*Returns a square root of the matrix*/
		Mat mat = Mat();
		mat.width = height;
		mat.height = width;
		mat.matrix = new float* [width];
		for (int j = 0; j < mat.height; j++) {
			float* row = new float[mat.width];
			for (int i = 0; i < mat.width; i++) {
				row[i] = pen::op::Sqrt(matrix[i][j]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	float* Mat::operator[](int idx) {
		/*Returns a row within the matrix*/
		return matrix[idx];
	}

	void Mat::operator|=(const Mat& b) {
		/*Sets a given matrix equal to another matrix on the stack*/
		if (matrix != nullptr) {
			for (int i = 0; i < height; i++) {
				delete[] matrix[i];
			}
			delete[] matrix;
		}

		width = b.width;
		height = b.height;
		matrix = new float* [b.height];
		for (int j = 0; j < b.height; j++) {
			float* row = new float[b.width];
			for (int i = 0; i < b.width; i++) {
				row[i] = b.matrix[j][i];
			}
			matrix[j] = row;
		}
	}

	void Mat::operator|=(const Mat&& b) {
		/*Sets a given matrix equal to another matrix on the stack*/
		if (matrix != nullptr) {
			for (int i = 0; i < height; i++) {
				delete[] matrix[i];
			}
			delete[] matrix;
		}

		width = b.width;
		height = b.height;
		matrix = new float* [b.height];
		for (int j = 0; j < b.height; j++) {
			float* row = new float[b.width];
			for (int i = 0; i < b.width; i++) {
				row[i] = b.matrix[j][i];
			}
			matrix[j] = row;
		}
	}

	void Mat::operator|=(const float& b) {
		/*Sets all elements in a given matrix equal to a scalar value*/
		if (matrix != nullptr) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					matrix[j][i] = b;
				}
			}
		}
	}

	void Mat::operator|=(const float&& b) {
		/*Sets all elements in a given matrix equal to a scalar value*/
		if (matrix != nullptr) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					matrix[j][i] = b;
				}
			}
		}
	}

	Mat* Mat::Ptr() {
		/*Returns the pointer to a heap copy of a given matrix*/
		Mat* mat = new Mat();
		mat->width = width;
		mat->height = height;
		mat->matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] =  matrix[j][i];
			}
			mat->matrix[j] = row;
		}
		return mat;
	}

	bool Mat::operator==(const Mat& b) {
		/*Checks if two matrices are equal*/
		bool isEqual = false;
		if (width == b.width && height == b.height) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					isEqual = matrix[j][i] == matrix[j][i];
					if (!isEqual) {
						break;
						break;
					}
				}
			}
		}
		return isEqual;
	}

	bool Mat::operator==(const Mat&& b) {
		/*Checks if two matrices are equal*/
		bool isEqual = false;
		if (width == b.width && height == b.height) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					isEqual = matrix[j][i] == matrix[j][i];
					if (!isEqual) {
						break;
						break;
					}
				}
			}
		}
		return isEqual;
	}

	Mat Mat::operator+(const Mat& b) {
		/*Broadcasted addition between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] + b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator+(const Mat&& b) {
		/*Broadcasted addition between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] + b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator+=(const Mat& b) {
		/*In-place broadcasted addition between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] + b.matrix[j][i];
			}
		}
	}

	void Mat::operator+=(const Mat&& b) {
		/*In-place broadcasted addition between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] + b.matrix[j][i];
			}
		}
	}

	Mat Mat::operator+(const float& b) {
		/*Broadcasted addition with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] + b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator+(const float&& b) {
		/*Broadcasted addition with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] + b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator+=(const float& b) {
		/*In-place broadcasted addition with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] + b;
			}
		}
	}

	void Mat::operator+=(const float&& b) {
		/*In-place broadcasted addition with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] + b;
			}
		}
	}

	Mat Mat::Sum(bool row) {
		/*Returns a matrix of the sum from a given matrix*/
		if (row) {
			Mat mat = Mat(0.0f, 1, height);
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					mat.matrix[j][0] += matrix[j][i];
				}
			}
			return mat;
		}
		else {
			Mat mat = Mat(0.0f, width, 1);
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					mat.matrix[0][i] += matrix[j][i];
				}
			}
			return mat;
		}
	}

	Mat Mat::operator-(const Mat& b) {
		/*Broadcasted subtraction between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] - b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator-(const Mat&& b) {
		/*Broadcasted subtraction between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] - b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator-=(const Mat& b) {
		/*In-place broadcasted subtraction between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] - b.matrix[j][i];
			}
		}
	}

	void Mat::operator-=(const Mat&& b) {
		/*In-place broadcasted subtraction between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] - b.matrix[j][i];
			}
		}
	}

	Mat Mat::operator-(const float& b) {
		/*Broadcasted subtraction with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] - b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator-(const float&& b) {
		/*Broadcasted subtraction with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] - b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator-=(const float& b) {
		/*In-place broadcasted subtraction with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] - b;
			}
		}
	}

	void Mat::operator-=(const float&& b) {
		/*In-place broadcasted subtraction with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] - b;
			}
		}
	}

	Mat Mat::operator*(const Mat& b) {
		/*Broadcasted multiplication between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] * b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator*(const Mat&& b) {
		/*Broadcasted multiplication between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] * b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator*=(const Mat& b) {
		/*In-place broadcasted multiplication between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] * b.matrix[j][i];
			}
		}
	}

	void Mat::operator*=(const Mat&& b) {
		/*In-place broadcasted multiplication between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] * b.matrix[j][i];
			}
		}
	}

	Mat Mat::operator*(const float& b) {
		/*Broadcasted multiplication with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] * b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator*(const float&& b) {
		/*Broadcasted multiplication with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] * b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator*=(const float& b) {
		/*In-place broadcasted multiplication with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] * b;
			}
		}
	}

	void Mat::operator*=(const float&& b) {
		/*In-place broadcasted multiplication with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] * b;
			}
		}
	}

	Mat Mat::Mul(const Mat& b) {
		/*Matrix multiplication*/
		if (width == b.height) {
			Mat mat = Mat();
			mat.width = width;
			mat.height = height;
			mat.matrix = new float* [height];
			for (int i = 0; i < height; i++) {
				float* row = new float[b.height];
				for (int j = 0; j < b.height; j++) {
					row[j] = matrix[i][j] * b.matrix[j][i];
				}
				mat.matrix[i] = row;
			}
			return mat;
		}
		return Mat();
	}

	Mat Mat::Mul(const Mat&& b) {
		/*Matrix multiplication*/
		if (width == b.height) {
			Mat mat = Mat();
			mat.width = width;
			mat.height = height;
			mat.matrix = new float* [height];
			for (int i = 0; i < height; i++) {
				float* row = new float[b.height];
				for (int j = 0; j < b.height; j++) {
					row[j] = matrix[i][j] * b.matrix[j][i];
				}
				mat.matrix[i] = row;
			}
			return mat;
		}
		return Mat();
	}

	Mat Mat::Dot(const Mat& b) {
		/*Dot multiplication*/
		if (width == b.height) {
			Mat mat = Mat();
			mat.width = 1;
			mat.height = width;
			mat.matrix = new float* [width];
			for (int i = 0; i < width; i++) {
				float* row = new float[1];
				row[0] = 0.0f;
				for (int j = 0; j < b.height; j++) {
					row[0] += (matrix[i][j] * b.matrix[j][i]);
				}
				mat.matrix[i] = row;
			}
			return mat;
		}
		return Mat();
	}

	Mat Mat::Dot(const Mat&& b) {
		/*Dot multiplication*/
		if (width == b.height) {
			Mat mat = Mat();
			mat.width = 1;
			mat.height = width;
			mat.matrix = new float* [width];
			for (int i = 0; i < width; i++) {
				float* row = new float[1];
				row[0] = 0.0f;
				for (int j = 0; j < b.height; j++) {
					row[0] += (matrix[i][j] * b.matrix[j][i]);
				}
				mat.matrix[i] = row;
			}
			return mat;
		}
		return Mat();
	}

	Mat Mat::operator/(const Mat& b) {
		/*Broadcasted division between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] / b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator/(const Mat&& b) {
		/*Broadcasted division between two matrices*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] / b.matrix[j][i];
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator/=(const Mat& b) {
		/*In-place broadcasted division between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] / b.matrix[j][i];
			}
		}
	}

	void Mat::operator/=(const Mat&& b) {
		/*In-place broadcasted division between two matrices*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] / b.matrix[j][i];
			}
		}
	}

	Mat Mat::operator/(const float& b) {
		/*Broadcasted division with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] / b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::operator/(const float&& b) {
		/*Broadcasted division with a scalar value*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = matrix[j][i] / b;
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	void Mat::operator/=(const float& b) {
		/*In-place broadcasted division with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] / b;
			}
		}
	}

	void Mat::operator/=(const float&& b) {
		/*In-place broadcasted division with a scalar value*/
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				matrix[j][i] = matrix[j][i] / b;
			}
		}
	}

	Mat Mat::Exp() {
		/*Returns an exponential matrix of the given matrix*/
		Mat mat = Mat();
		mat.width = width;
		mat.height = height;
		mat.matrix = new float* [height];
		for (int j = 0; j < height; j++) {
			float* row = new float[width];
			for (int i = 0; i < width; i++) {
				row[i] = pen::op::Pow(2.71828f, matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat& a, const Mat& b) {
		/*Returns a matrix with the greater elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat&& a, const Mat& b) {
		/*Returns a matrix with the greater elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat& a, const Mat&& b) {
		/*Returns a matrix with the greater elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat&& a, const Mat&& b) {
		/*Returns a matrix with the greater elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat& a, const float& b) {
		/*Returns a matrix with the greater elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat&& a, const float& b) {
		/*Returns a matrix with the greater elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat& a, const float&& b) {
		/*Returns a matrix with the greater elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Max(const Mat&& a, const float&& b) {
		/*Returns a matrix with the greater elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Max(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat& a, const Mat& b) {
		/*Returns a matrix with the lesser elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat&& a, const Mat& b) {
		/*Returns a matrix with the lesser elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat& a, const Mat&& b) {
		/*Returns a matrix with the lesser elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat&& a, const Mat&& b) {
		/*Returns a matrix with the lesser elements between two matrices*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b.matrix[j][i]);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat& a, const float& b) {
		/*Returns a matrix with the lesser elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat&& a, const float& b) {
		/*Returns a matrix with the lesser elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat& a, const float&& b) {
		/*Returns a matrix with the lesser elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}

	Mat Mat::Min(const Mat&& a, const float&& b) {
		/*Returns a matrix with the lesser elements between a matrix and a scalar value*/
		Mat mat = Mat();
		mat.width = a.width;
		mat.height = a.height;
		mat.matrix = new float* [a.height];
		for (int j = 0; j < a.height; j++) {
			float* row = new float[a.width];
			for (int i = 0; i < a.width; i++) {
				row[i] = pen::op::Min(a.matrix[j][i], b);
			}
			mat.matrix[j] = row;
		}
		return mat;
	}
}