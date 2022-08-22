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

	NSError* error = nullptr;
    id<MTLLibrary> library = [inst.iosDevice newLibraryWithSource:[NSString stringWithUTF8String:shaderProgram] options:nil error: &error];
	if (!library)
	{
		__builtin_printf("%s", [[error localizedDescription] UTF8String]);
	}

    id<MTLFunction> vertexFn = [library newFunctionWithName:@"vertexMain"];
    id<MTLFunction> fragFn = [library newFunctionWithName:@"fragmentMain"];

    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    [desc setVertexFunction:vertexFn];
    [desc setFragmentFunction:fragFn];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setBlendingEnabled:YES];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setRgbBlendOperation:MTLBlendOperationAdd];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setAlphaBlendOperation:MTLBlendOperationAdd];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setSourceRGBBlendFactor:MTLBlendFactorOne];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setSourceAlphaBlendFactor:MTLBlendFactorOne];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setDestinationRGBBlendFactor:MTLBlendFactorOneMinusSourceAlpha];
    [[[desc colorAttachments] objectAtIndexedSubscript:0] setDestinationAlphaBlendFactor:MTLBlendFactorOneMinusSourceAlpha];

    switch(type){
        case 1:
            inst.iosPipelineState = [inst.iosDevice newRenderPipelineStateWithDescriptor:desc error:&error];
            if (!inst.iosPipelineState)
            {
                __builtin_printf("%s", [[error localizedDescription] UTF8String]);
            }
            break;
        case 2:
            inst.iosInstancedPipelineState = [inst.iosDevice newRenderPipelineStateWithDescriptor:desc error:&error];
            if (!inst.iosInstancedPipelineState)
            {
                __builtin_printf("%s", [[error localizedDescription] UTF8String]);
            }
            break;
        default:
            break;
    }
    
}

+ (void) IOSUpdateInstanceUniform: (IOSInstanceData*) data{
	/*Updates the instanced offsets*/
    PenMacIOSState* inst = [PenMacIOSState Get];
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
