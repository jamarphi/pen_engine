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
#include "ios_texture.h"

#ifdef __PEN_IOS__
@implementation IOSTexture
void IOS_CPPObjectCMapping::InitializeTexture(unsigned int texWidth, unsigned int texHeight, unsigned int type, unsigned int texSlot, unsigned char* textureData){
    /*Initializes a Metal ios texture*/
    [IOSTexture InitializeTexture:texWidth :texHeight :type :texSlot :textureData];
}

+ (void) InitializeTexture: (unsigned int) texWidth :(unsigned int) texHeight :(unsigned int) type :(unsigned int) texSlot :(unsigned char*) textureData{
	/*Initializes a Metal ios texture*/
    IOSState* inst = [IOSState Get];
	MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::alloc()->init();
	textureDesc->setWidth(texWidth);
	textureDesc->setHeight(texHeight);
	textureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
	textureDesc->setTextureType(MTL::TextureType2D);
	textureDesc->setStorageMode(MTL::StorageModeManaged);
	textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

    MTL::Texture* texture = inst.iosDevice->newTexture(textureDesc);
    if(type == 1) inst.iosPixelBuffer = texture;

	texture->replaceRegion(MTL::Region(0, 0, 0, texWidth, texHeight, 1), 0, textureData, texWidth * 4);
    if(texSlot < 8) {
        MTL::Texture** textures = inst.iosTextures;
        textures[texSlot] = texture;
    }

	textureDesc->release();
}

void IOS_CPPObjectCMapping::UpdatePixels(){
    /*Updates the ios pixel buffer*/
    [IOSTexture UpdatePixels];
}

+ (void) UpdatePixels{
	/*Updates the ios pixel buffer*/
    IOSState* inst = [IOSState Get];
	inst.iosPixelBuffer->replaceRegion(MTL::Region(0, 0, 0, 1280, 720, 1), 0, pen::State::Get()->pixelArray, 5120);
}
@end
#endif
