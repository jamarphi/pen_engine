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

#include "../../../src/state/config.h"

#ifdef __PEN_IOS__
#import "ios_state.h"
#import "ios_cpp_objective_c_mapping.h"
//#import <Metal/Metal.h>

@class IOSVertexBuffer;

@interface IOSVertexBuffer : NSObject

    + (void) IOSVertexBufferInit:(unsigned int) layerId
                     :(BatchVertexData*) data
                     :(unsigned int) size;
	+ (void) IOSVertexBufferDestroy: (unsigned int) layerId;
+ (NSMutableDictionary*) IOSVertexBuffersGet;
@end
#endif
