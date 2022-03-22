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
#include "replay_buffer.h"

namespace pen {
	namespace ai {
		ReplayBuffer::ReplayBuffer() {
			size = 20;
			batchSize = 20;
		}

		void ReplayBuffer::Insert(pen::ai::ReplayBufferData data) {
			/*Adds a record to the queue*/
			if (records.size() > size) {
				std::vector<pen::ai::ReplayBufferData> tempList;
				for (int i = 1; i < records.size(); i++) {
					tempList.push_back(records[i]);
				}
				records = tempList;
			}
			records.push_back(data);
		}

		pen::ai::ReplayBufferData* ReplayBuffer::Sample() {
			/*Returns a list of indices chosen at random*/
			pen::ai::ReplayBufferData* samples = new pen::ai::ReplayBufferData[batchSize];
			for (int i = 0; i < batchSize; i++) {
				samples[i] = records[((int)(((float)std::rand() / RAND_MAX) + records.size()))];
			}
			return samples;
		}
	}
}