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
#include "mac_ios_shader.h"

#ifdef __PEN_MAC_IOS__

@implementation PenMacIOSShader
+ (void) PenMacIOSShaderInit: (const char*) shaderProgram :(unsigned int) type {
	/*Creates a Metal shader*/
    PenMacIOSState* inst = [PenMacIOSState Get];

	NSError* pError = nullptr;
    id<MTLLibrary> pLibrary = [inst.iosDevice newLibraryWithSource:[NSString stringWithUTF8String:shaderProgram] options:nil error: &pError];
	if (!pLibrary)
	{
		__builtin_printf("%s", [[pError localizedDescription] UTF8String]);
	}

    id<MTLFunction> pVertexFn = [pLibrary newFunctionWithName:@"vertexMain"];
    id<MTLFunction> pFragFn = [pLibrary newFunctionWithName:@"fragmentMain"];

    //id<MTLArgumentEncoder> pArgEncoder = [pVertexFn newArgumentEncoderWithBufferIndex:0];
	//inst.iosArgEncoder = pArgEncoder;

    MTLRenderPipelineDescriptor* pDesc = [[MTLRenderPipelineDescriptor alloc] init];
    [pDesc setVertexFunction:pVertexFn];
    [pDesc setFragmentFunction:pFragFn];
    [[[pDesc colorAttachments] objectAtIndexedSubscript:0] setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];

    switch(type){
        case 1:
            inst.iosPipelineState = [inst.iosDevice newRenderPipelineStateWithDescriptor:pDesc error:&pError];
            if (!inst.iosPipelineState)
            {
                __builtin_printf("%s", [[pError localizedDescription] UTF8String]);
            }
            inst.isInstanced = 0;
            break;
        case 2:
            inst.iosInstancedPipelineState = [inst.iosDevice newRenderPipelineStateWithDescriptor:pDesc error:&pError];
            if (!inst.iosInstancedPipelineState)
            {
                __builtin_printf("%s", [[pError localizedDescription] UTF8String]);
            }
            break;
        default:
            break;
    }
    
}

+ (void) IOSUpdateInstanceUniform: (IOSInstanceData*) data{
	/*Updates the instanced offsets*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    inst.isInstanced = 1;
	int size = sizeof(IOSInstanceData) * 400;
#ifndef TARGET_OS_IOS
    id<MTLBuffer> instanceBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeManaged];
#else
    id<MTLBuffer> instanceBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeShared];
#endif
    memcpy([instanceBuffer contents], data, size);
#ifndef TARGET_OS_IOS
    [instanceBuffer didModifyRange: NSMakeRange(0, [instanceBuffer length])];
#endif
	inst.iosInstanceBuffer = instanceBuffer;
}
@end

void MapMacPenMacIOSShaderInit(const char* shaderProgram, unsigned int type){
    /*Creates a Metal shader*/
    [PenMacIOSShader PenMacIOSShaderInit:shaderProgram :type];
}

void MapMacIOSUpdateInstanceUniform(IOSInstanceData* data){
    /*Updates the instanced offsets*/
    [PenMacIOSShader IOSUpdateInstanceUniform:data];
}
#endif
