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
#include "animation_3d.h"

namespace pen {
	std::vector<pen::Animation3DItem> Animation3D::animationList = {};

	void Animation3D::Add(pen::GraphicallyAcceleratedItem3D* item, const unsigned int& type, const long& ms, const bool& infinite, void (*onAnimationEndEvent)(pen::GraphicallyAcceleratedItem3D*, unsigned int), void (*onCustomAnimationCallback)(pen::GraphicallyAcceleratedItem3D*), const float& unitA, const float& unitB, const float& unitC, const float& unitD) {
		/*Add animation to the queue*/
		int frames = 0;
		float deltaTime = 0.0035f;
		if (!infinite) frames = ((float)ms / 1000.0f) * (1.0f / deltaTime) * ((float)ms / 1000.0f); /*((float)ms / 1000.0f) at end extra constant hack to make it more accurate*/

		pen::Animation3DItem newItem;
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
		pen::Animation3D::animationList.push_back(newItem);
	}

	bool Animation3D::CheckStatus(const pen::Animation3DItem& item) {
		/*If the item is already transformed from another animation that is not done then return true*/
		for (int i = 0; pen::Animation3D::animationList.size(); i++) {
			if (pen::Animation3D::animationList[i].ran && pen::Animation3D::animationList[i].item == item.item && pen::Animation3D::animationList[i].type == item.type) {
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}

	void Animation3D::Animate(Animation3DItem item) {
		/*Run the animation*/
		switch (item.type) {
		case 0:
		case 1:
		case 2:
			/*Rotate, unit A is used since there is only one value*/
			pen::ui::_3d::Rotate(item.item, item.unitA, item.type);
			break;
		case 3:
			/*Translation, three units are used since there are three values*/
			pen::ui::_3d::Translate(item.item, item.unitA, item.unitB, item.unitC);
			break;
		case 4:
			/*Scale, three units are used since there are three values*/
			pen::ui::_3d::Scale(item.item, item.unitA, item.unitB, item.unitC);
			break;
        case 5:
            /*Pan the layer camera, three units are used since there are three values*/
            pen::PanLayerCamera(item.unitA, item.unitB, item.unitC);
            break;
        case 6:
            /*Aim the layer camera, two units are used since there are two values*/
            pen::LookLayerCamera(item.unitA, item.unitB);
            break;
        case 7:
            /*Zoom is already handled with camera panning*/
            break;
		case 8:
			/*Updates the color, all four units are used since there are four values*/
			item.item->GetColor()->x += item.unitA;
			item.item->GetColor()->y += item.unitB;
			item.item->GetColor()->z += item.unitC;
			item.item->GetColor()->w += item.unitD;
			break;
		case 9:
			if (item.customAnimationCallback != nullptr) (*item.customAnimationCallback)(item.item);
			break;
		default:
			break;
		}
	}

	void Animation3D::Run() {
		/*Runs the animations for each item*/
		if (!pen::Animation3D::animationList.empty()) {
			for (auto& item : pen::Animation3D::animationList) {
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
			bool colorUpdate = false;
			std::vector<Animation3DItem> tempItems;
			while (keepGoing) {
				tempItems.clear();
				keepGoing = false;
				for (int i = 0; i < pen::Animation3D::animationList.size(); i++) {
					if (pen::Animation3D::animationList[i].type == pen::AnimationType::COLOR && pen::Animation3D::animationList[i].frames > 0) colorUpdate = true;
					if (pen::Animation3D::animationList[i].ran && pen::Animation3D::animationList[i].frames == 0 && !pen::Animation3D::animationList[i].infinite) {
						keepGoing = true;
						if (pen::Animation3D::animationList[i].onAnimationEnd != nullptr) (*pen::Animation3D::animationList[i].onAnimationEnd)(pen::Animation3D::animationList[i].item, pen::Animation3D::animationList[i].type);
						for (int j = 0; j < pen::Animation3D::animationList.size(); j++) {
							if (i != j) tempItems.push_back(pen::Animation3D::animationList[j]);
						}
						pen::Animation3D::animationList.clear();
						break;
					}
				}
				if (tempItems.size() > 0) pen::Animation3D::animationList = tempItems;
			}

			for (int k = 0; k < pen::Animation3D::animationList.size(); k++) pen::Animation3D::animationList[k].ran = false;

			/*Batch only needs to be submitted if color is updated since 3d transformations are done through the model matrix*/
			if (colorUpdate) pen::ui::Submit();
		}
	}
}
