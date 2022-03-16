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
#include <vector>
#include <fstream>
#include <sstream>
#include "ai_state.h"
#include "../../src/ops/operations/trig.h"
#include "../../src/ops/operations/operations.h"
#include "weight.h"

namespace pen {
	namespace ai {
		struct Shift {
			long stateId;
			long nextStateId;
			pen::ai::StateAction* action;
			/*Reward of the given state and the successor state*/
			float reward;
			/*Probability of a successor state given a state*/
			float prob;
		};

		class Agent {
			/*The base agent is based on the dynaq+ architecture*/
			friend class FreeAgent;
		public:
			static long nextId;
			long id;
			pen::ai::StateAction* lastAction;
			float stepSize;
			float epsilon;
			pen::ai::AIState* initialState;
			long numStates;
			pen::ai::AIState** states;
			pen::ai::AIState* currentState;
			Shift** shift;
			float discountValue;
			float accuracyCap;
			float totalReward;
			int planningSteps;
			float bonusFactor;
			int numEpisodes;
		public:
			Agent();
			Agent(const std::string& path, pen::ai::Action** userActions, int numActions);
			virtual void Init(pen::ai::AIState** userStates, long userStateNum, pen::ai::AIState* initialState, int numPlanningSteps, float userEpsilon = 0.1f, float userStepSize = 0.1f);
			~Agent();
			pen::ai::AIState* FindState(Agent* agent, char stateId);
			void DeleteStates();
			float Reward(pen::ai::AIState* s, pen::ai::AIState* nextState);
			float Prob(pen::ai::AIState* nextState, float reward, pen::ai::AIState* s, pen::ai::StateAction* a);
			void UpdateShift(pen::ai::AIState* s, pen::ai::StateAction* a);
			void PrintShifts();
			virtual void Step(bool terminal);
			static int Rand(int range);
			static int WeightedRand(int range, float* probs);
			float Max(pen::ai::AIState* s);
			int Argmax(pen::ai::StateAction** qValues, int length);
			void QGreedify(pen::ai::AIState* s);
			void ValueIteration();
			void OptimalityUpdate(pen::ai::AIState* s);
			void UpdateModel(pen::ai::AIState* s, float reward);
			void Plan(bool terminal);
			pen::ai::StateAction* ChooseAction(pen::ai::AIState* s);
			virtual void Save(const std::string& path);
			virtual void Load(const std::string& path, pen::ai::Action** userActions, int numActions);

			/*----Free Agents----*/
			virtual Weight** GetWeights() { return nullptr; };
			virtual int GetLayers() { return 0; };
			virtual pen::Mat ComputeOutput(pen::Mat* input, Weight* weights, int numLayers) { return pen::Mat(); };
			virtual void UpdateWeights(Weight* weights, pen::Mat* g, int numLayers) {};
			virtual pen::Mat OneHot(int state) { return pen::Mat(); };
			/*----Free Agents----*/
		private:
			std::string FormatList(int idx, char character, bool nested = false);
			float* ParseStateList(std::string input, char character, int numStateParams);
			pen::ai::StateAction** ParsePoliciesList(pen::ai::Action** actionList, int numActions, int policiesNum, std::string input, char character);
			std::string Split(const std::string& line, const char& character, const unsigned int& section);
		};
	}
}