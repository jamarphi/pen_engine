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
#include "animation_ui.h"

namespace pen {
	std::vector<pen::AnimationUIItem> AnimationUI::animationList = {};

	void AnimationUI::Add(pen::ui::Item* item, const unsigned int& type, const long& ms, const bool& infinite, void (*onAnimationEndEvent)(pen::ui::Item*, unsigned int), void (*onCustomAnimationCallback)(pen::ui::Item*), const float& unitA, const float& unitB, const float& unitC, const float& unitD) {
		/*Add animation to the queue*/
		int frames = 0;
		float deltaTime = 0.0035f;
		if (!infinite) frames = ((float)ms / 1000.0f) * (1.0f / deltaTime) * ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/

		pen::AnimationUIItem newItem;
		newItem.item = item;
		newItem.type = type;
		newItem.infinite = infinite;
		newItem.frames = frames;
		newItem.ran = false;
		newItem.onAnimationEnd = onAnimationEndEvent;
		newItem.customAnimationCallback = onCustomAnimationCallback;
		if (type == pen::AnimationType::COLOR) {
			newItem.unitA = (unitA - item->GetColor()->x) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitB = (unitB - item->GetColor()->y) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitC = (unitC - item->GetColor()->z) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitD = (unitD - item->GetColor()->w) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		}
		else {
			newItem.unitA = unitA * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitB = unitB * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitC = unitC * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitD = unitD * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		}
		animationList.push_back(newItem);
	}

	void AnimationUI::Run() {
		/*Runs the animations for each item*/
		if (!pen::AnimationUI::animationList.empty()) {
			for (auto& item : pen::AnimationUI::animationList) {
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
			std::vector<pen::AnimationUIItem> tempItems;
			while (keepGoing) {
				tempItems.clear();
				keepGoing = false;
				for (int i = 0; i < pen::AnimationUI::animationList.size(); i++) {
					if (pen::AnimationUI::animationList[i].ran && pen::AnimationUI::animationList[i].frames == 0 && !pen::AnimationUI::animationList[i].infinite) {
						keepGoing = true;
						if (pen::AnimationUI::animationList[i].onAnimationEnd != nullptr) (*pen::AnimationUI::animationList[i].onAnimationEnd)(pen::AnimationUI::animationList[i].item, pen::AnimationUI::animationList[i].type);
						for (int j = 0; j < pen::AnimationUI::animationList.size(); j++) {
							if (i != j) tempItems.push_back(pen::AnimationUI::animationList[j]);
						}
						pen::AnimationUI::animationList.clear();
						break;
					}
				}
				if (tempItems.size() > 0) pen::AnimationUI::animationList = tempItems;
			}

			for (int k = 0; k < pen::AnimationUI::animationList.size(); k++) pen::AnimationUI::animationList[k].ran = false;

			/*All animations for gui items requires a batch submit*/
			pen::ui::Submit();
		}
	}

	bool AnimationUI::CheckStatus(const pen::AnimationUIItem& item) {
		/*If the item is already transformed from another animation that is not done then return true*/
		for (int i = 0; pen::AnimationUI::animationList.size(); i++) {
			if (pen::AnimationUI::animationList[i].ran && pen::AnimationUI::animationList[i].item == item.item && pen::AnimationUI::animationList[i].type == item.type) {
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}

	void AnimationUI::Animate(pen::AnimationUIItem item) {
		/*Run the animation*/
		switch (item.type) {
		case 0:
		case 1:
		case 2:
			/*Rotate, unit A is used since there is only one value*/
			pen::ui::Rotate(item.item, item.unitA, true);
			break;
		case 3:
			/*Translation, two units are used since there are two values*/
			pen::ui::Translate(item.item, pen::Vec2(item.unitA, item.unitB), true);
			break;
		case 4:
			/*Scale, two units are used since there are only two values*/
			pen::ui::Scale(item.item, pen::Vec2(item.unitA, item.unitB > 0 ? item.unitB : item.unitA), true);
			break;
		case 5:
		case 6:
		case 7:
			/*These cases are ignored since panning, looking, and zooming don't apply for gui items*/
			break;
		case 8:
			/*Updates the color, all four units are used since there are four values*/
			item.item->SetColor(pen::Vec4(
				item.item->GetColor()->x + item.unitA,
				item.item->GetColor()->y + item.unitB,
				item.item->GetColor()->z + item.unitC,
				item.item->GetColor()->w + item.unitD
			));
			break;
		case 9:
			if (item.customAnimationCallback != nullptr) (*item.customAnimationCallback)(item.item);
			break;
		default:
			break;
		}
	}
}
