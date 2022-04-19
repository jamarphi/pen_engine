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
package com.jamar.penengine;

import android.content.Context;
import android.media.AudioManager;
import android.util.Log;

class PenAudioFocusManager {

    private final static String TAG = "AudioFocusManager";
    private final static int AUDIOFOCUS_GAIN = 0;
    private final static int AUDIOFOCUS_LOST = 1;
    private final static int AUDIOFOCUS_LOST_TRANSIENT = 2;
    private final static int AUDIOFOCUS_LOST_TRANSIENT_CAN_DUCK = 3;

    private static AudioManager.OnAudioFocusChangeListener sAfChangeListener =
            new AudioManager.OnAudioFocusChangeListener() {
                public void onAudioFocusChange(int focusChange) {

                    if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
                        // Permanent loss of audio focus
                        // Pause playback immediately
                        PenHelper.runOnGLThread(new Runnable() {
                            @Override
                            public void run() {
                                nativeOnAudioFocusChange(AUDIOFOCUS_LOST);
                            }
                        });

                    } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT) {
                        // Pause playback
                        PenHelper.runOnGLThread(new Runnable() {
                            @Override
                            public void run() {
                                nativeOnAudioFocusChange(AUDIOFOCUS_LOST_TRANSIENT);
                            }
                        });
                    } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK) {
                        // Lower the volume, keep playing
                        PenHelper.runOnGLThread(new Runnable() {
                            @Override
                            public void run() {
                                nativeOnAudioFocusChange(AUDIOFOCUS_LOST_TRANSIENT_CAN_DUCK);
                            }
                        });
                    } else if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
                        // Your pen_engine_android has been granted audio focus again
                        // Raise volume to normal, restart playback if necessary
                        PenHelper.runOnGLThread(new Runnable() {
                            @Override
                            public void run() {
                                nativeOnAudioFocusChange(AUDIOFOCUS_GAIN);
                            }
                        });
                    }
                }
            };

    static boolean registerAudioFocusListener(Context context) {
        AudioManager am = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);

        // Request audio focus for playback
        int result = am.requestAudioFocus(sAfChangeListener,
                // Use the music stream.
                AudioManager.STREAM_MUSIC,
                // Request permanent focus.
                AudioManager.AUDIOFOCUS_GAIN);

        if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
            return true;
        }

        return false;
    }

    static void unregisterAudioFocusListener(Context context) {
        AudioManager am = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        int result = am.abandonAudioFocus(sAfChangeListener);
        PenHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                nativeOnAudioFocusChange(AUDIOFOCUS_GAIN);
            }
        });
    }

    private static native void nativeOnAudioFocusChange(int focusChange);
}