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
#include "weight.h"

namespace pen {
	namespace ai {
		Weight::Weight() {
			weights = nullptr;
			weightGrads = nullptr;
			bias = nullptr;
			biasGrads = nullptr;
			length = 0;
		}

		Weight::Weight(int numLayerNodes) {
			length = numLayerNodes;
			weights = new pen::Mat();
			weights->matrix = new float* [1];
			weights->matrix[0] = new float[numLayerNodes];
			weightGrads = new pen::Mat();
			weightGrads->matrix = new float* [1];
			weightGrads->matrix[0] = new float[numLayerNodes];
			bias = new pen::Mat();
			bias->matrix = new float* [1];
			bias->matrix[0] = new float[numLayerNodes] {0.0f};
			biasGrads = new pen::Mat();
			biasGrads->matrix = new float* [1];
			biasGrads->matrix[0] = new float[numLayerNodes] {0.0f};

			for (int i = 0; i < numLayerNodes; i++) {
				/*Randomize the weights and biases between 0 and 1*/
				weights->matrix[0][i] = (float)std::rand() / RAND_MAX;
				bias->matrix[0][i] = (float)std::rand() / RAND_MAX;
			}
		}

		Weight::~Weight() {
			if (weights != nullptr) delete[] weights;
			if (bias != nullptr) delete[] bias;
		}
	}
}