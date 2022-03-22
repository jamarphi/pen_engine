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
#include <string>
#include <cstring>

namespace pen {
	namespace ai {
		class Action {
			/*
			Actions are defined by what callback they are associated with, as well as what state they cause the agent's environment to transition to.
			If the nextStateId is -1, then that means the action stays in the current state.
			*/
		private:
			static long nextId;
		public:
			long id;
			void (*action)();
			float value;
			std::string actionName;
		public:
			Action(void (*userAction)(), const std::string& userActionName);
		};

		class StateAction {
			/*A container that has the probability of an action based on the state it is associated with as well as the next state from that current state given that action*/
		public:
			long id;
			Action* policy;
			float value;
			float reward;
			int visits;
			long nextStateId;
			std::string actionName;
		public:
			StateAction() {
				id = -1;
				policy = nullptr;
				value = 0.0f;
				reward = 0.0f;
				visits = 0;
				nextStateId = 0;
				actionName = "";
			}

			StateAction(long* userId, Action* userAction, long userNextStateId, float nextStateReward, const std::string& userActionName, int numActions = -1) {
				/*A container that has the probability of an action based on the state it is associated with as well as the next state from that current state given that action*/
				id = *userId;
				(*userId)++;
				policy = userAction;
				value = 1.0f / (float)(numActions > 0.0f ? 1.0f : numActions);
				nextStateId = userNextStateId;
				reward = nextStateReward;
				visits = 0;
			}
		};
	}
}