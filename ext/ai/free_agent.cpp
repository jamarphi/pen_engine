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
			discountValue = 0.9f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			numLayers = 0;
			betaM = 0.9f;
			betaV = 0.99f;
			preferenceFactor = 0.001f;
			weights = nullptr;
			mHat = nullptr;
			vHat = nullptr;
			replayBuffer = nullptr;
		}

		FreeAgent::FreeAgent(const std::string& path, pen::ai::Action** userActions, int numActions) {
			/*Load in an agent from file*/
			id = -1;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.0001f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			discountValue = 0.9f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			numLayers = 0;
			betaM = 0.9f;
			betaV = 0.99f;
			preferenceFactor = 0.001f;
			weights = nullptr;
			mHat = nullptr;
			vHat = nullptr;
			Load(path, userActions, numActions);
		}

		FreeAgent::~FreeAgent() {
			if (mHat != nullptr) {
				pen::Mat::Delete(mHat);
				mHat = nullptr;
			}

			if (vHat != nullptr) {
				pen::Mat::Delete(vHat);
				vHat = nullptr;
			}

			if (replayBuffer != nullptr) {
				delete replayBuffer;
				replayBuffer = nullptr;
			}
		}

		void FreeAgent::Init(pen::Mat* userInitialState) {
			/*Initialize the environment for the free agent*/
			prevState = *userInitialState;
			prevAction = ChoosePolicy(userInitialState);
			replayBuffer = new pen::ai::ReplayBuffer();
		}

		void FreeAgent::Init(pen::ai::Action** userActions, pen::ai::Weight* userWeights, pen::Mat* userInitialState, int userNumLayers, long userNumActions, int userNumEpisodes, float userEpsilon, float userStepSize) {
			/*Initialize the environment for the free agent*/
			epsilon = userEpsilon;
			stepSize = userStepSize;
			numActions = userNumActions;
			states = nullptr;
			initialState = nullptr;
			currentState = nullptr;
			numEpisodes = userNumEpisodes;
			numLayers = userNumLayers;
			actions = userActions;
			weights = userWeights;

			mHat = new pen::Mat[userNumLayers - 1];
			vHat = new pen::Mat[userNumLayers - 1];
			for (int i = 0; i < userNumLayers - 1; i++) {
				mHat[i] = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, userWeights[i].numCurrLayerNodes);
				vHat[i] = pen::Mat(0.0f, userWeights[i + 1].numCurrLayerNodes, userWeights[i].numCurrLayerNodes);
			}

			prevState = *userInitialState;
			prevAction = ChoosePolicy(userInitialState);
			replayBuffer = new pen::ai::ReplayBuffer();
		}

		void FreeAgent::Step(pen::Mat state, float reward) {
			/*Take a step through the environment, the state is the vector of parameters that affect learning as a column vector*/
			totalReward += reward;
			pen::ai::Action* action = ChoosePolicy(&state);
			replayBuffer->Insert({ prevState, prevAction, reward, prevState.matrix[0][0] == 0.0f ? state : pen::Mat(0.0f, prevState.width, prevState.height) });

			/*Perform experience steps*/
			if (replayBuffer->records.size() > replayBuffer->batchSize) {
				for (int i = 0; i < replayBuffer->batchSize; i++) {
					pen::ai::ReplayBufferData* experiences = replayBuffer->Sample();
					Optimize(experiences);
				}
			}

			if (prevState.matrix[0][0] == 0.0f) {
				prevState = state;
				prevAction = action;
			}
		}

		Weight* FreeAgent::GetWeights() {
			/*Return the weights for this neural network*/
			return weights;
		}

		int FreeAgent::GetLayers() {
			/*Return the number of layers for this neural network*/
			return numLayers;
		}

		pen::Mat FreeAgent::ComputeActionValues(pen::Mat* input) {
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
					layerVal |= (input->Dot(weightMatrix.T()) + biasMatrix);
				}
				else {
					layerVal |= activation.Dot(weightMatrix.T() + biasMatrix);
				}
				activation |= pen::Mat::Max(layerVal, threshold);
			}

			return layerVal;
		}

		void FreeAgent::UpdateWeights(Weight* weights, int numLayers) {
			/*Update the weights with the step size and gradients*/
			for (int i = 0; i < numLayers - 1; i++) {
				Weight* weight = &weights[i];

				for (int j = 0; j < weight->numCurrLayerNodes; j++) {
					mHat[i] |= mHat[i] * betaM + *weight->weightGrads * (1 - betaM);
					vHat[i] |= vHat[i] * betaV + *weight->weightGrads * *weight->biasGrads * (1 - betaV);
					*weight->weights += (mHat[i] * stepSize / (vHat[i].Sqrt() + epsilon));
				}
			}
		}

		pen::Mat FreeAgent::Softmax(pen::Mat* actionValues) {
			/*Returns action preferences given the action values*/
			pen::Mat preferences = *actionValues / preferenceFactor;
			pen::Mat maxPreferences = pen::Mat(0.0f, 1, preferences.height);

			for (int j = 0; j < preferences.height; j++) {
				float maxVal = -100000.0f;
				for (int i = 0; i < preferences.width; i++) {
					if (maxVal < preferences[j][i]) maxVal = preferences[j][i];
				}
				maxPreferences.matrix[j][0] = maxVal;
			}

			pen::Mat expPreferences = (preferences - maxPreferences).Exp();
			pen::Mat sumExpPreferences = expPreferences.Sum(true);

			/*The distribution of action probabilities*/
			return expPreferences / sumExpPreferences;
		}

		pen::Mat FreeAgent::TDError(pen::Mat* states, pen::Mat* nextStates, pen::Mat* actionsMat, pen::Mat* rewards, pen::Mat* terminals) {
			/*Returns a TD error matrix*/
			pen::Mat qNextMat = ComputeActionValues(nextStates);

			/*Softmax policy selection*/
			pen::Mat probsMat = Softmax(&qNextMat);

			pen::Mat vNextVec = (probsMat * qNextMat).Sum(true) * ((*terminals - 1.0f) * -1.0f);

			/*Expected sarsa*/
			pen::Mat targetVec = *rewards + vNextVec * discountValue;

			pen::Mat qMat = ComputeActionValues(states);

			/*Qvec is a list of the actions taken given the experiences from the replay buffer*/
			pen::Mat qVec = pen::Mat(0.0f, qMat.height, 1);
			for (int i = 0; i < qMat.height; i++) {
				qVec.matrix[0][i] = qMat[i][(int)*actionsMat[0][i]];
			}

			return targetVec - qVec;
		}

		void FreeAgent::TDUpdate(pen::Mat* states, pen::Mat* delta) {
			/*Update the gradients*/
			pen::Mat activation = pen::Mat();
			pen::Mat layerVal = pen::Mat();
			float threshold = 0.0f;

			for (int i = 0; i < numLayers - 1; i++) {
				pen::Mat weightMatrix = pen::Mat();
				pen::Mat biasMatrix = pen::Mat();
				weightMatrix |= *weights[i].weights;
				biasMatrix = *weights[i].bias;
				if (i == 0) {
					layerVal |= (states->Dot(weightMatrix.T()) + biasMatrix);
				}
				else {
					layerVal |= activation.Dot(weightMatrix.T() + biasMatrix);
				}
				activation |= pen::Mat::Max(layerVal, threshold);

				/*Compute the gradients*/
				pen::Mat nextWeightMatrix = pen::Mat();
				nextWeightMatrix |= *weights[i + 1].weights;
				pen::Mat thresholdMatrix = pen::Mat();
				thresholdMatrix.matrix = new float* [layerVal.height];
				for (int j = 0; j < layerVal.height; j++) {
					float* row = new float[layerVal.width];
					for (int k = 0; k < layerVal.width; k++) {
						row[k] = (layerVal[j][k] > 0.0f ? 1.0f : 0.0f);
					}
					thresholdMatrix.matrix[j] = row;
				}

				if (i == 0) {
					pen::Mat v = *delta;
					v |= v.Dot(nextWeightMatrix.T()) * thresholdMatrix;
					*weights[i].weightGrads |= states->T().Dot(v) / numStateParams;
					*weights[i].biasGrads |= v.Sum(false) / numStateParams;
				}
				else {
					pen::Mat v = *delta;
					*weights[i].weightGrads |= activation.T().Dot(v) / numStateParams;
					*weights[i].biasGrads |= v.Sum(false) / numStateParams;
				}
			}
		}

		void FreeAgent::Optimize(pen::ai::ReplayBufferData* experiences) {
			/*Optimize the network based on the replay experience*/
			pen::Mat statesMat = pen::Mat(0.0f, replayBuffer->batchSize * numStateParams, 1);
			pen::Mat actionMat = pen::Mat(0.0f, replayBuffer->batchSize, 1);
			pen::Mat rewardsMat = pen::Mat(0.0f, replayBuffer->batchSize, 1);
			pen::Mat terminalsMat = pen::Mat(0.0f, replayBuffer->batchSize, 1);
			pen::Mat nextStatesMat = pen::Mat(0.0f, replayBuffer->batchSize * numStateParams, 1);
			for (int i = 0; i < replayBuffer->batchSize; i++) {
				/*The states and nextStates compact all state params into a row vector*/
				for (int j = 0; j < numStateParams; j++) {
					statesMat[0][i * numStateParams + j] = experiences[i].state.matrix[j][0];
					nextStatesMat[0][i * numStateParams + j] = experiences[i].nextState.matrix[j][0];
				}

				actionMat[0][i] = experiences[i].action->id;
				rewardsMat[0][i] = experiences[i].reward;
				terminalsMat[0][i] = experiences[i].state.matrix[0][0];
			}

			pen::Mat deltaVec = TDError(&statesMat, &nextStatesMat, &actionMat, &rewardsMat, &terminalsMat);

			pen::Mat deltaMat = pen::Mat(0.0f, numActions, numStateParams);

			for (int i = 0; i < deltaVec.width; i++) {
				deltaMat.matrix[i][(int)actionMat.matrix[0][i]] = deltaVec.matrix[0][i];
			}

			TDUpdate(&statesMat, &deltaMat);
			UpdateWeights(weights, numLayers);
		}

		pen::ai::Action* FreeAgent::ChoosePolicy(pen::Mat* s) {
			/*Choose an action based on the softmax policy*/
			pen::Mat actionValues = ComputeActionValues(s);
			pen::Mat probs = Softmax(&actionValues);
			return actions[WeightedRand(&probs)];
		}

		int FreeAgent::WeightedRand(pen::Mat* vec) {
			/*Returns an index based on the weighted distribution of probability*/
			int range = 0;
			int weightSum = 0;
			if (vec->height == 1) {
				/*Row vector*/
				range = vec->width;
				for (int i = 0; i < range; i++) {
					weightSum += vec->matrix[0][i];
				}

				int randNum = Rand(weightSum);
				for (int j = 0; j < range; j++) {
					if (randNum < vec->matrix[0][j]) {
						return j;
					}
					randNum -= vec->matrix[0][j];
				}
			}
			else if (vec->width == 1) {
				/*Column vector*/
				range = vec->height;
				for (int i = 0; i < range; i++) {
					weightSum += vec->matrix[i][0];
				}

				int randNum = Rand(weightSum);
				for (int j = 0; j < range; j++) {
					if (randNum < vec->matrix[j][0]) {
						return j;
					}
					randNum -= vec->matrix[j][0];
				}
			}
			return -1;
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
							+ "\nnum actions:" + std::to_string(numActions)
							+ "\nnum episodes:" + std::to_string(numEpisodes) + "\nnum layers:" + std::to_string(numLayers) + "\nnum state params:" + std::to_string(numStateParams)
							+ "\n[m hat weights]^[m hat biases]^[v hat weights]^[v hat biases]");
						modelFile << input;

						for (int i = 0; i < numLayers - 1; i++) {
							input = "[";
							if (i < numLayers - 2) {
								input += (FormatMatrix(&mHat[i], ',') + " ]^[" + FormatMatrix(&vHat[i], ',') + "]^");
							}
							else {
								input += (FormatMatrix(&mHat[i], ',') + " ]^[" + FormatMatrix(&vHat[i], ',') + "]");
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

		void FreeAgent::Load(const std::string& path, pen::ai::Action** userActions, long userNumActions) {
			/*Load a free agent model*/
			std::string tempPath = (path.find(".farlpen") != std::string::npos ? path : path + ".farlpen");
			std::ifstream modelFile;
			std::string input = "";
			pen::ai::Weight* weight = nullptr;
			std::string matrixStr = "";
			bool header = true;
			int counter = 0;
			std::vector<pen::ai::Weight*> weightVector;
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
				while (!modelFile.eof()) {
					input = "";

					/*Load the weights*/
					if (header) {
						if (counter < 7) {
							/*Grab the header meta data for the agent*/
							std::getline(modelFile, input);
							if (counter == 0) epsilon = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 1) discountValue = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 2) stepSize = std::stof(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 3) numActions = std::stol(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 4) numEpisodes = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 5) numLayers = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 6) numStateParams = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							counter++;
						}
						else {
							std::getline(modelFile, input);
							mHat = new pen::Mat[numLayers - 1];
							vHat = new pen::Mat[numLayers - 1];

							for (int i = 0; i < numLayers - 1; i++) {
								matrixStr = Agent::Split(input, '^', i * 2);
								matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
								mHat[i] = *ParseMatrix(matrixStr);
								matrixStr = Agent::Split(input, '^', i * 2 + 1);
								matrixStr = matrixStr.substr(1, matrixStr.length() - 2);
								vHat[i] = *ParseMatrix(matrixStr);
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
				weights = new pen::ai::Weight[weightCount];
				for (int i = 0; i < weightCount; i++) {
					weights[i] = *weightVector[i];
				}
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
