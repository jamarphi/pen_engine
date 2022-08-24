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
#include "replay_buffer.h"
#include "../../src/state/config.h"
#include "../../src/state/layer_manager.h"
#ifdef __PEN_MAC_IOS__
#include <TargetConditionals.h>
#endif

/*__PEN_AI_AGENT_MOBILE__ is for IOS and Android*/
/*
 On mobile the text file will be returned as a string for the user to save it
 to a server or their own custom database since android's asset directory is read only
 and ios should be the same to keep things consistent
 */
#ifdef __APPLE__
#if !TARGET_OS_OSX
#define __PEN_AI_AGENT_MOBILE__
#endif
#endif

#ifdef __PEN_ANDROID__
#ifndef __PEN_AI_AGENT_MOBILE__
#define __PEN_AI_AGENT_MOBILE__
#endif
#endif

namespace pen {
	namespace ai {
		class Agent {
			/*This agent is based on the dynaq+ architecture*/
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
			pen::ai::AIState* FindState(Agent* agent, char stateId);
			virtual void Step(bool terminal);
#ifndef __PEN_MOBILE__
			virtual void Save(const std::string& path);
#else
            virtual std::string Save(const std::string& path);
#endif
			virtual void Load(const std::string& path, pen::ai::Action** userActions, long userNumActions);			
		private:
			int Rand(int range);
			virtual int WeightedRand(int range, float* probs);
			float Max(pen::ai::AIState* s);
			int Argmax(pen::ai::StateAction** qValues, int length);
			void UpdateModel(pen::ai::AIState* s, float reward);
			void Plan(bool terminal);
			pen::ai::StateAction* ChooseAction(pen::ai::AIState* s);
			std::string FormatList(int idx, char character, bool nested = false);
			float* ParseStateList(std::string input, char character, int numStateParams);
			pen::ai::StateAction** ParsePoliciesList(pen::ai::Action** actionList, int numActions, int policiesNum, std::string input, char character);
			std::string Split(const std::string& line, const char& character, const unsigned int& section);

			/*----Free Agents----*/
			virtual Weight* GetWeights() { return nullptr; };
			virtual int GetLayers() { return 0; };
			/*----Free Agents----*/
		};
	}
}
