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
#include "animation_pixel_2d.h"

namespace pen {
	std::vector<pen::AnimationPixelItem> AnimationPixel::animationList = {};
	
	void AnimationPixel::Add(pen::Item* item, const unsigned int& type, const long& ms, const bool& infinite, void (*onAnimationEndEvent)(pen::Item*, unsigned int), void (*onCustomAnimationCallback)(pen::Item*), const float& unitA, const float& unitB, const float& unitC, const float& unitD) {
		/*Add animation to the queue*/
		int frames = 0;
		float deltaTime = 0.0035f;
		if (!infinite) frames = ((float)ms / 1000.0f) * (1.0f / deltaTime) * ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/

		pen::AnimationPixelItem newItem;
		newItem.item = item;
		newItem.type = type;
		newItem.infinite = infinite;
		newItem.frames = frames;
		newItem.ran = false;
		newItem.onAnimationEnd = onAnimationEndEvent;
		newItem.customAnimationCallback = onCustomAnimationCallback;
		if (type == pen::AnimationType::COLOR) {
			newItem.unitA = (unitA - item->color.x) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitB = (unitB - item->color.y) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitC = (unitC - item->color.z) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitD = (unitD - item->color.w) * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		}
		else {
			newItem.unitA = unitA * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitB = unitB * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitC = unitC * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
			newItem.unitD = unitD * deltaTime / ((float)ms / 1000.0f) / ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/
		}
		pen::AnimationPixel::animationList.push_back(newItem);
	}

	bool AnimationPixel::CheckStatus(const pen::AnimationPixelItem& item) {
		/*If the item is already transformed from another animation that is not done then return true*/
		for (int i = 0; pen::AnimationPixel::animationList.size(); i++) {
			if (pen::AnimationPixel::animationList[i].ran && pen::AnimationPixel::animationList[i].item == item.item && pen::AnimationPixel::animationList[i].type == item.type) {
				return true;
			}
			else {
				return false;
			}
		}
        return false;
	}

	void AnimationPixel::Animate(AnimationPixelItem item) {
		/*Run the animation*/
		switch (item.type) {
		case 0:
		case 1:
		case 2:
			/*Rotate, unit A is used since there is only one value*/
			pen::Rotate(item.item, item.unitA);
			break;
		case 3:
			/*Translation, three units are used since there are three values*/
			pen::Translate(item.item, item.unitA, item.unitB);
			break;
		case 4:
			/*Scale, two units are used since there are only two values*/
			pen::Scale(item.item, item.unitA, item.unitB > 0 ? item.unitB : item.unitA);
			break;
        case 5:
            /*Pan the pixel buffer, two units are used since there are two values*/
			pen::PanLayerCamera(item.unitA, item.unitB, 0.0f);
            break;
        case 6:
            /*Case 6 is ignored since this is 2D so there is no camera aiming*/
            break;
        case 7:
            /*Zoom the pixel buffer, only one unit is used since there is one value*/
			pen::PanLayerCamera(0.0f, 0.0f, item.unitA);
            break;
		case 8:
			/*Updates the color, all four units are used since there are four values*/
			item.item->color.x += item.unitA;
			item.item->color.y += item.unitB;
			item.item->color.z += item.unitC;
			item.item->color.w += item.unitD;
			break;
		case 9:
			if (item.customAnimationCallback != nullptr) (*item.customAnimationCallback)(item.item);
			break;
		default:
			break;
		}
	}

	void AnimationPixel::Run() {
		/*Runs the animations for each item*/
		if (!pen::AnimationPixel::animationList.empty()) {
			for (auto& item : pen::AnimationPixel::animationList) {
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
			std::vector<AnimationPixelItem> tempItems;
			while (keepGoing) {
				tempItems.clear();
				keepGoing = false;
				for (int i = 0; i < pen::AnimationPixel::animationList.size(); i++) {
					if (pen::AnimationPixel::animationList[i].ran && pen::AnimationPixel::animationList[i].frames == 0 && !pen::AnimationPixel::animationList[i].infinite) {
						keepGoing = true;
						if (pen::AnimationPixel::animationList[i].onAnimationEnd != nullptr) (*pen::AnimationPixel::animationList[i].onAnimationEnd)(pen::AnimationPixel::animationList[i].item, pen::AnimationPixel::animationList[i].type);
						for (int j = 0; j < pen::AnimationPixel::animationList.size(); j++) {
							if (i != j) tempItems.push_back(pen::AnimationPixel::animationList[j]);
						}
						pen::AnimationPixel::animationList.clear();
						break;
					}
				}
				if (tempItems.size() > 0) pen::AnimationPixel::animationList = tempItems;
			}

			for (int k = 0; k < pen::AnimationPixel::animationList.size(); k++) pen::AnimationPixel::animationList[k].ran = false;
		}
	}
}
