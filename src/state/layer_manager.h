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
#include "../objects/layer.h"
#include "../objects/layer_3d.h"
#include "../core/color.h"
#include <fstream>
#include <sstream>

namespace pen {
	namespace ui {
		class LM {
			/*Layer manager class for layer state*/
		public:
			static std::vector<pen::Layer*> layers;
			static pen::Layer* pixelLayer;
			/*This id is hidden from the user and auto increments when a new layer is created*/
			static uint16_t generalLayerId;
		};

		static void Sort() {
			/*Sorts the layers to have fixed layers rendered last*/
			bool stillGoing = true;
			while (stillGoing) {
				stillGoing = false;
				for (int i = 0; i < pen::ui::LM::layers.size() - 1; i++) {
					/*Put all fixed layers after non-fixed layers*/
					if (pen::ui::LM::layers[i]->isFixed && !pen::ui::LM::layers[i + 1]->isFixed) {
						stillGoing = true;
						pen::Layer* tempLayer = pen::ui::LM::layers[i + 1];
						pen::ui::LM::layers[i + 1] = pen::ui::LM::layers[i];
						pen::ui::LM::layers[i] = tempLayer;
					}
				}
			}
		}

		static pen::ui::Item* AddItem(pen::ui::Item* item) {
			/*Adds a ui item to a layer that can be associated with it, this abstracts away management from the user

				Layers are separated based on their shape type, asset grouping id, and their isFixed status
			*/
			for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
				if (pen::ui::LM::layers.at(i)->assetGroupingId == item->assetGroupingId && pen::ui::LM::layers.at(i)->shapeType == item->shapeType &&
					pen::ui::LM::layers.at(i)->isFixed == item->isFixed && pen::ui::LM::layers.at(i)->isSingular == item->isSingular) {
					if (pen::ui::LM::layers.at(i)->itemCount + item->itemCount + 1 < MAX_OBJECTS) {
						/*Adds the item to an existing layer that has space available*/
						bool pushed = pen::ui::LM::layers.at(i)->Push(item);
						if (!pushed) break;
						pen::ui::LM::layers.at(i)->Update();
						return item;
					}
					else {
						/*Creates a new layer for this asset group, shape type, and/or isFixed status*/
						pen::ui::LM::layers.push_back(new pen::Layer(pen::ui::LM::generalLayerId,
							item->assetGroupingId, item->shapeType, item->isFixed, item->isSingular, item->isWireFrame));
						pen::ui::LM::generalLayerId++;
						pen::ui::LM::layers.at(pen::ui::LM::layers.size() - 1)->Push(item);
						pen::ui::LM::layers.at(pen::ui::LM::layers.size() - 1)->Initialize();
						pen::ui::Sort();
						return item;
					}
				}
			}

			/*Create the first layer for this asset group, shape type, and/or isFixed status*/
			pen::ui::LM::layers.push_back(new pen::Layer(pen::ui::LM::generalLayerId,
				item->assetGroupingId, item->shapeType, item->isFixed, item->isSingular, item->isWireFrame));
			pen::ui::LM::generalLayerId++;
			pen::ui::LM::layers[pen::ui::LM::layers.size() - 1]->Push(item);
			pen::ui::LM::layers[pen::ui::LM::layers.size() - 1]->Initialize();
			pen::ui::Sort();
			return item;
		}

		static pen::ui::Item* AddChildItem(pen::ui::Item* parentItem, pen::ui::Item* childItem) {
			/*First find the layer the parent item is connected to*/
			pen::Layer* parentItemLayer = nullptr;
			for (auto& layer : pen::ui::LM::layers) {
				for (int i = 0; i < layer->layerItems.size(); i++) {
					if (parentItem == layer->layerItems[i]) {
						parentItemLayer = layer;
						break;
						break;
					}
				}
			}

			if (parentItemLayer == nullptr) return childItem;

			/*Check if the layer has enough room*/
			if (parentItemLayer->itemCount + childItem->itemCount + 1 <= MAX_OBJECTS) {
				/*Add the child item to this parent item*/
				parentItem->Push(childItem);
				return childItem;
			}
			else {
				parentItem->Push(childItem);
				std::vector<pen::ui::Item*> tempItems;

				/*Remove pointer from old layer*/
				for (auto& item : parentItemLayer->layerItems) {
					if (item != parentItem) tempItems.push_back(item);
				}
				parentItemLayer->layerItems = tempItems;

				/*Move parentItem with childItem to new layer*/
				for (int j = 0; j < pen::ui::LM::layers.size(); j++) {
					if (pen::ui::LM::layers[j] != parentItemLayer &&
						(pen::ui::LM::layers[j]->itemCount + parentItem->itemCount + 1 < MAX_OBJECTS) &&
						pen::ui::LM::layers[j]->shapeType == parentItemLayer->shapeType && pen::ui::LM::layers[j]->assetGroupingId == parentItemLayer->assetGroupingId) {
						/*Add to an existing layer*/
						pen::ui::LM::layers[j]->Push(parentItem);
						return childItem;
					}
				}

				/*Create a new layer for this item*/
				pen::ui::LM::layers.push_back(new pen::Layer(pen::ui::LM::generalLayerId,
					parentItem->assetGroupingId, parentItem->shapeType, parentItem->isFixed, parentItem->isSingular, parentItem->isWireFrame));
				pen::ui::LM::generalLayerId++;
				pen::ui::LM::layers.at(pen::ui::LM::layers.size() - 1)->Push(parentItem);
				pen::ui::LM::layers.at(pen::ui::LM::layers.size() - 1)->Initialize();
				pen::ui::Sort();
				return childItem;
			}
		}

		static pen::ui::Item* FindItem(uint32_t id) {
			/*Finds a specific item based on its id*/
			pen::ui::Item* tempItem = nullptr;

			for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
				for (int j = 0; j < pen::ui::LM::layers.at(i)->layerItems.size(); j++) {
					tempItem = pen::ui::LM::layers.at(i)->layerItems[j]->FindItem(id);
					if (tempItem != nullptr) {
						return tempItem;
					}
				}
			}
			return tempItem;
		}

		static void RemoveItemById(uint32_t id) {
			/*Removes a specific item based on its id*/

			for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
				for (int j = 0; j < pen::ui::LM::layers.at(i)->layerItems.size(); j++) {
					/*Remove all nested items with this id*/
					pen::ui::LM::layers.at(i)->layerItems[j]->RemoveItemById(id);

					if (pen::ui::LM::layers.at(i)->layerItems[j]->id == id) {
						/*Remove this main item from the layer since it also has this id*/
						std::vector<pen::ui::Item*> tempLayerItems;
						pen::ui::Item* itemToDelete = nullptr;
						for (int k = 0; k < pen::ui::LM::layers.at(i)->layerItems.size(); k++) {
							if (pen::ui::LM::layers.at(i)->layerItems[k]->id != id) {
								tempLayerItems.push_back(pen::ui::LM::layers.at(i)->layerItems[k]);
							}
							else {
								itemToDelete = pen::ui::LM::layers.at(i)->layerItems[k];
							}
						}
						pen::ui::LM::layers.at(i)->layerItems = tempLayerItems;
						delete itemToDelete;
					}
				}
			}
		}

		static void RemoveItem(pen::ui::Item* id) {
			/*Removes a specific item based on its id*/

			for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
				for (int j = 0; j < pen::ui::LM::layers.at(i)->layerItems.size(); j++) {
					/*Remove all nested items with this id*/
					bool removed = pen::ui::LM::layers.at(i)->layerItems[j]->RemoveItem(id);

					if (removed) {
						break;
						break;
					}

					if (pen::ui::LM::layers.at(i)->layerItems[j] == id) {
						/*Remove this main item from the layer since it has this id*/
						std::vector<pen::ui::Item*> tempLayerItems;
						pen::ui::Item* itemToDelete = nullptr;
						for (int k = 0; k < pen::ui::LM::layers.at(i)->layerItems.size(); k++) {
							if (pen::ui::LM::layers.at(i)->layerItems[k] != id) {
								tempLayerItems.push_back(pen::ui::LM::layers.at(i)->layerItems[k]);
							}
							else {
								itemToDelete = pen::ui::LM::layers.at(i)->layerItems[k];
							}
						}
						pen::ui::LM::layers.at(i)->layerItems = tempLayerItems;
						if (itemToDelete != nullptr) {
							itemToDelete->RemoveNestedItems(itemToDelete);
							delete itemToDelete;
						}
						break;
						break;
					}
				}
			}
		}

		static void Submit() {
			/*This updates the layers with any modified changes done by the user*/
			for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
				for (int j = 0; j < pen::ui::LM::layers.at(i)->layerItems.size(); j++) {
					pen::ui::LM::layers.at(i)->layerItems.at(j)->CombineChildBuffers();
				}
				pen::ui::LM::layers.at(i)->CombineBuffers();
			}
			pen::State::Get()->firstUpdateFrame = true;
			pen::State::Get()->updateBatch = true;
		}

		static void PixelLayerAlloc() {
			/*
			Dimensions of pixel by default is 1,000 in width by 500 in height.
			500,000 pixels, four values per pixel.
			*/
#ifndef __PEN_MOBILE__
			unsigned char* dataArray = new unsigned char[2000000];

			/*Sets color for each pixel*/
			for (int i = 0; i < 2000000; i += 4) {
				dataArray[i] = 0;
				dataArray[i + 1] = 0;
				dataArray[i + 2] = 0;
				dataArray[i + 3] = 0;
			}

			/*Set this pixel array to the pixel layer*/
			pen::ui::LM::pixelLayer->pixels = dataArray;
			pen::State::Get()->pixelArray = dataArray;
#endif
		}

		static bool InitializePixelBuffer() {
			/*Create a layer specifically for pixel-by-pixel drawing*/
			if (pen::ui::LM::pixelLayer != nullptr) return true;
			pen::State::Get()->usingBuffer = true;
			pen::ui::LM::pixelLayer = new pen::Layer(pen::ui::LM::generalLayerId, 0, pen::ui::Shape::BUFFER, true, false, false);
			pen::ui::LM::layers.push_back(pen::ui::LM::pixelLayer);
			pen::ui::Sort();
			pen::ui::LM::generalLayerId++;

			/*
				Allocate an array of points for dealing with pixel drawing.
			*/
			pen::State* tempStateInst = pen::State::Get();
			pen::ui::LM::pixelLayer->Push(new pen::ui::Item(PIXEL_DRAWING_ID,
				pen::Vec3(0.0f, 0.0f, 0.0f),
				pen::Vec2(tempStateInst->screenWidth, tempStateInst->screenHeight),
				pen::ui::Shape::BUFFER, pen::PEN_WHITE, nullptr, nullptr, true, "pixel"));
			pen::ui::PixelLayerAlloc();
			pen::ui::LM::pixelLayer->Initialize();



			/*Sorts this layer before other fixed layers and after non-fixed layers*/
			if (pen::ui::LM::layers.size() > 1) {
				int fixedLayers = -1;
				for (int i = 0; i < pen::ui::LM::layers.size(); i++) {
					if (pen::ui::LM::layers.at(i)->isFixed == true && pen::ui::LM::layers.at(i)->shapeType != pen::ui::Shape::BUFFER) {
						fixedLayers = i;
						break;
					}
				}
				if (fixedLayers > -1) {
					/*Put pixel-by-pixel layer before other fixed layers, but after non-fixed layers*/
					pen::Layer* bufferLayer = pen::ui::LM::pixelLayer;
					pen::Layer* firstTempLayer = nullptr;
					pen::Layer* tempLayer = nullptr;
					for (int j = fixedLayers; j < pen::ui::LM::layers.size() - 1; j++) {
						if (j > fixedLayers) {
							tempLayer = pen::ui::LM::layers[j + 1]; //next index
							pen::ui::LM::layers[j + 1] = firstTempLayer; //current index
							firstTempLayer = tempLayer; //set current layer to next layer
						}
						else {
							firstTempLayer = pen::ui::LM::layers[j + 1];
							pen::ui::LM::layers[j + 1] = pen::ui::LM::layers[j];
						}
					}

					/*Place the buffer shape type fixed layer at the index for it to be the first fixed layer*/
					pen::ui::LM::layers[fixedLayers] = bufferLayer;
				}
				return true;
			}
			return true;
		}

		static std::string Split(const std::string& line, const char& character, const unsigned int& section) {
			/*Split a line by a given character*/
			int counter = 0;
			int previousSectionIdx = 0;
			bool keepGoing = true;
			if (line.find(character) != std::string::npos) {
				while (keepGoing) {
					keepGoing = false;
					if (line.find(character, previousSectionIdx + 1) != std::string::npos) {
						if (counter == section) {
							if (counter > 0) {
								int startIdx = previousSectionIdx + 1;
								int endIdx = line.find(character, previousSectionIdx + 1);

								return line.substr(startIdx, endIdx - startIdx);
							}
							else {
								return line.substr(0, line.find(character));
							}
						}
						else {
							previousSectionIdx = line.find(character, previousSectionIdx + 1);
							counter++;
							keepGoing = true;
						}
					}
				}
				return "";
			}
			else {
				return line;
			}
		}

		static void AddItem3D(const uint32_t& id, const std::string& path, const pen::Vec4& objectColor, const bool& objectIsFixed, const bool& isWireFrame) {
			/*Scans an obj file for the vertices and indices*/
			std::string tempPath = (path.find(":") != std::string::npos) ? path : GENERAL_MODEL_SOURCE + path;

			std::ifstream modelFile;
			modelFile.open(tempPath);
			if (modelFile.is_open()) {
				/*Create the 3D layer and item*/
				unsigned int faceCounter = 0;
				bool quadFormatError = false;
				std::vector<float> buffPositions;
				pen::Vec3 vertex1 = pen::Vec3(0.0f, 0.0f, 0.0f);
				bool firstVertex = true;
				bool normalize = false;

				pen::ui::Item* item3D = new pen::Item3D(id, pen::Vec3(0.0f, 0.0f, 0.0f), pen::Vec2(50.0f, 50.0f),
					objectColor, objectIsFixed);
				item3D->isWireFrame = isWireFrame;

				while (!modelFile.eof()) {
					/*It is assumed that the vertex points in the obj file will be normalized already*/
					char fileLine[150];
					std::stringstream stream;
					std::string firstElement;
					std::string point1;
					std::string point2;
					std::string point3;
					std::string point4;
					int index1 = 0;
					int index2 = 0;
					int index3 = 0;
					int index4 = 0;
					modelFile.getline(fileLine, 150);
					stream << fileLine;

					if (fileLine[0] == 'v' && fileLine[1] == ' ') {
						/*Vertex point*/
						stream >> firstElement >> vertex1.x >> vertex1.y >> vertex1.z;

						if (firstVertex) {
							/*Divides all vertex points by 10 if one of the first vertex points is greater than 10*/
							if (pen::op::Abs(vertex1.x) > 10.0f || pen::op::Abs(vertex1.y) > 10.0f
								|| pen::op::Abs(vertex1.z) > 10.0f) {
								vertex1.x /= 10.0f;
								vertex1.y /= 10.0f;
								vertex1.z /= 10.0f;
								normalize = true;
								firstVertex = false;
							}
						}
						else {
							if (normalize) {
								vertex1.x /= 10.0f;
								vertex1.y /= 10.0f;
								vertex1.z /= 10.0f;
							}
						}

						buffPositions = {
							vertex1.x, vertex1.y, vertex1.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
						};

						item3D->Push(new pen::Item3D(true, pen::Vec3(vertex1.x, vertex1.y, vertex1.z),
							pen::ui::Shape::POINT, item3D->color, item3D->isFixed, &buffPositions[0]));
					}
					else if (fileLine[0] == 'f') {
						/*Face data*/
						stream >> firstElement >> point1 >> point2 >> point3 >> point4;
						std::string faceIndex1 = pen::ui::Split(point1, '/', 0);
						std::string faceIndex2 = pen::ui::Split(point2, '/', 0);
						std::string faceIndex3 = pen::ui::Split(point3, '/', 0);
						std::string faceIndex4 = point4.size() > 0 ? pen::ui::Split(point4, '/', 0) : "";

						/*Obj files indices start at 1*/
						index1 = std::stoi(faceIndex1) - 1;
						index2 = std::stoi(faceIndex2) - 1;
						index3 = std::stoi(faceIndex3) - 1;
						index4 = faceIndex4.size() > 0 ? std::stoi(faceIndex4) - 1 : -1;

						/*Set the indices to the item indices array*/
						item3D->indices3D[(faceCounter * 3)] = index1;
						item3D->indices3D[(faceCounter * 3) + 1] = index2;
						item3D->indices3D[(faceCounter * 3) + 2] = index3;

						if (index4 > -1) {
							/*Quad*/
							item3D->indices3D[(faceCounter * 3) + 3] = index3;
							item3D->indices3D[(faceCounter * 3) + 4] = index4;
							item3D->indices3D[(faceCounter * 3) + 5] = index2;
							faceCounter += 2;
							item3D->indexCount3D += 6;
						}
						else {
							faceCounter++;
							item3D->indexCount3D += 3;
						}
					}
				}

				modelFile.close();

				/*Check if the element count doesn't match based on the last triangle added*/
				if (buffPositions.size() % BATCH_VERTEX_ELEMENTS != 0) {
					std::cout << "buffPositions is not padded correctly to match BATCH_VERTEX_ELEMENTS" << std::endl;
#ifndef __PEN_MOBILE__
					glfwTerminate();
#endif
				}

				item3D->CombineChildBuffers();

				/*Initialize the layers based on the triangle count*/
				for (int i = 0; i < faceCounter; i += MAX_OBJECTS) {
					pen::ui::LM::layers.push_back(new pen::Layer3D(pen::ui::LM::generalLayerId,
						item3D->assetGroupingId, item3D->shapeType, item3D->isFixed, item3D->isSingular, item3D->isWireFrame));
					pen::ui::LM::generalLayerId++;
					pen::ui::LM::layers[pen::ui::LM::layers.size() - 1]->Push(item3D, i);
					pen::ui::LM::layers[pen::ui::LM::layers.size() - 1]->Initialize();
					pen::ui::Sort();
				}
			}
		}
	}
}