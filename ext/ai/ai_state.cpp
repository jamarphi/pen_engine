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
#include "ai_state.h"

namespace pen {
	namespace ai {
		AIState::AIState() {
			id = 0x00000000;
			policiesNum = 0;
			state = nullptr;
			stateParamsNum = 0;
			policies = nullptr;
			reward = 0.0f;
			stateValue = 0.0f;
			optimalPolicy = nullptr;
			terminal = false;
		}

		AIState::AIState(char userId, int numStates, int numActions, float userReward, StateAction** userActions, bool terminalStatus, float* userState, int numStateParams) {
			/*Creates a state with a given policy*/
			if (numActions <= numStates) {
				id = userId;
				policiesNum = numActions;
				reward = userReward;
				state = userState;
				stateValue = 0.0f;
				StateAction** p = new StateAction * [numActions];
				optimalPolicy = nullptr;
				terminal = terminalStatus;

				policies = userActions;

				if (userState != nullptr) {
					state = userState;
					stateParamsNum = numStateParams;
				}
			}
			else {
				std::cout << "State has more policies than overall allowed states in the environment\n";
			}
		}

		Action* AIState::FindPolicy(long id) {
			/*Returns a policy based on the id*/
			for (int i = 0; i < policiesNum; i++) {
				if (policies[i]->policy->id == id) return policies[i]->policy;
			}
			return nullptr;
		}
	}
}