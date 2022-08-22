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
#include "mac_ios_texture.h"

#ifdef __PEN_MAC_IOS__
@implementation PenMacIOSTexture
+ (void) IOSInitializeTexture: (unsigned int) texWidth :(unsigned int) texHeight :(unsigned int) type :(unsigned int) texSlot :(unsigned char*) textureData{
	/*Initializes a Metal ios texture*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    
    MTLTextureDescriptor* textureDesc = [[MTLTextureDescriptor alloc] init];
    [textureDesc setWidth:texWidth];
    [textureDesc setHeight:texHeight];
    [textureDesc setPixelFormat:MTLPixelFormatRGBA8Unorm];
    [textureDesc setTextureType:MTLTextureType2D];
    [textureDesc setStorageMode:MTLStorageModeShared];
    [textureDesc setUsage:MTLResourceUsageSample | MTLResourceUsageRead];

    id<MTLTexture> texture = [inst.iosDevice newTextureWithDescriptor:textureDesc];
    [texture replaceRegion:MTLRegionMake2D(0, 0, texWidth, texHeight) mipmapLevel:0 withBytes:textureData bytesPerRow:texWidth * 4];
    if(type == 1) inst.iosPixelBuffer = texture;
    if(texSlot < 8) {
        id<MTLTexture> __strong* textures = [PenMacIOSState GetTextures];
        textures[texSlot] = texture;
    }
}

+ (void) IOSLoadTexture:(NSString*)path :(NSString*)mimeType :(unsigned int) texSlot{
    /*Loads in a texture*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    MTKTextureLoader *loader = [[MTKTextureLoader alloc] initWithDevice: inst.iosDevice];
    NSError* error;
    NSURL* textureFile = [[NSBundle mainBundle] URLForResource:path withExtension:mimeType];
    
    id<MTLTexture> texture = [loader newTextureWithContentsOfURL:textureFile options:nil error:&error];
        
    if(!texture)
    {
        __builtin_printf("%s", [[error localizedDescription] UTF8String]);
    }
    
    if(texSlot < 8) {
        id<MTLTexture> __strong* textures = [PenMacIOSState GetTextures];
        textures[texSlot] = texture;
    }
}

+ (void) IOSUpdatePixels{
	/*Updates the ios pixel buffer*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    [inst.iosPixelBuffer replaceRegion:MTLRegionMake2D(0, 0, 1280, 720) mipmapLevel:0 withBytes:pen::State::Get()->pixelArray bytesPerRow:5120];
}
@end

void MapMacIOSInitializeTexture(unsigned int texWidth, unsigned int texHeight, unsigned int type, unsigned int texSlot, unsigned char* textureData){
    /*Initializes a Metal ios texture*/
    [PenMacIOSTexture IOSInitializeTexture:texWidth :texHeight :type :texSlot :textureData];
}

void MapMacIOSLoadTexture(const char* path, const char* mimeType, unsigned int texSlot){
    /*Loads a Metal ios texture*/
    [PenMacIOSTexture IOSLoadTexture:[NSString stringWithUTF8String:path] :[NSString stringWithUTF8String:mimeType] :texSlot];
}

void MapMacIOSUpdatePixels(){
    /*Updates the ios pixel buffer*/
    [PenMacIOSTexture IOSUpdatePixels];
}
#endif
