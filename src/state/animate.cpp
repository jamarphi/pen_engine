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
#include "animate.h"

namespace pen {
	std::vector<pen::AnimItem> Anim::animList = {};
	
	void Anim::Add(pen::Item* item, const unsigned int& type, const long& ms, const bool& infinite, const float& unitA, const float& unitB, const float& unitC) {
		/*Add animation to the queue*/
		int frames = 0;
		float deltaTime = 0.0035f;
		if (!infinite) frames = ((float)ms / 1000.0f) * (1.0f / deltaTime) * ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/

		pen::AnimItem newItem;
		newItem.item = item;
		newItem.type = type;
		newItem.infinite = infinite;
		newItem.frames = frames;
		newItem.ran = false;
		newItem.unitA = unitA * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		newItem.unitB = unitB * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		newItem.unitC = unitC * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		animList.push_back(newItem);
	}

	bool Anim::CheckStatus(const pen::AnimItem& item) {
		/*If the item is already transformed from another animation that is not done then return true*/
		for (int i = 0; animList.size(); i++) {
			if (animList[i].ran && animList[i].item == item.item && animList[i].type == item.type) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	void Anim::Animate(AnimItem item) {
		/*Run the animation*/
		switch (item.type) {
		case 0:
			/*Translation, all units are used since there are three values*/
			pen::Translate(item.item, item.unitA, item.unitB);
			break;
		case 1:
			/*Rotate, unit A is used since there is only one value*/
			pen::Rotate(item.item, item.unitA);
			break;
		case 2:
			/*Scale, two units are used since there are only two values*/
			pen::Scale(item.item, item.unitA, item.unitB > 0 ? item.unitB : item.unitA);
			break;
		default:
			break;
		}
	}

	void Anim::Run() {
		/*Runs the animations for each item*/
		if (!animList.empty()) {
			for (auto& item : animList) {
				if (!CheckStatus(item)) {
					/*Call animation on item*/
					Animate(item);
					if (item.frames > 0) item.frames--;
					item.ran = true;
				}
				else {
					item.ran = false;
				}
			}

			/*Remove any items that are done*/
			bool keepGoing = true;
			std::vector<AnimItem> tempItems;
			while (keepGoing) {
				tempItems.clear();
				keepGoing = false;
				for (int i = 0; i < animList.size(); i++) {
					if (animList[i].ran && animList[i].frames == 0 && !animList[i].infinite) {
						keepGoing = true;
						for (int j = 0; j < animList.size(); j++) {
							if (i != j) tempItems.push_back(animList[j]);
						}
						animList.clear();
						break;
					}
				}
				if (tempItems.size() > 0) animList = tempItems;
			}

			for (int k = 0; k < animList.size(); k++) animList[k].ran = false;
		}
	}
}