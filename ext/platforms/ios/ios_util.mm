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
#include "../../../src/state/config.h"
#import <UIKit/UIKit.h>

// Accelerometer
#import<CoreMotion/CoreMotion.h>
#import<CoreFoundation/CoreFoundation.h>
#import <CoreText/CoreText.h>
// Vibrate
#import <AudioToolbox/AudioToolbox.h>

const float MAX_MEASURE_HEIGHT = 10000;

+ (id) sharedAccelerometerDispatcher;
- (id) init;
- (void) setAccelerometerEnabled: (bool) isEnabled;
- (void) setAccelerometerInterval:(float) interval;

@end

- (void) setAccelerometerEnabled: (bool) isEnabled
{

}

-(void) setAccelerometerInterval:(float)interval
{
    
}

- (void)accelerometer:(CMAccelerometerData *)accelerometerData
{
    _acceleration->x = accelerometerData.acceleration.x;
    _acceleration->y = accelerometerData.acceleration.y;
    _acceleration->z = accelerometerData.acceleration.z;
    _acceleration->timestamp = accelerometerData.timestamp;

    double tmp = _acceleration->x;
    UIInterfaceOrientation orientation;
    if (@available(iOS 13.0, *))
    {
        orientation = [[[UIApplication sharedApplication].windows[0] windowScene] interfaceOrientation];
    }
    else
    {
        // Fallback on earlier versions
        orientation = [[UIApplication sharedApplication] statusBarOrientation];
    }
    
    switch (orientation)
    {
        case UIInterfaceOrientationLandscapeRight:
            _acceleration->x = -_acceleration->y;
            _acceleration->y = tmp;
            break;
            
        case UIInterfaceOrientationLandscapeLeft:
            _acceleration->x = _acceleration->y;
            _acceleration->y = -tmp;
            break;
            
        case UIInterfaceOrientationPortraitUpsideDown:
            _acceleration->x = -_acceleration->y;
            _acceleration->y = -tmp;
            break;
            
        case UIInterfaceOrientationPortrait:
            break;
        default:
            NSAssert(false, @"unknown orientation");
    }
}
@end


int Device::getDPI()
{
    static int dpi = -1;

    if (dpi == -1)
    {
        float scale = 1.0f;

        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
            scale = [[UIScreen mainScreen] scale];
        }

        UIUserInterfaceIdiom userInterfaceIdiom = [UIDevice.currentDevice userInterfaceIdiom];
        if (userInterfaceIdiom == UIUserInterfaceIdiomPad) {
            dpi = 132 * scale;
        } else if (userInterfaceIdiom == UIUserInterfaceIdiomPhone) {
            dpi = 163 * scale;
        } else {
            dpi = 160 * scale;
        }
    }
    return dpi;
}

typedef struct
{
    unsigned int height;
    unsigned int width;
    bool         isPremultipliedAlpha;
    bool         hasShadow;
    CGSize       shadowOffset;
    float        shadowBlur;
    float        shadowOpacity;
    bool         hasStroke;
    float        strokeColorR;
    float        strokeColorG;
    float        strokeColorB;
    float        strokeColorA;
    float        strokeSize;
    float        tintColorR;
    float        tintColorG;
    float        tintColorB;
    float        tintColorA;

    unsigned char*  data;

} tImageInfo;

void Device::setKeepScreenOn(bool value)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled:(BOOL)value];
}

void Device::vibrate(float duration)
{
    if([[UIDevice currentDevice].model isEqualToString:@"iPhone"])
    {
        AudioServicesPlaySystemSound (1352); //works ALWAYS as of this post
    }
    else
    {
        AudioServicesPlayAlertSound (kSystemSoundID_Vibrate);
    }
}