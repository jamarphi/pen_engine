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
#include "free_agent.h"

namespace pen {
	namespace ai {
		FreeAgent::FreeAgent() {
			id = pen::ai::Agent::nextId;
			pen::ai::Agent::nextId++;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.0001f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			shift = nullptr;
			discountValue = 0.9f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			numLayers = 0;
			weights = nullptr;
			mHatW = nullptr;
			mHatB = nullptr;
			vHatW = pen::Mat();
			vHatB = pen::Mat();
		}

		FreeAgent::FreeAgent(const std::string& path, pen::ai::Action** userActions, int numActions) {
			/*Load in an agent with the actions already defined*/
			id = -1;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.0001f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			shift = nullptr;
			discountValue = 0.9f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			numLayers = 0;
			weights = nullptr;
			mHatW = nullptr;
			mHatB = nullptr;
			vHatW = pen::Mat();
			vHatB = pen::Mat();
			Load(path, userActions, numActions);
		}

		void FreeAgent::Init(pen::ai::Weight* userWeights, int userNumLayers, long userStateNum, int userNumEpisodes, float userEpsilon, float userStepSize) {
			/*Each agent should have its own states unless you want an agent to share learning with another agent*/
			epsilon = userEpsilon;
			stepSize = userStepSize;
			numStates = userStateNum;
			states = nullptr;
			initialState = nullptr;
			currentState = nullptr;
			numEpisodes = userNumEpisodes;
			numLayers = userNumLayers;
			weights = userWeights;

			mHatW = new pen::Mat[userNumLayers - 1];
			mHatB = new pen::Mat[userNumLayers - 1];
			for (int i = 0; i < userNumLayers - 1; i++) {
				if (i < userNumLayers - 2) {
					mHatW[i] = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, userWeights[i].numCurrLayerNodes);
					mHatB[i] = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, 1);
				}
				else {
					vHatW = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, userWeights[i].numCurrLayerNodes);
					vHatB = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, 1);
				}
			}

			lastAction = ChooseAction(currentState);
		}

		void FreeAgent::Step() {
			/*Take a step through the environment*/
			pen::ai::AIState* s = states[(int)pen::op::Max(0.0f, Rand(numStates) - 1)];
			pen::Mat lastStateV = OneHot((int)currentState->id);
			pen::Mat lastValue = ComputeOutput(&lastStateV, weights, numLayers);
			pen::Mat delta = pen::Mat();
			if (s->terminal) {
				delta = (lastValue - s->reward) * -1.0f;
			}
			else {
				pen::Mat currentStateV = OneHot((int)s->id);
				pen::Mat currentValue = ComputeOutput(&currentStateV, weights, numLayers);
				delta = currentValue * discountValue - lastValue + s->reward;
				lastAction = ChooseAction(s);
			}

			for (int i = 0; i < numLayers; i++) {
				*weights[i].weightGrads = delta * *weights[i].weightGrads;
			}

			UpdateWeights(weights, numLayers);
			currentState = s;
		}

		Weight* FreeAgent::GetWeights() {
			/*Return the weights for this neural network*/
			return weights;
		}

		int FreeAgent::GetLayers() {
			/*Return the number of layers for this neural network*/
			return numLayers;
		}

		pen::Mat FreeAgent::ComputeOutput(pen::Mat* input, Weight* weights, int numLayers) {
			/*Compute the output given the weights for the neural network*/
			pen::Mat activation = pen::Mat();
			pen::Mat layerVal = pen::Mat();
			float threshold = 0.0f;
			for (int i = 0; i < numLayers - 1; i++) {
				pen::Mat weightMatrix = pen::Mat();
				pen::Mat biasMatrix = pen::Mat();
				weightMatrix |= *weights[i].weights;
				biasMatrix = *weights[i].bias;
				if (i == 0) {					
					layerVal |= (input->Mul(weightMatrix.T()) + biasMatrix);
				}
				else {
					layerVal |= activation.Mul(weightMatrix.T() + biasMatrix);
				}
				activation |= pen::Mat::Max(layerVal, threshold);

				/*Compute gradients*/
				if (i < numLayers - 2) {
					pen::Mat nextWeightMatrix = pen::Mat();
					nextWeightMatrix |= *weights[i + 1].weights;
					pen::Mat thresholdMatrix = pen::Mat();
					pen::Mat weightsByThreshold = pen::Mat();
					thresholdMatrix.matrix = new float* [activation.height];
					for (int j = 0; j < activation.height; j++) {
						float* row = new float[activation.width];
						for (int k = 0; k < activation.width; k++) {
							row[k] = (activation[j][k] > 0.0f ? 1.0f : 0.0f);
						}
						thresholdMatrix.matrix[j] = row;
					}
				
					weightsByThreshold |= nextWeightMatrix.T() * thresholdMatrix;
					*weights[i].weightGrads |= activation.Mul(weightsByThreshold);
					*weights[i].biasGrads |= weightsByThreshold;
				}
				else {
					*weights[i].weightGrads |= activation.T();
					float* temp = new float[1]{ 1.0f };
					*weights[i].biasGrads |= pen::Mat(temp, 1, 1);
				}
			}

			return layerVal;
		}

		void FreeAgent::UpdateWeights(Weight* weights, int numLayers) {
			/*Update the weights with the step size and gradients*/
			for (int i = 0; i < numLayers - 1; i++) {
				Weight* weight = &weights[i];
				if (i < numLayers - 2) {
					mHatW[i] |= (mHatW[i] * betaM + *weight->weightGrads * (1 - betaM)) / (1 - betaM);
					mHatB[i] |= (mHatB[i] * betaM + *weight->biasGrads * (1 - betaM)) / (1 - betaM);
					*weight->weights += (mHatW[i] * stepSize / (vHatW.Sqrt() + epsilon));
					*weight->bias += (mHatB[i] * stepSize / (vHatB.Sqrt() + epsilon));
				}
				else {
					for (int j = 0; j < weight->weightGrads->height; j++) {
						vHatW |= (vHatW * betaV + *weight->weightGrads * *weight->weightGrads * (1 - betaV)) / (1 - betaV);
						vHatB |= (vHatB * betaM + *weight->biasGrads * *weight->biasGrads * (1 - betaV)) / (1 - betaV);
						*weight->weights = (*weight->weights * stepSize / (vHatW.Sqrt() + epsilon));
						*weight->bias = (*weight->bias * stepSize / (vHatB.Sqrt() + epsilon));
					}
				}
			}
		}

		pen::Mat FreeAgent::OneHot(int state) {
			/*Creates a one hot vector representation of the given state*/
			float* values = new float[numStates] {0.0f};
			values[state - 1] = 1.0f;
			return pen::Mat(values, numStates, 1);
		}

		void FreeAgent::Save(const std::string& path) {
			/*Save a free agent model*/
			std::string tempPath = (path.find(".farlpen") != std::string::npos ? path : path + ".farlpen");
			std::ofstream modelFile;
			std::string input = "";
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
				bool writing = true;

				/*Each weight has data for a specific layer*/
				bool header = true;

				while (writing) {
					input = "";
					/*Write a weight*/
					if (header) {
						/*Write the header information before weights*/
						input += ("epsilon:" + std::to_string(epsilon) + "\ndiscount value:" + std::to_string(discountValue) + "\nstep size:" + std::to_string(stepSize)
							+ "\ninitial state id:" + std::to_string(initialState->id)
							+ "\nnum episodes:" + std::to_string(numEpisodes) + "\nnum layers:" + std::to_string(numLayers)
							+ "\n[m hat weights]^[m hat biases]^[v hat weights]^[v hat biases]");
						modelFile << input;

						for (int i = 0; i < numLayers - 1; i++) {
							input = "[";
							if (i < numLayers - 2) {
								input += (FormatMatrix(&mHatW[i], ',') + " ]^[" + FormatMatrix(&mHatB[i], ',') + "]^");
							}
							else {
								input += (FormatMatrix(&vHatW, ',') + " ]^[" + FormatMatrix(&vHatB, ',') + "]");
							}
						}
						modelFile << input;
						input = "\n====\nnum prev layer nodes/num current layer nodes/[weights]/[weight gradients]/[bias]/[bias gradients]/length\n";
						modelFile << input;
						header = false;
					}
					else {
						for (int i = 0; i < numLayers; i++) {
							input = "";
							input += (std::to_string(weights[i].numPrevLayerNodes) + "/" + std::to_string(weights[i].numCurrLayerNodes)
								+ "/[" + FormatMatrix(weights[i].weights, ',') + "]/["
								+ FormatMatrix(weights[i].weightGrads, ',') + "]/[" + FormatMatrix(weights[i].bias, ',') + "]/[" + FormatMatrix(weights[i].biasGrads, ',') + "]");

							modelFile << (input + "\n----\n");
						}

						writing = false;
						modelFile << "\n----\n";
					}
				}
				modelFile.close();
			}
		}

		void FreeAgent::Load(const std::string& path, pen::ai::Action** userActions, int numActions) {
			/*Load a free agent model*/
			std::string tempPath = (path.find(".farlpen") != std::string::npos ? path : path + ".farlpen");
			std::ifstream modelFile;
			std::string input = "";
			pen::ai::Weight* weight = nullptr;
			std::string matrixStr = "";
			char initialStateId = 0x00000000;
			bool header = true;
			int counter = 0;
			std::vector<pen::ai::Weight*> weightVector;
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
				while (!modelFile.eof()) {
					input = "";

					/*Load a state*/
					if (header) {
						if (counter < 6) {
							/*Grab the header meta data for the agent*/
							std::getline(modelFile, input);
							if (counter == 0) epsilon = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 1) discountValue = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 2) stepSize = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 3) initialStateId = pen::ai::Agent::Split(input, ':', 1)[0];
							if (counter == 4) numEpisodes = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 5) numLayers = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							counter++;
						}
						else {
							std::getline(modelFile, input);
							mHatW = new pen::Mat[numLayers - 1];
							mHatB = new pen::Mat[numLayers - 1];

							for (int i = 0; i < numLayers - 1; i++) {
								if (i < numLayers - 2) {
									matrixStr = Agent::Split(input, '^', i * 2);
									matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
									mHatW[i] = *ParseMatrix(matrixStr);
									matrixStr = Agent::Split(input, '^', i * 2 + 1);
									matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
									mHatB[i] = *ParseMatrix(matrixStr);
								}
								else {
									matrixStr = Agent::Split(input, '^', i * 2);
									matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
									vHatW = *ParseMatrix(matrixStr);
									matrixStr = Agent::Split(input, '^', i * 2 + 1);
									matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
									vHatB = *ParseMatrix(matrixStr);
								}
							}

							header = false;
						}
					}
					else {
						std::getline(modelFile, input);

						if (input == "----" || input == "====") continue;

						weight = new pen::ai::Weight();
						weight->numPrevLayerNodes = std::stoi(pen::ai::Agent::Split(input, '/', 0));
						weight->numCurrLayerNodes = std::stoi(pen::ai::Agent::Split(input, '/', 1));
						matrixStr = Agent::Split(input, '/', 2);
						matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
						weight->weights = ParseMatrix(matrixStr);
						matrixStr = Agent::Split(input, '/', 3);
						matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
						weight->weightGrads = ParseMatrix(matrixStr);
						matrixStr = Agent::Split(input, '/', 4);
						matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
						weight->bias = ParseMatrix(matrixStr);
						matrixStr = Agent::Split(input, '/', 5);
						matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
						weight->biasGrads = ParseMatrix(matrixStr);

						weightVector.push_back(weight);
					}

				}
				modelFile.close();

				/*Set the weights in the agent*/
				int weightCount = weightVector.size();
				int offset = 0;
				weights = new pen::ai::Weight[weightCount];
				for (int i = 0; i < weightCount; i++) {
					weights[i] = *weightVector[i];
				}

				initialState = FindState(this, initialStateId);
				currentState = initialState;

				lastAction = ChooseAction(initialState);
			}
		}

		pen::Mat* FreeAgent::ParseMatrix(std::string input) {
			/*Parse the matrices from strings*/
			std::vector<std::string> listItems;
			bool splitting = true;
			std::string tempStr = "";
			int counter = 0;
			int width = 0;
			int height = 0;
			while (splitting) {
				tempStr = Agent::Split(input, ',', counter);
				if (tempStr != "" && counter > 1) {
					listItems.push_back(tempStr);
				}
				else if (counter < 2) {
					if (counter == 0) width = std::stoi(tempStr);
					if (counter == 1) height = std::stoi(tempStr);
				}
				else {
					splitting = false;
				}
			}

			pen::Mat* mat = new pen::Mat();
			mat->width = width;
			mat->height = height;
			mat->matrix = new float* [height];
			for (int j = 0; j < height; j++) {
				float* row = new float[width];
				for (int i = 0; i < width; i++) {
					row[i] = std::stof(listItems[j * width + i]);
				}
				mat->matrix[j] = row;
			}
			return mat;
		}

		std::string FreeAgent::FormatMatrix(pen::Mat* mat, char character) {
			/*Format the matrix into a string*/
			std::string input = "";

			input += (std::to_string(mat->width) + character + std::to_string(mat->height) + character);

			for (int j = 0; j < mat->height; j++) {
				for (int i = 0; i < mat->width; i++) {
					input += (std::to_string(mat->matrix[j][i]) + character);
				}
			}
			return input.substr(0, input.length() - 1);
		}
	}
}