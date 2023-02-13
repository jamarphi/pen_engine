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
#include "mac_ios_view_delegate.h"

#ifdef __PEN_MAC_IOS__
static PenMacIOSMTKViewDelegate* instance;
static IOSUniformData* uniformArray;
static unsigned int previousLayerCount;
static unsigned int layerCount;
static long indexCount;

@implementation PenMacIOSMTKViewDelegate

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view size:(CGSize)size
{
    self = [super init];
    if(self)
    {
        PenMacIOSState* inst = [PenMacIOSState Get];
#if !TARGET_OS_OSX
        self.motionManager = [[CMMotionManager alloc] init];
        if(self.motionManager.isAccelerometerAvailable){
            self.motionManager.accelerometerUpdateInterval = 1.0 / 60.0;
            void (^accelerometerCallback)(CMAccelerometerData*, NSError*) = ^(CMAccelerometerData* accelerometerData, NSError* error){
                double acelX = (double)accelerometerData.acceleration.x;
                double acelY = (double)accelerometerData.acceleration.y;
                double acelZ = (double)accelerometerData.acceleration.z;
                
                if (pen::State::Get()->mobileOnTiltCallback != nullptr){ (*pen::State::Get()->mobileOnTiltCallback)(acelX, acelY, acelZ);
                }
            };
            
            [self.motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:accelerometerCallback];
        }
#endif
        
        inst.iosDevice = view.device;

        /*Currently at max three different buffer types sent to metal shaders*/
        inst.dispatchSemaphore = dispatch_semaphore_create(3);
        
        App* app = new App();
        pen::State::Get()->mobileActive = true;
        app->CreateApplication("App", size.width, size.height, "");
        
        inst.iosCommandQueue = [inst.iosDevice newCommandQueue];
        inst.iosMtkView = view;

    #if TARGET_OS_OSX
        NSApplication* macApp = reinterpret_cast<NSApplication*>([inst.iosLaunchNotification object]);
        [macApp activateIgnoringOtherApps:true];
    #endif
        
        app->OnCreate();
    }
    instance = self;
    return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    /*Render loop*/
    (*pen::State::Get()->mobileOnRenderCallback)();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    /*Update the size of the screen*/
    pen::State* inst = pen::State::Get();
    if (size.width < inst->screenWidth || size.height < inst->screenHeight) {
        size.width = inst->screenWidth;
        size.height = inst->screenHeight;
    }

    inst->actualScreenHeight = size.height;
    inst->actualScreenWidth = size.width;
}

#if TARGET_OS_OSX
- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseMoved:(NSEvent *)event {
    /*The mouse has moved*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    coreStateInst->keyableItem = nullptr;
    NSPoint location = event.locationInWindow;
    NSPoint localPoint = [inst.iosMtkView convertPoint:location fromView:inst.iosMtkView];
    
    double xPos = (double)location.x;
    double yPos = (double)location.y;
    /*Flip y position to start from the bottom*/
    yPos = coreStateInst->actualScreenHeight - yPos;

    /*Scale based on screen width and height and convert to pixel buffer coordinates*/
    xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
    yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
    xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
    yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;

    if(coreStateInst->mobileMouse->size() > 0) {
        coreStateInst->mobileMouse->at(0)->x = (int)xPos;
        coreStateInst->mobileMouse->at(0)->y = (int)yPos;
    }else{
        coreStateInst->mobileMouse->push_back(new pen::Tap{0, (int)xPos, (int)yPos});
    }
}

- (void)mouseDown:(NSEvent *)event {
    /*A click has started*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    coreStateInst->keyableItem = nullptr;
    NSPoint location = event.locationInWindow;
    NSPoint localPoint = [inst.iosMtkView convertPoint:location fromView:inst.iosMtkView];
    
    double xPos = (double)location.x;
    double yPos = (double)location.y;
    /*Flip y position to start from the bottom*/
    yPos = coreStateInst->actualScreenHeight - yPos;

    /*Scale based on screen width and height and convert to pixel buffer coordinates*/
    xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
    yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
    xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
    yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;

    if(coreStateInst->mobileMouse->size() > 0) {
        coreStateInst->mobileMouse->at(0)->x = (int)xPos;
        coreStateInst->mobileMouse->at(0)->y = (int)yPos;
    }else{
        coreStateInst->mobileMouse->push_back(new pen::Tap{0, (int)xPos, (int)yPos});
    }
    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::PRESSED, 0);
}

- (void)mouseDragged:(NSEvent *)event {
    /*A click is being dragged*/
    NSPoint location = event.locationInWindow;
    double xPos = (double)location.x;
    double yPos = (double)location.y;
    pen::State* inst = pen::State::Get();
    if ((inst->handleGUIDragEvents && inst->draggableItem != nullptr)
        || inst->handleCameraInput) {
        /*Flip y position to start from the bottom*/
        yPos = inst->actualScreenHeight - yPos;

        /*Scale based on screen width and height and convert to pixel buffer coordinates*/
        xPos = xPos * inst->screenWidth / inst->actualScreenWidth;
        yPos = yPos * inst->screenHeight / inst->actualScreenHeight;
        xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
        yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;

        if(coreStateInst->mobileMouse->size() > 0) {
            coreStateInst->mobileMouse->at(0)->x = (int)xPos;
            coreStateInst->mobileMouse->at(0)->y = (int)yPos;
        }else{
            coreStateInst->mobileMouse->push_back(new pen::Tap{0, (int)xPos, (int)yPos});
        }

        bool cameraHandled = pen::Render::Get()->camera.HandleInput(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::HELD, true);
        cameraHandled = pen::GetPixelCamera()->HandleInput(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::HELD, false);
        if (!cameraHandled) {
            pen::ui::Item* item = (pen::ui::Item*)pen::State::Get()->draggableItem;
            item->OnDrag(item, &xPos, &yPos);
        }
    }
}

- (void)mouseUp:(NSEvent *)event {
    /*A click has ended*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    coreStateInst->draggableItem = nullptr;
    NSPoint location = event.locationInWindow;
    NSPoint localPoint = [inst.iosMtkView convertPoint:location fromView:inst.iosMtkView];
    double xPos = (double)location.x;
    double yPos = (double)location.y;
    /*Flip y position to start from the bottom*/
    yPos = coreStateInst->actualScreenHeight - yPos;

    /*Scale based on screen width and height and convert to pixel buffer coordinates*/
    xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
    yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
    xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
    yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;
    if(coreStateInst->mobileMouse->size() > 0) {
        coreStateInst->mobileMouse->at(0)->x = (int)xPos;
        coreStateInst->mobileMouse->at(0)->y = (int)yPos;
    }else{
        coreStateInst->mobileMouse->push_back(new pen::Tap{0, (int)xPos, (int)yPos});
    }
    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::RELEASED, 0);
}

- (void)keyDown:(NSEvent *)event{
    /*A key has been pressed*/
    NSString* characters = [event characters];
    const char* keys = [characters UTF8String];
    pen::State* inst = pen::State::Get();
    if ((inst->handleGUIKeyEvents && inst->keyableItem != nullptr) || inst->handleCameraInput) {
        bool cameraHandled = pen::Render::Get()->camera.HandleInput((int)keys[0], pen::in::KEYS::PRESSED, true);
        cameraHandled = pen::GetPixelCamera()->HandleInput((int)keys[0], pen::in::KEYS::PRESSED, false);
        if (!cameraHandled) {
            pen::ui::Item* item = (pen::ui::Item*)pen::State::Get()->keyableItem;
            item->OnKey(item, (int)keys[0], pen::in::KEYS::PRESSED);
        }
    }
}

- (void)keyUp:(NSEvent *)event{
    /*A key has been released*/
    NSString* characters = [event characters];
    const char* keys = [characters UTF8String];
    pen::State* inst = pen::State::Get();
    if ((inst->handleGUIKeyEvents && inst->keyableItem != nullptr) || inst->handleCameraInput) {
        bool cameraHandled = pen::Render::Get()->camera.HandleInput((int)keys[0], pen::in::KEYS::RELEASED, true);
        cameraHandled = pen::GetPixelCamera()->HandleInput((int)keys[0], pen::in::KEYS::PRESSED, false);
        if (!cameraHandled) {
            pen::ui::Item* item = (pen::ui::Item*)pen::State::Get()->keyableItem;
            item->OnKey(item, (int)keys[0], pen::in::KEYS::RELEASED);
        }
    }
}

- (NSSize)windowWillResize:(NSWindow *)sender
                    toSize:(NSSize)frameSize {
    /*Update the size of the window*/
    int width = (int) frameSize.width;
    int height = (int) frameSize.height;
    pen::State* inst = pen::State::Get();
    if (width < inst->screenWidth || height < inst->screenHeight) {
        width = inst->screenWidth;
        height = inst->screenHeight;
    }

    inst->actualScreenHeight = height;
    inst->actualScreenWidth = width;
    return frameSize;
}

+(void)MakeMouseHidden {
    /*Hides the mouse*/
    [NSCursor hide];
}

+(void)MakeMouseShow {
    /*Shows the mouse*/
    [NSCursor show];
};

+(BOOL)WindowActive {
    /*Returns whether the application window is active*/
    return [[IOSState Get].iosMtkView window].onActiveSpace;
}

#else
- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    /*A touch has started*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    NSArray<UITouch*>* touchArray = [touches allObjects];
    for (int i = 0; i < touches.count; i++){
        UITouch* touch = [touchArray objectAtIndex:i];
        CGPoint location = [touch locationInView:inst.iosMtkView];
        double xPos = (double)location.x;
        double yPos = (double)location.y;
        /*Flip y position to start from the bottom*/
        yPos = coreStateInst->actualScreenHeight - yPos;

        /*Scale based on screen width and height and convert to pixel buffer coordinates*/
        xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
        yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
        xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
        yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;
        coreStateInst->mobileMouse->push_back(new pen::Tap{(int) touch, (int)xPos, (int)yPos});
    }
    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::PRESSED, 0);
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    /*A touch is moving*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    NSArray<UITouch*>* touchArray = [touches allObjects];
    for (int i = 0; i < touches.count; i++){
        UITouch* touch = [touchArray] objectAtIndex:i];
        CGPoint location = [touch locationInView:inst.iosMtkView];
        double xPos = (double)location.x;
        double yPos = (double)location.y;
        /*Flip y position to start from the bottom*/
        yPos = coreStateInst->actualScreenHeight - yPos;

        /*Scale based on screen width and height and convert to pixel buffer coordinates*/
        xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
        yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
        xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
        yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;
        for (int j = 0; j < inst->mobileMouse->size(); j++) {
            if (inst->mobileMouse->at(j)->id == (int)touch) {
                inst->mobileMouse->at(j)->x = (int)xPos;
                inst->mobileMouse->at(j)->y = (int)yPos;
                break;
            }
        }
    }
}

- (void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    /*A touch has ended*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    pen::State* coreStateInst = pen::State::Get();
    NSArray<UITouch*>* touchArray = [touches allObjects];
    std::vector<pen::Tap*>* tempTaps = new std::vector<pen::Tap*>();
    std::vector<int> ids;
    for (int i = 0; i < touches.count; i++){
        UITouch* touch = [touchArray objectAtIndex:i];
        CGPoint location = [touch locationInView:inst.iosMtkView];
        double xPos = (double)location.x;
        double yPos = (double)location.y;
        /*Flip y position to start from the bottom*/
        yPos = coreStateInst->actualScreenHeight - yPos;

        /*Scale based on screen width and height and convert to pixel buffer coordinates*/
        xPos = xPos * coreStateInst->screenWidth / coreStateInst->actualScreenWidth;
        yPos = yPos * coreStateInst->screenHeight / coreStateInst->actualScreenHeight;
        xPos = xPos * pen::PixelBufferWidth() / coreStateInst->actualScreenWidth;
        yPos = yPos * pen::PixelBufferHeight() / coreStateInst->actualScreenHeight;
        for (int j = 0; j < inst->mobileMouse->size(); j++) {
            if (inst->mobileMouse->at(j)->id == (int)touch) {
                /*Updates the mobileMouse vector with the released point for handling in mobile_click_callback before removing it*/
                inst->mobileMouse->at(j)->x = (int)xPos;
                inst->mobileMouse->at(j)->y = (int)yPos;
                id.push_back((int)touch);
                break;
            }
        }
    }
    

    pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::RELEASED, 0);
    for(int k = 0; k < inst->mobileMouse->size(); k++){
        for(int l = 0; l < ids.size(); l++){
            if((int)inst->mobileMouse->at(k)->id == ids[l]){
                break;
            }else if(l == ids.size() - 1 && (int)inst->mobileMouse->at(k)->id != ids[l]){
                tempTaps->push_back(inst->mobileMouse->at(k));
            }
        }
    }
    delete inst->mobileMouse;
    inst->mobileMouse = tempTaps;
}
#endif

+ (PenMacIOSMTKViewDelegate*) Get{
    /*Returns an instance of PenMacIOSMTKViewDelegate*/
    return instance;
}

+ (void) AddUniform:(unsigned int) layerId :(pen::Mat4x4) mvp{
    /*Adds a uniform to the uniform dictionary*/
    if(!uniformArray){
        layerCount = layerId + 1;
        indexCount = layerCount * 6 * MAX_OBJECTS_SINGULAR;
        uniformArray = new IOSUniformData[layerCount];
    }
    
    if(layerId > layerCount - 1){
        IOSUniformData* tempArray = new IOSUniformData[layerId + 1];
        for(int i = 0; i < layerCount; i++){
            tempArray[i] = uniformArray[i];
        }
        delete uniformArray;
        layerCount = layerId + 1;
        indexCount = layerCount * 6 * MAX_OBJECTS_SINGULAR;
        uniformArray = tempArray;
    }
    
    simd::float4 colA = {mvp.matrix[0][0], mvp.matrix[1][0], mvp.matrix[2][0], mvp.matrix[3][0]};
    simd::float4 colB = {mvp.matrix[0][1], mvp.matrix[1][1], mvp.matrix[2][1], mvp.matrix[3][1]};
    simd::float4 colC = {mvp.matrix[0][2], mvp.matrix[1][2], mvp.matrix[2][2], mvp.matrix[3][2]};
    simd::float4 colD = {mvp.matrix[0][3], mvp.matrix[1][3], mvp.matrix[2][3], mvp.matrix[3][3]};
    simd::float4x4 mat = simd::float4x4(colA, colB, colC, colD);
    
    IOSUniformData uniform = IOSUniformData{mat};
    uniformArray[layerId] = uniform;
}

+ (void) UpdateUniforms{
	/*Updates the uniform data*/
    PenMacIOSState* inst = [PenMacIOSState Get];
	
	int size = sizeof(IOSUniformData) * layerCount;

    if(!inst.iosUniformBuffer || previousLayerCount != layerCount){
        
#if TARGET_OS_OSX
        inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeManaged];
#else
        inst.iosUniformBuffer = [inst.iosDevice newBufferWithLength:size options:MTLResourceStorageModeShared];
#endif
        previousLayerCount = layerCount;
    }
    
    memcpy([inst.iosUniformBuffer contents], uniformArray, size);
#if TARGET_OS_OSX
    [inst.iosUniformBuffer didModifyRange: NSMakeRange(0, [inst.iosUniformBuffer length])];
#endif
}

+ (void) SubmitBatch:(id<MTLBuffer>) iosVertexBuffer
    :(BatchVertexData*) data
    :(int) size {
    /*Submits the vertex data to the GPU*/
    memcpy([iosVertexBuffer contents], data, size);
#if TARGET_OS_OSX
    [iosVertexBuffer didModifyRange: NSMakeRange(0, [iosVertexBuffer length])];
#endif
}

+ (void) Render:(unsigned int) shapeType{
    /*Renders the ios mtk view*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    id<MTLBuffer> iosVertexBuffer = [PenMacIOSVertexBuffer Get].iosVertexBuffer;
    id<MTLBuffer> iosIndexBuffer = [PenMacIOSIndexBuffer Get].iosIndexBuffer;
    dispatch_semaphore_wait(inst.dispatchSemaphore, DISPATCH_TIME_FOREVER);
    id<MTLCommandBuffer> command = [inst.iosCommandQueue commandBuffer];
    MTLRenderPassDescriptor* renderPassDescriptor = [inst.iosMtkView currentRenderPassDescriptor];
    if(renderPassDescriptor){
        inst.iosCommandEncoder = [command renderCommandEncoderWithDescriptor:renderPassDescriptor];
        inst.iosCommandBuffer = command;
        
        [command addCompletedHandler:^(id<MTLCommandBuffer> dispatchCallback) {
            dispatch_semaphore_signal( inst.dispatchSemaphore );
        }];

        [inst.iosCommandEncoder setRenderPipelineState:inst.iosPipelineState];
        [inst.iosCommandEncoder setVertexBuffer:iosVertexBuffer offset:0 atIndex:0];
        [inst.iosCommandEncoder setVertexBuffer:inst.iosUniformBuffer offset:0 atIndex:1];
        [inst.iosCommandEncoder setVertexBuffer:inst.iosInstanceBuffer offset:0 atIndex:2];
        [inst.iosCommandEncoder setFragmentTextures:[PenMacIOSState GetTextures] withRange:NSMakeRange(0,8)];
        [inst.iosCommandEncoder setCullMode:MTLCullModeBack];
        [inst.iosCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
        
        MTLPrimitiveType type;

        switch (shapeType) {
        case 0:
            type = MTLPrimitiveTypePoint;
            break;
        case 1:
            type = MTLPrimitiveTypeLine;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            type = MTLPrimitiveTypeTriangle;
            break;
        default:
            type = MTLPrimitiveTypeTriangle;
            break;
        }

        [inst.iosCommandEncoder drawIndexedPrimitives:type indexCount:indexCount indexType:MTLIndexTypeUInt32 indexBuffer:iosIndexBuffer indexBufferOffset:0 instanceCount:inst.isInstanced > 0 ? 400 : 1];
        [inst.iosCommandEncoder endEncoding];
        
        [inst.iosCommandBuffer presentDrawable:[inst.iosMtkView currentDrawable]];
        [inst.iosCommandBuffer commit];
    }
}

+ (void) Background: (float) r
:(float) g
:(float) b
:(float) a{
    /*Updates the background of the mtk window*/
    PenMacIOSState* inst = [PenMacIOSState Get];
    [inst.iosMtkView setClearColor:MTLClearColorMake(r, g, b, a)];
}
@end

void MapMacIOSMakeMouseHidden(){
    /*Hides the mouse*/
#if TARGET_OS_OSX
    [PenMacIOSMTKViewDelegate MakeMouseHidden];
#endif
}

void MapMacIOSMakeMouseShow(){
    /*Shows the mouse*/
#if TARGET_OS_OSX
    [PenMacIOSMTKViewDelegate MakeMouseShow];
#endif
}

bool MapMacIOSWindowActive(){
    /*Returns whether the application window is active*/
#if TARGET_OS_OSX
    return (bool)[PenMacIOSMTKViewDelegate WindowActive];
#endif
}

void MapMacIOSAddUniform(unsigned int layerId, pen::Mat4x4 mvp){
    /*Adds a uniform to the uniform dictionary*/
    [PenMacIOSMTKViewDelegate AddUniform:layerId :mvp];
}

void MapMacIOSUpdateUniforms(){
    /*Updates the uniform data*/
    [PenMacIOSMTKViewDelegate UpdateUniforms];
}

void MapMacIOSSubmitBatch(BatchVertexData* data, int size){
    /*Submits the vertex data to the GPU*/
    [PenMacIOSMTKViewDelegate SubmitBatch:[PenMacIOSVertexBuffer Get].iosVertexBuffer :data :size];
}

void MapMacIOSRender(unsigned int shapeType){
    /*Render the ios mtk view*/
    [PenMacIOSMTKViewDelegate Render:shapeType];
}

void MapMacIOSBackground(float r, float g, float b, float a){
    /*Updates the background of the mtk window*/
    [PenMacIOSMTKViewDelegate Background:r :g :b :a];
}
#endif
