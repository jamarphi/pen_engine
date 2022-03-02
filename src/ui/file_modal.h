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
#ifndef __PEN_LINUX__
#include "../objects/item.h"
#include "text_box.h"
#include "vertical_list.h"
#include "button.h"
#include "../util/dirent.h"

namespace pen {
	namespace ui {
		enum FILE_INTENT {
			SAVE_AS = 0,
			SELECT_DIRECTORY = 1,
			SELECT_FILE = 2,
			SELECT_FILES = 3
		};

		class FileModal : public pen::ui::Item {
		public:
			pen::ui::Item* list;
			pen::ui::Item* cancelBtn;
			pen::ui::Item* confirmBtn;
			pen::ui::Item* fileInput;
		public:
			FileModal();
			FileModal(uint32_t objectId, unsigned int userIntent, std::string userMimeType, pen::Vec4 objectColor = pen::Vec4(1.0f, 1.0f, 1.0f, 1.0f), pen::Vec4 objectAccentColor = pen::Vec4(0.8f, 0.8f, 0.8f, 1.0f), 
				pen::ui::Item* objectParent = nullptr, void (*onPathLoadCallback)(pen::ui::Item*, std::string, bool) = nullptr, std::string objectTextureName = "", float itemTexCoordStartX = 0.0f, float itemTexCoordStartY = 0.0f,
				float itemTexCoordEndX = 1.0f, float itemTexCoordEndY = 1.0f);
			~FileModal();

			void UpdateFileModalListing(std::string directoryPath, bool recursive = false, bool firstIter = true);
		};
	}
}
#endif