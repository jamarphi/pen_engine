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
+ (void) PenMacIOSShaderInit: (const char*) shaderProgram {
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

    inst.iosPipelineState = [inst.iosDevice newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!inst.iosPipelineState)
    {
        __builtin_printf("%s", [[error localizedDescription] UTF8String]);
    }
}

+ (void) IOSUpdateInstanceUniform: (IOSInstanceData*) data{
	/*Updates the instanced offsets*/
    PenMacIOSState* inst = [PenMacIOSState Get];
	int size = sizeof(IOSInstanceData) * 400;
    if(!inst.iosInstanceBuffer){
    #if TARGET_OS_OSX
        id<MTLBuffer> instanceBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeManaged];
    #else
        id<MTLBuffer> instanceBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeShared];
    #endif
        inst.iosInstanceBuffer = instanceBuffer;
    }
    memcpy([inst.iosInstanceBuffer contents], data, size);
#if TARGET_OS_OSX
    [inst.iosInstanceBuffer didModifyRange: NSMakeRange(0, [inst.iosInstanceBuffer length])];
#endif
}
@end

void MapMacPenMacIOSShaderInit(const char* shaderProgram){
    /*Creates a Metal shader*/
    [PenMacIOSShader PenMacIOSShaderInit:shaderProgram];
}

void MapMacIOSSetInstanceState(unsigned int instanceState){
    /*Sets the instance state for rendering*/
    [PenMacIOSState Get].isInstanced = instanceState;
}

IOSInstanceData* MapMacIOSGetInstanceData(){
    /*Returns the instance buffer data if there is any*/
    IOSInstanceData* data = reinterpret_cast<IOSInstanceData*>([[PenMacIOSState Get].iosInstanceBuffer contents]);
    if(data){
        return data;
    }
    return nullptr;
}

void MapMacIOSUpdateInstanceUniform(IOSInstanceData* data){
    /*Updates the instanced offsets*/
    [PenMacIOSShader IOSUpdateInstanceUniform:data];
}
#endif
