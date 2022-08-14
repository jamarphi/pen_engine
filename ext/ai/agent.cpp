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
#include "agent.h"

namespace pen {
	namespace ai {
		long Agent::nextId = 0;

		Agent::Agent() {
			id = Agent::nextId;
			Agent::nextId++;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.1f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			discountValue = 1.0f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
		}

		Agent::Agent(const std::string& path, pen::ai::Action** userActions, int numActions) {
			/*Load in an agent from file*/
			id = -1;
			lastAction = nullptr;
			stepSize = 0.1f;
			epsilon = 0.1f;
			initialState = nullptr;
			numStates = 0;
			states = nullptr;
			currentState = nullptr;
			discountValue = 1.0f;
			accuracyCap = 0.1f;
			totalReward = 0.0f;
			planningSteps = 10;
			bonusFactor = 0.001f;
			numEpisodes = 0;
			Load(path, userActions, numActions);
		}

		void Agent::Init(pen::ai::AIState** userStates, long userStateNum, pen::ai::AIState* userInitialState, int numPlanningSteps, float userEpsilon, float userStepSize) {
			/*Initialize the environment for the agent*/
			epsilon = userEpsilon;
			stepSize = userStepSize;
			initialState = userInitialState;
			numStates = userStateNum;
			states = userStates;
			currentState = userInitialState;
			planningSteps = numPlanningSteps;

			lastAction = ChooseAction(userInitialState);
		}

		pen::ai::AIState* Agent::FindState(Agent* agent, char stateId) {
			/*Return a state based on its id*/
			for (int i = 0; i < agent->numStates; i++) {
				if (agent->states[i]->id == stateId) return agent->states[i];
			}
			return nullptr;
		}

		void Agent::Step(bool terminal) {
			/*Take a step through the environment*/

			/*Increments the visits for all state action pairs*/
			for (int i = 0; i < numStates; i++) {
				pen::ai::AIState* s = states[i];
				for (int j = 0; j < s->policiesNum; j++) {
					s->policies[j]->visits++;
				}
			}

			/*Take the past state's last action and set its visits to 0*/
			lastAction->visits = 0;

			/*Update the q value for this past action*/
			pen::ai::AIState* nextState = FindState(this, lastAction->nextStateId);
			lastAction->value += (stepSize * (lastAction->reward + (terminal ? 0.0f : 1.0f) * discountValue * Max(nextState) - lastAction->value));
			UpdateModel(nextState, lastAction->reward);
			Plan(terminal);

			if (!terminal) {
				pen::ai::StateAction* action = ChooseAction(nextState);
				currentState = nextState;
				lastAction = action;
			}
		}

		int Agent::Rand(int range) {
			/*Returns a number within a given range*/
			if (range > 0) {
				return ((int)(((float)std::rand() / RAND_MAX) + range));
			}
			return 0;
		}

		int Agent::WeightedRand(int range, float* probs) {
			/*Returns an index based on the weighted distribution of probability*/
			int weightSum = 0;
			for (int i = 0; i < range; i++) {
				weightSum += probs[i];
			}

			int randNum = Agent::Rand(weightSum);
			for (int j = 0; j < range; j++) {
				if (randNum < probs[j]) {
					delete[] probs;
					return j;
				}
				randNum -= probs[j];
			}

			delete[] probs;
			return -1;
		}

		float Agent::Max(pen::ai::AIState* s) {
			/*Returns the largest number in the action values for a given state*/
			float maxNum = -1000000.0f;
			for (int i = 0; i < s->policiesNum; i++) {
				if (s->policies[i]->value > maxNum) maxNum = s->policies[i]->value;
			}
			return maxNum;
		}

		int Agent::Argmax(pen::ai::StateAction** qValues, int length) {
			/*Returns the index of the largest value in an array, if there is a tie an index of the ties is returned randomly*/
			int topValue = -1000000;
			std::vector<int> ties;

			for (int i = 0; i < length; i++) {
				if (qValues[i]->value > topValue) {
					topValue = qValues[i]->value;
					ties.clear();
					ties.push_back(i);
				}
				else if (qValues[i]->value == topValue) {
					ties.push_back(i);
				}
			}

			return ties[(int)pen::op::Max(0.0f, Agent::Rand(ties.size()) - 1.0f)];
		}

		void Agent::UpdateModel(pen::ai::AIState* s, float reward) {
			/*Update the model after performing a step within the dynaq+ architecture*/
			pen::ai::StateAction* action = nullptr;

			/*The current state is the past state since the new state will replace it*/
			for (int i = 0; i < currentState->policiesNum; i++) {
				if (lastAction->actionName == currentState->policies[i]->actionName && action == nullptr) {
					action = currentState->policies[i];
				}
				else {
					currentState->policies[i]->reward = 0.0f;
				}
			}

			/*This action in the past state gets updated*/
			action->reward = reward;
		}

		void Agent::Plan(bool terminal) {
			/*This does Q planning*/
			for (int i = 0; i < planningSteps; i++) {
				pen::ai::AIState* pastState = states[(int)pen::op::Max(Rand(numStates), 0.0f)];
				pen::ai::StateAction* pastAction = pastState->policies[(int)pen::op::Max(Rand(pastState->policiesNum), 0.0f)];

				/*The reward for going to the next state*/
				float reward = pastAction->reward;
				reward += (bonusFactor * pen::op::Sqrt((float)pastAction->visits));
				pen::ai::AIState* s = FindState(this, pastAction->nextStateId);
				if (!terminal) {
					pastAction->value += (stepSize * (reward + discountValue * Max(s) - pastAction->value));
				}
				else {
					pastAction->value += (stepSize * (reward - pastAction->value));
				}
			}
		}

		pen::ai::StateAction* Agent::ChooseAction(pen::ai::AIState* s) {
			/*Choose an action using epsilon greedy*/
			pen::ai::StateAction* action = nullptr;
			if (((float)std::rand() / RAND_MAX) < epsilon) {
				action = s->policies[(int)pen::op::Max(Rand(s->policiesNum), 0.0f)];
			}
			else {
				action = s->policies[Argmax(s->policies, s->policiesNum)];
			}
			return action;
		}

#ifndef __PEN_AI_AGENT_MOBILE__
		void Agent::Save(const std::string& path) {
#else
        std::string Agent::Save(const std::string& path) {
            /*
             On mobile the text file will be returned as a string for the user to save it
             to a server or their own custom database since android's asset directory is read only
             and ios should be the same to keep things consistent
             */
#endif
			/*Save an agent model*/
			std::string tempPath = (path.find(".arlpen") != std::string::npos ? path : path + ".arlpen");
			std::string input = "";
            bool header = true;
            bool writing = true;
            int counter = 0;
#ifndef __PEN_AI_AGENT_MOBILE__
            std::ofstream modelFile;
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
#endif
				while (writing) {
#ifndef __PEN_AI_AGENT_MOBILE__
					input = "";
#endif
					/*Write a state*/
					if (header) {
						/*Write the header information before states*/
						input += ("epsilon:" + std::to_string(epsilon) + "\ndiscount value:" + std::to_string(discountValue) + "\nstep size:" + std::to_string(stepSize)
							+ "\nplanning steps:" + std::to_string(planningSteps) + "\ninitial state id:" + std::to_string(initialState->id)
							+ "\nnum episodes:" + std::to_string(numEpisodes)
							+ "\nid/policies num/[states]/state params num/{policies}/reward/state value/optimal policy\n");
#ifndef __PEN_AI_AGENT_MOBILE__
						modelFile << input;
#endif
						header = false;
					}
					else {
						pen::ai::AIState* s = states[counter];
						input += (s->id + "/" + std::to_string(s->policiesNum) + "/["
							+ FormatList(counter, ',', false) + "]/" + std::to_string(s->stateParamsNum) + "/{"
							+ FormatList(counter, ',', true) + "}/" + std::to_string(s->reward) + "/"
							+ std::to_string(s->stateValue) + "/" + (std::to_string(s->optimalPolicy->id) + "^" + std::to_string(s->optimalPolicy->policy->id)
								+ "^" + std::to_string(s->optimalPolicy->value) + "^" + s->optimalPolicy->actionName));

                        input += ("\n----\n");
#ifndef __PEN_AI_AGENT_MOBILE__
                        modelFile << input;
#endif

						counter++;
						if (counter == numStates) {
							writing = false;
                            input += ("\n----\n");
    #ifndef __PEN_AI_AGENT_MOBILE__
                            modelFile << input;
    #endif
						}
					}
				}
#ifndef __PEN_AI_AGENT_MOBILE__
				modelFile.close();
			}
#else
            return input;
#endif
		}

		void Agent::Load(const std::string& path, pen::ai::Action** userActions, long userNumActions) {
			/*Load an agent model*/
			std::string tempPath = (path.find(".arlpen") != std::string::npos ? path : path + ".arlpen");
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
#ifndef __PEN_MOBILE__
                std::ifstream modelFile;
                modelFile.open(tempPath);
                if (modelFile.is_open()) {
                    while (!modelFile.eof()) {
#else
                pen::Asset mobileAgentFile = pen::Asset::Load(tempPath, nullptr);
                unsigned int fileOffset = 0;
                        std::string mobileAgentFileStr(mobileAgentFile.data);
                if (mobileAgentFile.data != nullptr) {
                    while (true) {
#endif
					input = "";

					/*Load the states*/
					if (header) {
						if (counter < 6) {
							/*Grab the header meta data for the agent*/
#ifndef __PEN_MOBILE__
                            std::getline(modelFile, input);
#else
                            input = pen::ui::ReadLine(mobileAgentFileStr, &fileOffset);
                            if (input == "") {
                                break;
                            }
#endif
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
#ifndef __PEN_MOBILE__
                            std::getline(modelFile, input);
#else
                            input = pen::ui::ReadLine(mobileAgentFileStr, &fileOffset);
                            if (input == "") {
                                break;
                            }
#endif

						if (input == "----" || input == "====") continue;

						s = new pen::ai::AIState();
						s->id = Agent::Split(input, '/', 0)[0];
						s->policiesNum = std::stoi(Agent::Split(input, '/', 1));
						s->stateParamsNum = std::stoi(Agent::Split(input, '/', 3));
						stateArr = Agent::Split(input, '/', 2);
						stateArr = stateArr.substr(1, stateArr.length() - 2);
						s->state = Agent::ParseStateList(stateArr, ',', s->stateParamsNum);
						policiesArr = Agent::Split(input, '/', 4);
						policiesArr = policiesArr.substr(1, policiesArr.length() - 2);
						s->policies = Agent::ParsePoliciesList(userActions, userNumActions, s->policiesNum, stateArr, ',');
						s->reward = std::stof(Agent::Split(input, '/', 5));
						s->stateValue = std::stof(Agent::Split(input, '/', 6));
						optimalPi = Agent::Split(input, '/', 7);
						optimalPiName = Agent::Split(input, '^', 3);
						optimalPiValue = std::stof(Agent::Split(optimalPi, '^', 2));

						for (int i = 0; i < s->policiesNum; i++) {
							if (s->policies[i]->actionName == optimalPiName && s->policies[i]->value == optimalPiValue) {
								s->optimalPolicy = s->policies[i];
								break;
							}
						}

						stateVector.push_back(s);
					}

				}
#ifndef __PEN_MOBILE__
				modelFile.close();
#endif

				/*Set the states in the agent*/
				states = new pen::ai::AIState*[stateVector.size()];
				for (int j = 0; j < stateVector.size(); j++) {
					states[j] = stateVector[j];
				}

				initialState = FindState(this, initialStateId);
				currentState = initialState;

				lastAction = ChooseAction(initialState);
			}
		}

		std::string Agent::FormatList(int idx, char character, bool nested) {
			/*Format a list to a string*/
			std::string input = "";
			if (nested) {
				/*Format the policies list to a string*/
				pen::ai::StateAction** actions = states[idx]->policies;
				if (actions != nullptr) {
					int policies = states[idx]->policiesNum;
					for (int i = 0; i < policies; i++) {
						input += (std::to_string(actions[i]->id) + "^" + std::to_string(actions[i]->value) + "^" + std::to_string(actions[i]->reward) + "^" + std::to_string(actions[i]->visits)
							+ "^" + std::to_string(actions[i]->nextStateId) + "^" + actions[i]->actionName + character);
					}
					return input.substr(0, input.length() - 1);
				}
			}
			else {
				/*Format the state list to a string*/
				float* state = states[idx]->state;
				if (state != nullptr) {
					int params = states[idx]->stateParamsNum;
					for (int i = 0; i < params; i++) {
						input += (std::to_string(state[i]) + character);
					}
					return input.substr(0, input.length() - 1);
				}
			}
			return "";
		}

		float* Agent::ParseStateList(std::string input, char character, int numStateParams) {
			/*Parse the state list from a string*/
			std::vector<std::string> listItems;
			bool splitting = true;
			std::string tempStr = "";
			int counter = 0;
			while (splitting) {
				tempStr = Agent::Split(input, character, counter);
				if (tempStr != "") {
					listItems.push_back(tempStr);
				}
				else {
					splitting = false;
				}
			}

			float* state = new float[numStateParams];
			for (int i = 0; i < numStateParams; i++) {
				state[i] = std::stof(listItems[i]);
			}
			return state;
		}

		pen::ai::StateAction** Agent::ParsePoliciesList(pen::ai::Action** actionList, int numActions, int policiesNum, std::string input, char character) {
			/*Parse the policies list from a string*/
			std::vector<std::string> listItems;
			bool splitting = true;
			std::string tempStr = "";
			int counter = 0;
			long tempId = 0;
			while (splitting) {
				tempStr = Agent::Split(input, character, counter);
				if (tempStr != "") {
					listItems.push_back(tempStr);
				}
				else {
					splitting = false;
				}
			}

			pen::ai::StateAction** actions = new pen::ai::StateAction * [policiesNum];
			for (int i = 0; i < policiesNum; i++) {
				pen::ai::Action* action = nullptr;
				std::string listAction = Agent::Split(listItems[i], '^', 5);
				for (int j = 0; j < numActions; j++) {
					if (actionList[j]->actionName == listAction) {
						action = actionList[j];
						break;
					}
				}

				if (action != nullptr) {
					actions[i] = new pen::ai::StateAction(&tempId, action, std::stol(pen::ai::Agent::Split(listItems[i], '^', 4)), std::stof(pen::ai::Agent::Split(listItems[i], '^', 2)), action->actionName);
					actions[i]->id = std::stol(pen::ai::Agent::Split(listItems[i], '^', 0));
					actions[i]->value = std::stof(pen::ai::Agent::Split(listItems[i], '^', 1));
					actions[i]->visits = std::stoi(pen::ai::Agent::Split(listItems[i], '^', 3));
				}
			}

			return actions;
		}

		std::string Agent::Split(const std::string& line, const char& character, const unsigned int& section) {
			/*Split a line by a given character*/
			int counter = 0;
			int previousSectionIdx = 0;
			bool keepGoing = true;
			if (line.find(character) != std::string::npos) {
				while (keepGoing) {
					keepGoing = false;
					if (line.find(character, previousSectionIdx + 1) != std::string::npos) {
						if (counter == section) {
							if (counter > 0) {
								int startIdx = previousSectionIdx + 1;
								int endIdx = line.find(character, previousSectionIdx + 1);

								return line.substr(startIdx, endIdx - startIdx);
							}
							else {
								return line.substr(0, line.find(character));
							}
						}
						else {
							previousSectionIdx = line.find(character, previousSectionIdx + 1);
							counter++;
							keepGoing = true;
						}
					}
				}
				return "";
			}
			else {
				return line;
			}
		}
	}
}
