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
#include <iostream>

namespace pen {
	template <typename T, typename W> struct Pair {
		T first;
		W second;
		int id;
	};

	template <typename T, typename W> class Map {
	private:
		int nextId = 0;
	public:
		/*Key value pairs*/
		std::vector<Pair<T, W>> items;

		Map(){}

		W* operator[](int idx) {
			/*Returns the value at teh given index*/
			return &items[idx].second;
		}

		void Insert(T key, W value) {
			/*Adds key value pair to the map*/
			bool exists = false;
			for (int i = 0; i < items.size(); i++) {
				if (items[i].first == key) {
					exists = true;
					break;
				}
			}

			if (!exists) {
				Pair<T, W> hash = { key, value, nextId };
				items.push_back(hash);
				nextId++;
			}
			else {
				for (int j = 0; j < items.size(); j++) {
					if (items[j].first == key) {
						items[j].second = value;
						break;
					}
				}
			}
		}

		void Update(T key, W value) {
			/*Update an existing key*/
			bool found = false;

			for (int i = 0; i < items.size(); i++) {
				if (items[i].first == key) {
					items[i].second = value;
					found = true;
					break;
				}
			}

			if (!found) {
				std::cout << "Key to update not found" << std::endl;
			}
		}

		Pair<T, W>* Find(T key) {
			/*Locates pair based on given key*/
			for (int i = 0; i < items.size(); i++) {
				if (items[i].first == key) {
					return &items[i];
				}
			}
			return nullptr;
		}

		void Clear() {
			/*Empties out the map*/
			items.clear();
			nextId = 0;
		}

		void Remove(T key) {
			/*Removes a pair from the map*/
			std::vector<Pair<T, W>> tempList;
			for (int i = 0; i < items.size(); i++) {
				if (items[i].first != key) {
					tempList.push_back(items[i]);
				}
			}
			items = tempList;
		}

		int Size() {
			/*Returns the size of the map*/
			return items.size();
		}
	};
}