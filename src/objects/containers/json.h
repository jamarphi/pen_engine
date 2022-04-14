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
#include "map.h"
#include "../../state/state.h"
#include <string>

namespace pen {
	class JSON {
	public:
		struct Field {
			std::string value;
			/*
			* a: array
			* b: bool
			* c: char
			* f: float
			* i: int
			* j: sub json object
			* s: string
			*/
			char type;
			int numBytes;

			template<typename T> T Get() {
				/*Returns a float since float is a nonstandard template parameter, a JSON object, or a list of arrays since JSON and JSONArray can't be defined twice*/
				if (JSON::CheckForNumber(value)) {
					return std::stof(value);
				}
				else if (type == 'a') {
					return JSON::ParseArrays(value);
				}
				else {
					return JSON(value);
				}
			}

			template<typename int> int Get() {
				/*Returns an integer*/
				return std::stoi(value);
			}

			template<typename bool> bool Get() {
				/*Returns a boolean*/
				return value == "true" ? true : false;
			}

			template<typename char> char Get() {
				/*Returns a char*/
				return value[0];
			}

			template<typename char*> std::string Get() {
				/*Returns a string*/
				return value;
			}
		};

		class Array {
		public:
			std::vector<Array> children;
			bool nested;
			char type;
		private:
			std::vector<JSON> jsonList;
			std::vector<int> intList;
			std::vector<float> floatList;
			std::vector<bool> boolList;
			std::vector<char> charList;
			std::vector<std::string> stringList;

		public:
			Array() {
				children = {};
				nested = false;
				type = '\0';
				intList = {};
				floatList = {};
				boolList = {};
				charList = {};
				stringList = {};
			}

			Array(std::string data, std::vector<Array> arrChildren, char type) {
				/*If data is empty when passed in then this is an array of arrays*/std::vector<JSON> jsonList = {};
				intList = {};
				floatList = {};
				boolList = {};
				charList = {};
				stringList = {};
				nested = false;
				if (arrChildren.size() > 0) {
					nested = true;
					children = arrChildren;
				}

				if (data != "") {
					/*Since it's not an array of arrays, makes sure beginning and end brackets are present*/
					if (data[0] != '[') data = '[' + data;
					if (data[data.length() - 1] != ']') data += ']';
					switch (type) {
					case 'j':
						jsonList = JSON::ParseArray(data);
						break;
					case 'i':
						intList = JSON::ParseInts(data);
						break;
					case 'f':
						floatList = JSON::ParseFloats(data);
						break;
					case 'b':
						boolList = JSON::ParseBools(data);
						break;
					case 'c':
						charList = JSON::ParseChars(data);
						break;
					case 's':
						stringList = JSON::ParseStrings(data);
						break;
					default:
						break;
					}
				}
			}

			template<typename T> std::vector<T> GetList() {
				/*Returns a list of floats since float is a nonstandard template parameter, a JSON object, or a list of arrays since JSON and JSONArray can't be defined twice*/
				if (floatList.size() > 0) {
					return floatList;
				}
				else if (jsonList.size() > 0) {
					return jsonList;
				}
				else {
					children;
				}
			}

			template<typename int> std::vector<int> GetList() {
				/*Returns a list of integers*/
				return intList;
			}

			template<typename bool> std::vector<bool> GetList() {
				/*Returns a list of booleans*/
				return boolList;
			}

			template<typename char> std::vector<char> GetList() {
				/*Returns a list of chars*/
				return charList;
			}

			template<typename char*> std::vector<std::string> GetList() {
				/*Returns a list of booleans*/
				return boolList;
			}

			static std::vector<Array> ParseArrays(std::string data) {
				/*Parse an array of arrays*/
				std::string::iterator it;
				std::string arrStr = "";
				std::string arrStrValue = "";
				char type = '\0';
				int counter = 0;
				bool arrayOpen = false;
				int nestLevel = 0;
				std::vector<Array> arrayList;
				for (it = data.begin(); it != data.end(); it++) {
					if (counter > 0) {
						if (*it == ']' && !arrayOpen) {
							/*Main array is done*/
							break;
						}
						else if (*it == '[' || *it == ']') {
							/*Sub array*/
							if ((*it == '[' && !arrayOpen) || (*it == ']' && arrayOpen && nestLevel == 1)) {
								arrayOpen = !arrayOpen;
								if (arrayOpen) {
									nestLevel++;
								}
								else {
									nestLevel--;
								}

								if (arrayOpen) {
									/*First character is added so it can be parsed correctly in ParseArray()*/
									arrStr += *it;
								}
								else {
									/*Add ending ] bracket*/
									arrStr += *it;

									/*Check for type of array*/
									arrStrValue = arrStr.substr(1);
									size_t objectPos = arrStrValue.find("{") > 0 ? arrStrValue.find("{") : 0;
									size_t arrPos = arrStrValue.find("[") > 0 ? arrStrValue.find("[") : 0;
									if (objectPos < arrPos) {
										/*This is an object array*/
										arrayList.push_back(Array(arrStr, {}, 'j'));
									}
									else {
										arrStrValue = arrStrValue.substr(0, arrStrValue.find(",") != std::string::npos ? arrStrValue.find(",") : arrStrValue.length() - 1);

										if (arrPos != std::string::npos) {
											/*This is an array of arrays*/
											arrayList.push_back(Array("", ParseArrays(arrStr), 'a'));
										}
										else {
											bool isNumber = JSON::CheckForNumber(arrStrValue);
											if (isNumber) {
												/*Int or float*/
												if (arrStrValue.find(".") != std::string::npos) {
													/*Type is float*/
													arrayList.push_back(Array(arrStr, {}, 'f'));
												}
												else {
													/*Type is integer*/
													arrayList.push_back(Array(arrStr, {}, 'i'));
												}
											}
											else {
												if (arrStrValue == "true" || arrStrValue == "false") {
													/*Type is boolean*/
													arrayList.push_back(Array(arrStr, {}, 'b'));
												}
												else if (arrStrValue.length() == 1) {
													/*Type is char*/
													arrayList.push_back(Array(arrStr, {}, 'c'));
												}
												else {
													/*Type is string*/
													arrayList.push_back(Array(arrStr, {}, 's'));
												}
											}
										}
									}
									arrStr = "";
								}
							}
							else if (*it == '[' && arrayOpen) {
								nestLevel++;
								arrStr += *it;
							}
							else if (*it == ']' && arrayOpen) {
								nestLevel--;
								arrStr += *it;
							}
						}
						else {
							if (arrayOpen) {
								arrStr += *it;
							}
						}
					}
				}
			}
		};

	public:
		pen::Map<std::string, Field> json;

	public:
		JSON() {

		}

		JSON(std::string jsonStr) {
			json = ParseJSON(jsonStr);
		}

		static std::vector<int> ParseInts(std::string data) {
			/*Parse an array of integers*/
			std::string::iterator it;
			std::string intStr = "";
			std::vector<int> intList;
			for (it = data.begin(); it != data.end(); it++) {
				if (*it == '[' || *it == ']' || *it == ',') {
					if (*it == ',') {
						intList.push_back(std::stoi(intStr));
						intStr = "";
					}
					else {
						continue;
					}
				}
				else {
					intStr += *it;
				}		
			}
			return intList;
		}

		static std::vector<float> ParseFloats(std::string data) {
			/*Parse an array of floats*/
			std::string::iterator it;
			std::string floatStr = "";
			std::vector<float> floatList;
			for (it = data.begin(); it != data.end(); it++) {
				if (*it == '[' || *it == ']' || *it == ',') {
					if (*it == ',') {
						floatList.push_back(std::stof(floatStr));
						floatStr = "";
					}
					else {
						continue;
					}
				}
				else {
					floatStr += *it;
				}
			}
			return floatList;
		}

		static std::vector<char> ParseChars(std::string data) {
			/*Parse an array of chars*/
			std::string::iterator it;
			std::string charStr = "";
			std::vector<char> charList;
			for (it = data.begin(); it != data.end(); it++) {
				if (*it == '[' || *it == ']' || *it == ',') {
					if (*it == ',') {
						charList.push_back(charStr[0]);
						charStr = "";
					}
					else {
						continue;
					}
				}
				else {
					charStr += *it;
				}
			}
			return charList;
		}

		static std::vector<bool> ParseBools(std::string data) {
			/*Parse an array of booleans*/
			std::string::iterator it;
			std::string boolStr = "";
			std::vector<bool> boolList;
			for (it = data.begin(); it != data.end(); it++) {
				if (*it == '[' || *it == ']' || *it == ',') {
					if (*it == ',') {
						boolList.push_back(boolStr == "true" ? true : false);
						boolStr = "";
					}
					else {
						continue;
					}
				}
				else {
					boolStr += *it;
				}
			}
			return boolList;
		}

		static std::vector<std::string> ParseStrings(std::string data) {
			/*Parse an array of strings*/
			std::string::iterator it;
			std::string str = "";
			std::vector<std::string> stringList;
			for (it = data.begin(); it != data.end(); it++) {
				if (*it == '[' || *it == ']' || *it == ',') {
					if (*it == ',') {
						stringList.push_back(str);
						str = "";
					}
					else {
						continue;
					}
				}
				else {
					str += *it;
				}
			}
			return stringList;
		}

	private:
		pen::Map<std::string, Field> ParseJSON(std::string jsonStr) {
			/*Parse the json data into a map*/
			pen::Map<std::string, Field> jsonMap;
			std::string::iterator it;
			int counter = 0;
			int subCounter = 0;
			bool objectOpen = false;
			bool arrayOpen = false;
			bool printKey = true;
			std::string key = "";
			std::string value = "";
			int nestLevel = 0;
			char type = '\0';
			for (it = jsonStr.begin(); it != jsonStr.end(); it++) {
				if (counter > 0) {
					if (*it == '}' && !objectOpen && !arrayOpen) {
						/*JSON object is done*/
						break;
					}
					else if (*it == '"' || *it == '\'') {
						if(value.length() > 0)
						continue;
					}
					else if(*it == '[' || *it == ']') {
						/*Array*/
						if (!objectOpen) {
							if ((*it == '[' && !arrayOpen) || (*it == ']' && arrayOpen && nestLevel == 1)) {
								arrayOpen = !arrayOpen;
								type = 'a';
								if (arrayOpen) {
									nestLevel++;
								}
								else {
									nestLevel--;
								}
							}
							else if (*it == '[' && arrayOpen) {
								nestLevel++;
							}
							else if (*it == ']' && arrayOpen) {
								nestLevel--;
							}
							value += *it;
						}
					}
					else if (*it == '{' || *it == '}') {
						/*Sub JSON object*/
						if (!arrayOpen) {
							if ((*it == '{' && !objectOpen) || (*it == '}' && objectOpen && nestLevel == 1)) {
								objectOpen = !objectOpen;
								type = 'j';
								if (objectOpen) {
									nestLevel++;
								}
								else {
									nestLevel--;
								}
							}
							else if (*it == '{' && objectOpen) {
								nestLevel++;
							}
							else if (*it == '}' && objectOpen) {
								nestLevel--;
							}
							value += *it;
						}
					}
					else {
						if (objectOpen || arrayOpen) {
							value += *it;
						}
						else {
							if (*it == ',') {
								printKey = true;
								if (type != 'a' && type != 'j') {
									/*Determine the primitive type*/
									bool isNumber = CheckForNumber(value);
									if (isNumber) {
										/*Int or float*/
										if (value.find(".") != std::string::npos) {
											/*Type is float*/
											type = 'f';
										}
										else {
											/*Type is integer*/
											type = 'i';
										}
									}
									else {
										if (value == "true" || value == "false") {
											/*Type is boolean*/
											type = 'b';
										}
										else if (value.length() == 1) {
											/*Type is char*/
											type = 'c';
										}
										else {
											/*Type is string*/
											type = 's';
										}
									}
								}
								jsonMap.Insert(key, {value, type, subCounter});
								subCounter = 0;
								type = '\0';
								key = "";
								value = "";
								objectOpen = false;
								arrayOpen = false;
							}
							else if (*it == ':') {
								printKey = false;
							}
							else {
								if (printKey) {
									key += *it;
								}
								else {
									value += *it;
									subCounter++;
								}
							}
						}
					}
				}
				counter++;
			}
			return jsonMap;
		}

		static bool CheckForNumber(std::string str) {
			/*Check if string is number*/
			std::string::iterator it;
			for (it = str.begin(); it != str.end(); it++) {
				if (std::isdigit(*it) == 0 && *it != '.') return false;
			}
			return true;
		}

		static std::vector<JSON> ParseArray(std::string data) {
			/*Parse the array*/
			std::string::iterator it;
			std::string jsonStr = "";
			int counter = 0;
			bool objectOpen = false;
			std::vector<JSON> jsonObjects;
			int nestLevel = 0;
			for (it = data.begin(); it != data.end(); it++) {
				if (counter > 0) {
					if (*it == '}' && !objectOpen) {
						/*JSON object is done*/
						break;
					}
					else if (*it == '{' || *it == '}') {
						/*Sub JSON object*/
						if ((*it == '{' && !objectOpen) || (*it == '}' && objectOpen && nestLevel == 1)) {
							objectOpen = !objectOpen;
							if (objectOpen) {
								nestLevel++;
							}
							else {
								nestLevel--;
							}

							if (objectOpen) {
								/*First character is added so it can be parsed correctly in ParseJSON()*/
								jsonStr += *it;
							}
							else {
								/*Add ending } bracket*/
								jsonStr += *it;
								jsonObjects.push_back(JSON(jsonStr));
								jsonStr = "";
							}
						}
						else if (*it == '{' && objectOpen) {
							nestLevel++;
							jsonStr += *it;
						}
						else if (*it == '}' && objectOpen) {
							nestLevel--;
							jsonStr += *it;
						}
					}
					else {
						if (objectOpen) {
							jsonStr += *it;
						}
					}
				}
				counter++;
			}
			return jsonObjects;
		}
	};
}