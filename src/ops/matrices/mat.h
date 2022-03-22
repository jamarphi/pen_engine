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
#include "../operations/operations.h"
namespace pen {
	class Mat {
	public:
		float** matrix;
		int width;
		int height;
	public:
		Mat();
		Mat(float data, int userWidth, int userHeight);
		Mat(float* data, int userWidth, int userHeight);
		Mat(float** data, int userWidth, int userHeight);
		~Mat();
		static void Delete(Mat* a);

		Mat T();
		Mat Sqrt();
		float* operator[](int idx);
		void operator|=(const Mat& b);
		void operator|=(const Mat&& b);
		void operator|=(const float& b);
		void operator|=(const float&& b);
		Mat* Ptr();
		bool operator==(const Mat& b);
		bool operator==(const Mat&& b);

		Mat operator+(const Mat& b);
		Mat operator+(const Mat&& b);
		void operator+=(const Mat& b);
		void operator+=(const Mat&& b);
		Mat operator+(const float& b);
		Mat operator+(const float&& b);
		void operator+=(const float& b);
		void operator+=(const float&& b);
		Mat Sum(bool row = true);

		Mat operator-(const Mat& b);
		Mat operator-(const Mat&& b);
		void operator-=(const Mat& b);
		void operator-=(const Mat&& b);
		Mat operator-(const float& b);
		Mat operator-(const float&& b);
		void operator-=(const float& b);
		void operator-=(const float&& b);

		Mat operator*(const Mat& b);
		Mat operator*(const Mat&& b);
		void operator*=(const Mat& b);
		void operator*=(const Mat&& b);
		Mat operator*(const float& b);
		Mat operator*(const float&& b);
		void operator*=(const float& b);
		void operator*=(const float&& b);
		Mat Mul(const Mat& b);
		Mat Mul(const Mat&& b);
		Mat Dot(const Mat& b);
		Mat Dot(const Mat&& b);

		Mat operator/(const Mat& b);
		Mat operator/(const Mat&& b);
		void operator/=(const Mat& b);
		void operator/=(const Mat&& b);
		Mat operator/(const float& b);
		Mat operator/(const float&& b);
		void operator/=(const float& b);
		void operator/=(const float&& b);

		Mat Exp();

		static Mat Max(const Mat& a, const Mat& b);
		static Mat Max(const Mat&& a, const Mat& b);
		static Mat Max(const Mat& a, const Mat&& b);
		static Mat Max(const Mat&& a, const Mat&& b);
		static Mat Max(const Mat& a, const float& b);
		static Mat Max(const Mat&& a, const float& b);
		static Mat Max(const Mat& a, const float&& b);
		static Mat Max(const Mat&& a, const float&& b);
		static Mat Min(const Mat& a, const Mat& b);
		static Mat Min(const Mat&& a, const Mat& b);
		static Mat Min(const Mat& a, const Mat&& b);
		static Mat Min(const Mat&& a, const Mat&& b);
		static Mat Min(const Mat& a, const float& b);
		static Mat Min(const Mat&& a, const float& b);
		static Mat Min(const Mat& a, const float&& b);
		static Mat Min(const Mat&& a, const float&& b);
	};
}