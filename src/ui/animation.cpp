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
#include "animation.h"

namespace pen {
	namespace ui {
		std::vector<pen::ui::AnimationItem> Animation::animationList = {};

		void Animation::Add(pen::ui::Item* item, const unsigned int& type, const long& ms, const bool& infinite, const float& unitA, const float& unitB, const float& unitC) {
			/*Add animation to the queue*/
			int frames = 0;
			float deltaTime = 0.0035f;
			if (!infinite) frames = (ms / 1000) * (1.0f / deltaTime) * (ms / 1000); /*(ms / 1000) at end extra constant hack to make it more accurate*/

			pen::ui::AnimationItem newItem;
			newItem.item = item;
			newItem.type = type;
			newItem.infinite = infinite;
			newItem.frames = frames;
			newItem.ran = false;
			newItem.unitA = unitA * deltaTime / (ms / 1000) / (ms / 1000); /*(ms / 1000) at end extra constant hack to make it more accurate*/
			newItem.unitB = unitB * deltaTime / (ms / 1000) / (ms / 1000); /*(ms / 1000) at end extra constant hack to make it more accurate*/
			newItem.unitC = unitC * deltaTime / (ms / 1000) / (ms / 1000); /*(ms / 1000) at end extra constant hack to make it more accurate*/
			pen::ui::Animation::animationList.push_back(newItem);
		}

		void Animation::Run() {
			/*Runs the animations for each item*/
			if (!pen::ui::Animation::animationList.empty()) {
				for (auto& item : pen::ui::Animation::animationList) {
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
				std::vector<pen::ui::AnimationItem> tempItems;
				while (keepGoing) {
					tempItems.clear();
					keepGoing = false;
					for (int i = 0; i < pen::ui::Animation::animationList.size(); i++) {
						if (pen::ui::Animation::animationList[i].ran && pen::ui::Animation::animationList[i].frames == 0 && !pen::ui::Animation::animationList[i].infinite) {
							keepGoing = true;
							for (int j = 0; j < pen::ui::Animation::animationList.size(); j++) {
								if (i != j) tempItems.push_back(pen::ui::Animation::animationList[j]);
							}
							pen::ui::Animation::animationList.clear();
							break;
						}
					}
					if (tempItems.size() > 0) pen::ui::Animation::animationList = tempItems;
				}

				for (int k = 0; k < pen::ui::Animation::animationList.size(); k++) pen::ui::Animation::animationList[k].ran = false;

				pen::ui::Submit();
			}
		}

		bool Animation::CheckStatus(const pen::ui::AnimationItem& item) {
			/*If the item is already transformed from another animation that is not done then return true*/

			for (int i = 0; pen::ui::Animation::animationList.size(); i++) {
				if (pen::ui::Animation::animationList[i].ran && pen::ui::Animation::animationList[i].item == item.item && pen::ui::Animation::animationList[i].type == item.type) {
					return true;
				}
				else {
					return false;
				}
			}
		}

		void Animation::Animate(pen::ui::AnimationItem item) {
			/*Run the animation*/
			switch (item.type) {
			case 0:
				/*Translation, all units are used since there are three values*/
				pen::ui::Translate(item.item, pen::Vec3(item.unitA, item.unitB, item.unitC), true, true, false);
				break;
			case 1:
				/*Rotate X, unit A is used since there is only one value*/
				pen::ui::Rotate(item.item, item.unitA, 0, true, pen::Vec2(0.0f, 0.0f), true, false, true);
				break;
			case 2:
				/*Rotate Y, unit A is used since there is only one value*/
				pen::ui::Rotate(item.item, item.unitA, 1, true, pen::Vec2(0.0f, 0.0f), true, false, true);
				break;
			case 3:
				/*Rotate Z, unit A is used since there is only one value*/
				pen::ui::Rotate(item.item, item.unitA, 2, true, pen::Vec2(0.0f, 0.0f), true, false, true);
				break;
			case 4:
				/*Scale, two units are used since there are only two values*/
				pen::ui::Scale(item.item, pen::Vec2(item.unitA, item.unitB > 0 ? item.unitB : item.unitA), true, true, false);
				break;
			default:
				break;
			}
		}
	}
}