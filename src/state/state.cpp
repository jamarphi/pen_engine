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
#include "state.h"

pen::State* pen::State::instance = nullptr;

namespace pen {
	void State::SetDraggable(void* item) {
		pen::State::Get()->draggableItem = item;
	}

	void State::SetKeyable(void* item) {
		pen::State::Get()->keyableItem = item;
	}

	void State::LoadCharacters() {
		/*Loads the character meta data for positioning*/
		pen::State* inst = pen::State::Get();
		Character c;

		/*SPACE*/
		c = { 32,
		pen::Vec2(0,0),
		pen::Vec2(0,0),
		512
		};
		inst->characters.Insert(' ', c);

		/*!*/
		c = { 33,
		pen::Vec2(10,23),
		pen::Vec2(2,23),
		576
		};
		inst->characters.Insert('!', c); 

		/*"*/
		c = { 34,
		pen::Vec2(9,8),
		pen::Vec2(2,23),
		832
		};
		inst->characters.Insert('"', c);

		/*#*/
		c = {35,
			pen::Vec2(20, 23),
			pen::Vec2(0, 23),
			1344
	};
	inst->characters.Insert('#', c);

	/*$*/
	c = { 36,
	pen::Vec2(15,26),
	pen::Vec2(2,24),
	1152
	};
	inst->characters.Insert('$', c);

	/*%*/
	c = { 37,
	pen::Vec2(24,23),
	pen::Vec2(1,23),
	1664
	};
	inst->characters.Insert('%', c);

	/*&*/
	c = { 38,
	pen::Vec2(23,23),
	pen::Vec2(1,23),
	1472
	};
	inst->characters.Insert('&', c);

	/*'*/
	c = { 39,
	pen::Vec2(3,8),
	pen::Vec2(2,23),
	448
	};
	inst->characters.Insert('\'',c);

		/*(*/
		c = { 40,
		pen::Vec2(8,28),
		pen::Vec2(1,23),
		576
		};
	inst->characters.Insert('(', c);

	/*)*/
	c = { 41,
	pen::Vec2(9,28),
	pen::Vec2(0,23),
	576
	};
	inst->characters.Insert(')', c);

	/***/
	c = { 42,
	pen::Vec2(16,14),
	pen::Vec2(1,24),
	1152
	};
	inst->characters.Insert('*', c);

	/*+*/
	c = { 43,
	pen::Vec2(16,16),
	pen::Vec2(1,19),
	1152
	};
	inst->characters.Insert('+', c);

	/*,*/
	c = { 44,
	pen::Vec2(6,8),
	pen::Vec2(0,4),
	512
	};
	inst->characters.Insert(',', c);

	/*-*/
	c = { 45,
	pen::Vec2(8,3),
	pen::Vec2(1,10),
	640
	};
	inst->characters.Insert('-', c);

	/*.*/
	c = { 46,
	pen::Vec2(25,25),
	pen::Vec2(2,25),
	576
	};
	inst->characters.Insert('.', c);

	/*/*/
	c = { 47,
	pen::Vec2(12,23),
	pen::Vec2(0,23),
	768
	};
	inst->characters.Insert('/', c);

	/*0*/
	c = { 48,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('0', c);

	/*1*/
	c = { 49,
	pen::Vec2(10,23),
	pen::Vec2(2,23),
	1152
	};
	inst->characters.Insert('1', c);

	/*2*/
	c = { 50,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('2', c);

	/*3*/
	c = { 51,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('3', c);

	/*4*/
	c = { 52,
	pen::Vec2(18,23),
	pen::Vec2(0,23),
	1152
	};
	inst->characters.Insert('4', c);

	/*5*/
	c = { 53,
	pen::Vec2(15,23),
	pen::Vec2(2,23),
	1152
	};
	inst->characters.Insert('5', c);

	/*6*/
	c = { 54,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('6', c);

	/*7*/
	c = { 55,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('7', c);

	/*8*/
	c = { 56,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('8', c);

	/*9*/
	c = { 57,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('9', c);

	/*:*/
	c = { 58,
	pen::Vec2(5,17),
	pen::Vec2(2,17),
	576
	};
	inst->characters.Insert(':', c);

	/*;*/
	c = { 59,
	pen::Vec2(7,21),
	pen::Vec2(0,17),
	576
	};
	inst->characters.Insert(';', c);

	/*<*/
	c = { 60,
	pen::Vec2(16,15),
	pen::Vec2(1,19),
	1152
	};
	inst->characters.Insert('<', c);

	/*=*/
	c = { 61,
	pen::Vec2(16,9),
	pen::Vec2(1,16),
	1152
	};
	inst->characters.Insert('=', c);

	/*>*/
	c = { 62,
	pen::Vec2(16,15),
	pen::Vec2(1,19),
	1152
	};
	inst->characters.Insert('>', c);

	/*?*/
	c = { 63,
	pen::Vec2(13,23),
	pen::Vec2(0,23),
	896
	};
	inst->characters.Insert('?', c);

	/*@*/
	c = { 64,
	pen::Vec2(26,26),
	pen::Vec2(1,23),
	1856
	};
	inst->characters.Insert('@', c);

	/*A*/
	c = { 65,
	pen::Vec2(21,23),
	pen::Vec2(0,23),
	1280
	};
	inst->characters.Insert('A', c);

	/*B*/
	c = { 66,
	pen::Vec2(16,23),
	pen::Vec2(3,23),
	1344
	};
	inst->characters.Insert('B', c);

	/*C*/
	c = { 67,
	pen::Vec2(19,23),
	pen::Vec2(1,23),
	1280
	};
	inst->characters.Insert('C', c);

	/*D*/
	c = { 68,
	pen::Vec2(19,23),
	pen::Vec2(3,23),
	1472
	};
	inst->characters.Insert('D', c);

	/*E*/
	c = { 69,
	pen::Vec2(13,23),
	pen::Vec2(3,23),
	1152
	};
	inst->characters.Insert('E', c);

	/*F*/
	c = { 70,
	pen::Vec2(13,23),
	pen::Vec2(3,23),
	1088
	};
	inst->characters.Insert('F', c);

	/*G*/
	c = { 71,
	pen::Vec2(20,23),
	pen::Vec2(1,23),
	1472
	};
	inst->characters.Insert('G', c);

	/*H*/
	c = { 72,
	pen::Vec2(18,23),
	pen::Vec2(3,23),
	1536
	};
	inst->characters.Insert('H', c);

	/*I*/
	c = { 73,
	pen::Vec2(3,23),
	pen::Vec2(3,23),
	576
	};
	inst->characters.Insert('I', c);

	/*J*/
	c = { 74,
	pen::Vec2(9,29),
	pen::Vec2(-3,23),
	576
	};
	inst->characters.Insert('J', c);

	/*K*/
	c = { 75,
	pen::Vec2(17,23),
	pen::Vec2(3,23),
	1280
	};
	inst->characters.Insert('K', c);

	/*L*/
	c = { 76,
	pen::Vec2(13,23),
	pen::Vec2(3,23),
	1088
	};
	inst->characters.Insert('L', c);

	/*M*/
	c = { 77,
	pen::Vec2(23,23),
	pen::Vec2(3,23),
	1856
	};
	inst->characters.Insert('M', c);

	/*N*/
	c = { 78,
	pen::Vec2(18,23),
	pen::Vec2(3,23),
	1536
	};
	inst->characters.Insert('N', c);

	/*O*/
	c = { 79,
	pen::Vec2(22,23),
	pen::Vec2(1,23),
	1600
	};
	inst->characters.Insert('O', c);

	/*P*/
	c = { 80,
	pen::Vec2(15,23),
	pen::Vec2(3,23),
	1216
	};
	inst->characters.Insert('P', c);

	/*Q*/
	c = { 81,
	pen::Vec2(22,28),
	pen::Vec2(1,23),
	1600
	};
	inst->characters.Insert('Q', c);

	/*R*/
	c = { 82,
	pen::Vec2(17,23),
	pen::Vec2(3,23),
	1280
	};
	inst->characters.Insert('R', c);

	/*S*/
	c = { 83,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('S', c);

	/*T*/
	c = { 84,
	pen::Vec2(18,23),
	pen::Vec2(0,23),
	1152
	};
	inst->characters.Insert('T', c);

	/*U*/
	c = { 85,
	pen::Vec2(19,23),
	pen::Vec2(2,23),
	1472
	};
	inst->characters.Insert('U', c);

	/*V*/
	c = { 86,
	pen::Vec2(20,23),
	pen::Vec2(0,23),
	1216
	};
	inst->characters.Insert('V', c);

	/*W*/
	c = { 87,
	pen::Vec2(30,23),
	pen::Vec2(0,23),
	1920
	};
	inst->characters.Insert('W', c);

	/*X*/
	c = { 88,
	pen::Vec2(19,23),
	pen::Vec2(0,23),
	1152
	};
	inst->characters.Insert('X', c);

	/*Y*/
	c = { 89,
	pen::Vec2(18,23),
	pen::Vec2(0,23),
	1152
	};
	inst->characters.Insert('Y', c);

	/*Z*/
	c = { 90,
	pen::Vec2(16,23),
	pen::Vec2(1,23),
	1152
	};
	inst->characters.Insert('Z', c);

	/*[*/
	c = { 91,
	pen::Vec2(8,28),
	pen::Vec2(2,23),
	704
	};
	inst->characters.Insert('[', c);

	/*\*/
	c = { 92,
	pen::Vec2(12,23),
	pen::Vec2(0,23),
	768
	};
	inst->characters.Insert('\\',c);

		/*]*/
		c = { 93,
		pen::Vec2(8,28),
		pen::Vec2(0,23),
		704
		};
	inst->characters.Insert(']', c);

	/*^*/
	c = { 94,
	pen::Vec2(17,14),
	pen::Vec2(0,23),
	1088
	};
	inst->characters.Insert('^', c);

	/*_*/
	c = { 95,
	pen::Vec2(16,3),
	pen::Vec2(-1,-3),
	896
	};
	inst->characters.Insert('_', c);

	/*`*/
	c = { 96,
	pen::Vec2(7,5),
	pen::Vec2(6,24),
	1152
	};
	inst->characters.Insert('`', c);

	/*a*/
	c = { 97,
	pen::Vec2(15,17),
	pen::Vec2(1,17),
	1152
	};
	inst->characters.Insert('a', c);

	/*b*/
	c = { 98,
	pen::Vec2(14,22),
	pen::Vec2(2,22),
	1280
	};
	inst->characters.Insert('b', c);

	/*c*/
	c = { 99,
	pen::Vec2(14,17),
	pen::Vec2(4,17),
	960
	};
	inst->characters.Insert('c', c);

	/*d*/
	c = { 100,
	pen::Vec2(14,22),
	pen::Vec2(1,22),
	1280
	};
	inst->characters.Insert('d', c);

	/*e*/
	c = { 101,
	pen::Vec2(16,17),
	pen::Vec2(1,17),
	1152
	};
	inst->characters.Insert('e', c);

	/*f*/
	c = { 102,
	pen::Vec2(13,24),
	pen::Vec2(0,24),
	704
	};
	inst->characters.Insert('f', c);

	/*g*/
	c = { 103,
	pen::Vec2(17,25),
	pen::Vec2(0,17),
	1152
	};
	inst->characters.Insert('g', c);

	/*h*/
	c = { 104,
	pen::Vec2(16,24),
	pen::Vec2(2,24),
	1280
	};
	inst->characters.Insert('h', c);

	/*i*/
	c = { 105,
	pen::Vec2(8,24),
	pen::Vec2(2,24),
	512
	};
	inst->characters.Insert('i', c);

	/*j*/
	c = { 106,
	pen::Vec2(8,32),
	pen::Vec2(-2,24),
	512
	};
	inst->characters.Insert('j', c);

	/*k*/
	c = { 107,
	pen::Vec2(15,24),
	pen::Vec2(2,24),
	1088
	};
	inst->characters.Insert('k', c);

	/*l*/
	c = { 108,
	pen::Vec2(8,24),
	pen::Vec2(2,24),
	512
	};
	inst->characters.Insert('l', c);

	/*m*/
	c = { 109,
	pen::Vec2(26,17),
	pen::Vec2(2,17),
	1920
	};
	inst->characters.Insert('m', c);

	/*n*/
	c = { 110,
	pen::Vec2(16,17),
	pen::Vec2(2,17),
	1280
	};
	inst->characters.Insert('n', c);

	/*o*/
	c = { 111,
	pen::Vec2(17,17),
	pen::Vec2(1,17),
	1216
	};
	inst->characters.Insert('o', c);

	/*p*/
	c = { 112,
	pen::Vec2(16,25),
	pen::Vec2(2,17),
	1280
	};
	inst->characters.Insert('p', c);

	/*q*/
	c = { 113,
	pen::Vec2(16,25),
	pen::Vec2(1,17),
	1280
	};
	inst->characters.Insert('q', c);

	/*r*/
	c = { 114,
	pen::Vec2(11,17),
	pen::Vec2(2,17),
	832
	};
	inst->characters.Insert('r', c);

	/*s*/
	c = { 115,
	pen::Vec2(13,17),
	pen::Vec2(1,17),
	960
	};
	inst->characters.Insert('s', c);

	/*t*/
	c = { 116,
	pen::Vec2(11,21),
	pen::Vec2(0,21),
	704
	};
	inst->characters.Insert('t', c);

	/*u*/
	c = { 117,
	pen::Vec2(15,17),
	pen::Vec2(2,17),
	1280
	};
	inst->characters.Insert('u', c);

	/*v*/
	c = { 118,
	pen::Vec2(17,17),
	pen::Vec2(0,17),
	1024
	};
	inst->characters.Insert('v', c);

	/*w*/
	c = { 119,
	pen::Vec2(25,17),
	pen::Vec2(0,17),
	1600
	};
	inst->characters.Insert('w', c);

	/*x*/
	c = { 120,
	pen::Vec2(17,17),
	pen::Vec2(0,17),
	1088
	};
	inst->characters.Insert('x', c);

	/*y*/
	c = { 121,
	pen::Vec2(17,25),
	pen::Vec2(0,17),
	1024
	};
	inst->characters.Insert('y', c);

	/*z*/
	c = { 122,
	pen::Vec2(13,17),
	pen::Vec2(1,17),
	960
	};
	inst->characters.Insert('z', c);

	/*{*/
	c = { 123,
	pen::Vec2(12,28),
	pen::Vec2(0,23),
	768
	};
	inst->characters.Insert('{', c);

	/*|*/
	c = { 124,
	pen::Vec2(3,32),
	pen::Vec2(7,24),
	1152
	};
	inst->characters.Insert('|', c);

	/*}*/
	c = { 125,
	pen::Vec2(11,28),
	pen::Vec2(1,23),
	768
	};
	inst->characters.Insert('}', c);

	/*~*/
	c = { 126,
	pen::Vec2(16,4),
	pen::Vec2(1,13),
	1152
	};
	inst->characters.Insert('~', c);

	/**/
	c = { 127,
	pen::Vec2(14,23),
	pen::Vec2(3,23),
	1216
	};
	inst->characters.Insert('', c);


	}
}
