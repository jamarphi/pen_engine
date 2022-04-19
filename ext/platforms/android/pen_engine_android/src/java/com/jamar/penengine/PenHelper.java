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

import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.media.AudioManager;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.os.Vibrator;
import android.preference.PreferenceManager.OnActivityResultListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.DisplayCutout;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import com.android.vending.expansion.zipfile.APKExpansionSupport;
import com.android.vending.expansion.zipfile.ZipResourceFile;

import java.io.IOException;
import java.io.File;
import java.io.FilenameFilter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;


public class PenHelper {
    private static final String PREFS_NAME = "PenPrefsFile";
    private static final int RUNNABLES_PER_FRAME = 5;
    private static final String TAG = PenHelper.class.getSimpleName();
    private static PenAccelerometer sPenAccelerometer = null;
    private static boolean sAccelerometerEnabled;
    private static boolean sCompassEnabled;
    private static boolean sActivityVisible;
    private static String sPackageName;
    private static Activity sActivity = null;
    private static PenHelperListener sPenHelperListener;
    private static Set<OnActivityResultListener> onActivityResultListeners = new LinkedHashSet<OnActivityResultListener>();
    private static Vibrator sVibrateService = null;
    private static boolean sInited = false;

    public static void runOnGLThread(final Runnable r) {
        ((MainActivity)sActivity).runOnGLThread(r);
    }

    public static void init(final Activity activity) {
        sActivity = activity;
        PenHelper.sPenHelperListener = (PenHelperListener)activity;
        if (!sInited) {

            PackageManager pm = activity.getPackageManager();
            boolean isSupportLowLatency = pm.hasSystemFeature(PackageManager.FEATURE_AUDIO_LOW_LATENCY);

            Log.d(TAG, "isSupportLowLatency:" + isSupportLowLatency);

            int sampleRate = 44100;
            int bufferSizeInFrames = 192;

            if (Build.VERSION.SDK_INT >= 17) {
                AudioManager am = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
                // use reflection to remove dependence of API 17 when compiling

                // AudioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
                final Class audioManagerClass = AudioManager.class;
                Object[] parameters = new Object[]{PenReflectionHelper.<String>getConstantValue(audioManagerClass, "PROPERTY_OUTPUT_SAMPLE_RATE")};
                final String strSampleRate = PenReflectionHelper.<String>invokeInstanceMethod(am, "getProperty", new Class[]{String.class}, parameters);

                // AudioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
                parameters = new Object[]{PenReflectionHelper.<String>getConstantValue(audioManagerClass, "PROPERTY_OUTPUT_FRAMES_PER_BUFFER")};
                final String strBufferSizeInFrames = PenReflectionHelper.<String>invokeInstanceMethod(am, "getProperty", new Class[]{String.class}, parameters);

                try {
                    sampleRate = Integer.parseInt(strSampleRate);
                    bufferSizeInFrames = Integer.parseInt(strBufferSizeInFrames);
                } catch (NumberFormatException e) {
                    Log.e(TAG, "parseInt failed", e);
                }
                Log.d(TAG, "sampleRate: " + sampleRate + ", framesPerBuffer: " + bufferSizeInFrames);
            } else {
                Log.d(TAG, "android version is lower than 17");
            }

            nativeSetAudioDeviceInfo(isSupportLowLatency, sampleRate, bufferSizeInFrames);
            final ApplicationInfo applicationInfo = activity.getApplicationInfo();
            PenHelper.sPackageName = applicationInfo.packageName;
            PenHelper.sVibrateService = (Vibrator)activity.getSystemService(Context.VIBRATOR_SERVICE);
            sInited = true;
        }
    }
    
    public static Activity getActivity() {
        return sActivity;
    }
    
    public static void addOnActivityResultListener(OnActivityResultListener listener) {
        onActivityResultListeners.add(listener);
    }
    
    public static Set<OnActivityResultListener> getOnActivityResultListeners() {
        return onActivityResultListeners;
    }
    
    public static boolean isActivityVisible(){
        return sActivityVisible;
    }

    public static void enableAccelerometer() {
        PenHelper.sAccelerometerEnabled = true;
        PenHelper.getAccelerometer().enableAccel();
    }

    public static void enableCompass() {
        PenHelper.sCompassEnabled = true;
        PenHelper.getAccelerometer().enableCompass();
    }

    public static void setAccelerometerInterval(float interval) {
        PenHelper.getAccelerometer().setInterval(interval);
    }

    public static void disableAccelerometer() {
        PenHelper.sAccelerometerEnabled = false;
        PenHelper.getAccelerometer().disable();
    }

    public static void setKeepScreenOn(boolean value) {
        ((MainActivity)sActivity).setKeepScreenOn(value);
    }

    public static void vibrate(float duration) {
        sVibrateService.vibrate((long)(duration * 1000));
    }

    public static void onResume() {
        sActivityVisible = true;
        if (PenHelper.sAccelerometerEnabled) {
            PenHelper.getAccelerometer().enableAccel();
        }
        if (PenHelper.sCompassEnabled) {
            PenHelper.getAccelerometer().enableCompass();
        }
    }

    public static void onPause() {
        sActivityVisible = false;
        if (PenHelper.sAccelerometerEnabled) {
            PenHelper.getAccelerometer().disable();
        }
    }
    
    public static void terminateProcess() {
        // Remove it from recent apps.
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            sActivity.finishAndRemoveTask();
        }
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    public static interface PenHelperListener {
        public void showDialog(final String pTitle, final String pMessage);

        public void runOnGLThread(final Runnable pRunnable);
    }

    public static float[] getCompassValue() {
        return PenHelper.getAccelerometer().compassFieldValues;
    }

    private static PenAccelerometer getAccelerometer() {
        if (null == sPenAccelerometer)
            PenHelper.sPenAccelerometer = new PenAccelerometer(sActivity);

        return sPenAccelerometer;
    }
}