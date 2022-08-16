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

#include "mac_ios_sound.h"

#ifdef __PEN_MAC_IOS__

static PenMacIOSSound* instance;
static NSMutableDictionary<NSString*,AVAudioPlayer*>* sounds;

@implementation PenMacIOSSound
- (void) Play: (NSString*) file :(unsigned int) loopNum{
    /*Plays a sound*/
    AVAudioPlayer* player = [sounds objectForKey:file];
    if(!player){
        NSURL* url = [NSURL URLWithString:file];
        [player initWithContentsOfURL:url error:nil];
        [sounds setObject:player forKey:file];
        player.numberOfLoops = loopNum;
        [player play];
    }else{
        [player playAtTime:0.0];
    }
}

- (void) Pause: (NSString*) file {
    /*Pauses a sound*/
    AVAudioPlayer* player = [sounds objectForKey:file];
    if(player){
        [player pause];
    }
}

- (void) Stop: (NSString*) file {
    /*Stops a sound*/
    AVAudioPlayer* player = [sounds objectForKey:file];
    if(player){
        [player stop];
    }
}

- (void) Remove: (NSString*) file{
    /*Removes a sound*/
    AVAudioPlayer* player = [sounds objectForKey:file];
    if(player){
        [player stop];
        [sounds removeObjectForKey:file];
    }
}

+(PenMacIOSSound*) Get{
    /*Returns an instance of PenMacIOSSound*/
    if (!instance)
        instance = [[PenMacIOSSound alloc] init];
    return instance;
}
@end

void MapMacPenMacIOSSoundPlay(const char* file, unsigned int loopNum){
    /*Plays a sound*/
    [[PenMacIOSSound Get] Play:[NSString stringWithUTF8String:file] :loopNum];
}

void MapMacPenMacIOSSoundPause(const char* file){
    /*Pauses a sound*/
    [[PenMacIOSSound Get] Pause:[NSString stringWithUTF8String:file]];
}

void MapMacPenMacIOSSoundStop(const char* file){
    /*Stops a sound*/
    [[PenMacIOSSound Get] Stop:[NSString stringWithUTF8String:file]];
}

void MapMacPenMacIOSSoundRemove(const char* file){
    /*Removes a sound*/
    [[PenMacIOSSound Get] Remove:[NSString stringWithUTF8String:file]];
}

#endif

