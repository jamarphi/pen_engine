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

#import "mac_ios_view_controller.h"

#ifdef __PEN_MAC_IOS__
#if !TARGET_OS_OSX
@implementation PenIOSViewController
{
    MTKView* _view;
    PenMacIOSMTKViewDelegate* _renderer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    _view.preferredFramesPerSecond = 30;
    [[_view.window rootViewController] prefersStatusBarHidden];
    [_view setColorPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB];

    /*Initializes the mobile mouse coordinate vector before any events occur*/
    pen::State::Get()->mobileMouse = new std::vector<pen::Tap*>();
    
    NSAssert(_view.device, @"Metal is not supported on this device");

    dispatch_queue_t q = dispatch_get_global_queue( QOS_CLASS_USER_INITIATED, 0 );
    
    CGSize size = _view.bounds.size;
    __weak PenIOSViewController* weakSelf = self;
    dispatch_async( q, ^(){
        PenIOSViewController* strongSelf;
        if ( (strongSelf = weakSelf) )
        {
            strongSelf->_renderer = [[PenMacIOSMTKViewDelegate alloc] initWithMetalKitView:strongSelf->_view size:size];

            NSAssert(strongSelf->_renderer, @"Renderer failed initialization");

            dispatch_async( dispatch_get_main_queue(), ^(){
                PenIOSViewController* innerStrongSelf;
                if ( (innerStrongSelf = weakSelf) )
                {
                    [innerStrongSelf->_renderer mtkView:innerStrongSelf->_view
                                 drawableSizeWillChange:innerStrongSelf->_view.drawableSize];

                    innerStrongSelf->_view.delegate = innerStrongSelf->_renderer;
                }
            });
        }
    } );
}
@end
#endif
#endif
