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
#include "action.h"
#include "../../src/ops/matrices/mat.h"
#include <vector>
#include <iostream>

namespace pen {
	namespace ai {
		struct ReplayBufferData {
			pen::Mat state;
			pen::ai::Action* action;
			float reward;
			pen::Mat nextState;
		};

		class ReplayBuffer {
		public:
			std::vector<pen::ai::ReplayBufferData> records;
			int size;
			int batchSize;
		public:
			ReplayBuffer();
			void Insert(pen::ai::ReplayBufferData data);
			pen::ai::ReplayBufferData* Sample();
		};
	}
}