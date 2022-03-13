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
			epsilon = 0.1f;
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
		}

		FreeAgent::FreeAgent(const std::string& path, pen::ai::Action** userActions, int numActions) {
			/*Load in an agent with the actions already defined*/
			id = -1;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.1f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			shift = nullptr;
			discountValue = 1.0f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			Load(path, userActions, numActions);
		}

		void FreeAgent::Init(pen::ai::AIState** userStates, long userStateNum, pen::ai::AIState* userInitialState, int userNumEpisodes, float userEpsilon, float userStepSize) {
			/*Each agent should have its own states unless you want an agent to share learning with another agent*/
			epsilon = userEpsilon;
			stepSize = userStepSize;
			initialState = userInitialState;
			numStates = userStateNum;
			states = userStates;
			currentState = userInitialState;
			numEpisodes = userNumEpisodes;

			lastAction = ChooseAction(userInitialState);
		}

		void FreeAgent::Step() {
			/*Take a step through the environment*/

			pen::ai::AIState* nextState = FindState(this, lastAction->nextStateId);
			pen::ai::StateAction* action = ChooseAction(nextState);
			lastAction->value += (stepSize * (lastAction->reward + (nextState->terminal ? 0.0f : 1.0f) * discountValue * Max(nextState) - lastAction->value));
			currentState = nextState;
			lastAction = action;
		}

		void FreeAgent::Save(const std::string& path) {
			/*Save a free agent model*/
			std::string tempPath = (path.find(".farlpen") != std::string::npos ? path : path + ".farlpen");
			std::ofstream modelFile;
			std::string input = "";
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
				bool writing = true;
				int counter = 0;
				bool header = true;

				while (writing) {
					input = "";
					/*Write a state*/
					if (header) {
						/*Write the header information before states*/
						input += ("epsilon:" + std::to_string(epsilon) + "\ndiscount value:" + std::to_string(discountValue) + "\nstep size:" + std::to_string(stepSize)
							+ "\nplanning steps:" + std::to_string(planningSteps) + "\ninitial state id:" + std::to_string(initialState->id)
							+ "\nnum episodes:" + std::to_string(numEpisodes)
							+ "\nid/policies num/[states]/state params num/{policies}/reward/state value/optimal policy\n");
						modelFile << input;
						header = false;
					}
					else {
						pen::ai::AIState* s = states[counter];
						input += (s->id + "/" + std::to_string(s->policiesNum) + "/["
							+ pen::ai::Agent::FormatList(counter, ',', false) + "]/" + std::to_string(s->stateParamsNum) + "/{"
							+ pen::ai::Agent::FormatList(counter, ',', true) + "}/" + std::to_string(s->reward) + "/"
							+ std::to_string(s->stateValue) + "/" + (std::to_string(s->optimalPolicy->id) + "^" + std::to_string(s->optimalPolicy->policy->id)
								+ "^" + std::to_string(s->optimalPolicy->value) + "^" + s->optimalPolicy->actionName));

						modelFile << (input + "\n----\n");

						counter++;
						if (counter == numStates) {
							writing = false;
							modelFile << "\n----\n";
						}
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
			std::string stateArr = "";
			std::string policiesArr = "";
			std::string optimalPi = "";
			std::string optimalPiName = "";
			pen::ai::AIState* s = nullptr;
			char initialStateId = 0x00000000;
			float optimalPiValue = 0.0f;
			bool header = true;
			int counter = 0;
			std::vector<pen::ai::AIState*> stateVector;
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
							if (counter == 3) planningSteps = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							if (counter == 4) initialStateId = pen::ai::Agent::Split(input, ':', 1)[0];
							if (counter == 5) numEpisodes = std::stoi(pen::ai::Agent::Split(input, ':', 1));
							counter++;
						}
						else {
							header = false;
						}
					}
					else {
						std::getline(modelFile, input);

						if (input == "----" || input == "====") continue;

						s = new pen::ai::AIState();
						s->id = pen::ai::Agent::Split(input, '/', 0)[0];
						s->policiesNum = std::stoi(pen::ai::Agent::Split(input, '/', 1));
						s->stateParamsNum = std::stoi(pen::ai::Agent::Split(input, '/', 3));
						stateArr = pen::ai::Agent::Split(input, '/', 2);
						stateArr = stateArr.substr(1, stateArr.length() - 2);
						s->state = pen::ai::Agent::ParseStateList(stateArr, ',', s->stateParamsNum);
						policiesArr = pen::ai::Agent::Split(input, '/', 4);
						policiesArr = policiesArr.substr(1, policiesArr.length() - 2);
						s->policies = pen::ai::Agent::ParsePoliciesList(userActions, numActions, s->policiesNum, stateArr, ',');
						s->reward = std::stof(pen::ai::Agent::Split(input, '/', 5));
						s->stateValue = std::stof(pen::ai::Agent::Split(input, '/', 6));
						optimalPi = pen::ai::Agent::Split(input, '/', 7);
						optimalPiName = pen::ai::Agent::Split(input, '^', 3);
						optimalPiValue = std::stof(pen::ai::Agent::Split(optimalPi, '^', 2));

						for (int i = 0; i < s->policiesNum; i++) {
							if (s->policies[i]->actionName == optimalPiName && s->policies[i]->value == optimalPiValue) {
								s->optimalPolicy = s->policies[i];
								break;
							}
						}

						stateVector.push_back(s);
					}

				}
				modelFile.close();

				/*Set the states in the agent*/
				states = new pen::ai::AIState * [stateVector.size()];
				for (int j = 0; j < stateVector.size(); j++) {
					states[j] = stateVector[j];
				}

				initialState = FindState(this, initialStateId);
				currentState = initialState;

				lastAction = ChooseAction(initialState);
			}
		}
	}
}