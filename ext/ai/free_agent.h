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
#include "agent.h"

namespace pen {
	namespace ai {
		class FreeAgent : public pen::ai::Agent {
			/*FreeAgent is based on Q learning*/
		public:
			FreeAgent();
			FreeAgent(const std::string& path, pen::ai::Action** userActions, int numActions);
			void Init(pen::ai::AIState** userStates, long userStateNum, pen::ai::AIState* initialState, int userNumEpisodes, float userEpsilon = 0.1f, float userStepSize = 0.1f);
			void Step();
			void Save(const std::string& path);
			void Load(const std::string& path, pen::ai::Action** userActions, int numActions);
		};
	}
}