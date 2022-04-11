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
    // ===========================================================
    // Constants
    // ===========================================================
    private static final String PREFS_NAME = "PenPrefsFile";
    private static final int RUNNABLES_PER_FRAME = 5;
    private static final String TAG = PenHelper.class.getSimpleName();

    // ===========================================================
    // Fields
    // ===========================================================
    private static AssetManager sAssetManager;
    private static PenAccelerometer sPenAccelerometer = null;
    private static boolean sAccelerometerEnabled;
    private static boolean sCompassEnabled;
    private static boolean sActivityVisible;
    private static String sPackageName;
    private static Activity sActivity = null;
    private static PenHelperListener sPenHelperListener;
    private static Set<OnActivityResultListener> onActivityResultListeners = new LinkedHashSet<OnActivityResultListener>();
    private static Vibrator sVibrateService = null;

    // The absolute path to the OBB if it exists, else the absolute path to the APK.
    private static String sAssetsPath = "";
    
    // The OBB file
    private static ZipResourceFile sOBBFile = null;

    // ===========================================================
    // Constructors
    // ===========================================================

    public static void runOnGLThread(final Runnable r) {
        ((MainActivity)sActivity).runOnGLThread(r);
    }

    private static boolean sInited = false;
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

            PenHelper.sAssetManager = activity.getAssets();
            PenHelper.nativeSetContext((Context)activity, PenHelper.sAssetManager);
    
            PenBitmap.setContext(activity);

            PenHelper.sVibrateService = (Vibrator)activity.getSystemService(Context.VIBRATOR_SERVICE);

            sInited = true;
        }
    }
    
    // This function returns the absolute path to the OBB if it exists,
    // else it returns the absolute path to the APK.
    public static String getAssetsPath()
    {
        if (PenHelper.sAssetsPath.equals("")) {

            String pathToOBB = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + PenHelper.sPackageName;

	    	// Listing all files inside the folder (pathToOBB) where OBB files are expected to be found.
            String[] fileNames = new File(pathToOBB).list(new FilenameFilter() { // Using filter to pick up only main OBB file name.
                public boolean accept(File dir, String name) {
                    return name.startsWith("main.") && name.endsWith(".obb");  // It's possible to filter only by extension here to get path to patch OBB file also.
                }
            });

            String fullPathToOBB = "";
            if (fileNames != null && fileNames.length > 0)  // If there is at least 1 element inside the array with OBB file names, then we may think fileNames[0] will have desired main OBB file name.
                fullPathToOBB = pathToOBB + "/" + fileNames[0];  // Composing full file name for main OBB file.

            File obbFile = new File(fullPathToOBB);
            if (obbFile.exists())
                PenHelper.sAssetsPath = fullPathToOBB;
            else
                PenHelper.sAssetsPath = PenHelper.sActivity.getApplicationInfo().sourceDir;
        }
        
        return PenHelper.sAssetsPath;
    }
    
    public static ZipResourceFile getObbFile() {
        if (null == sOBBFile) {
            int versionCode = 1;
            try {
                versionCode = MainActivity.getContext().getPackageManager().getPackageInfo(PenHelper.getActivityPackageName(), 0).versionCode;
            } catch (NameNotFoundException e) {
                e.printStackTrace();
            }

            try {
                sOBBFile = APKExpansionSupport.getAPKExpansionZipFile(MainActivity.getContext(), versionCode, 0);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return sOBBFile;
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

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

    private static native void nativeSetContext(final Context pContext, final AssetManager pAssetManager);

    private static native void nativeSetAudioDeviceInfo(boolean isSupportLowLatency, int deviceSampleRate, int audioBufferSizeInFames);

    public static String getActivityPackageName() {
        return PenHelper.sPackageName;
    }
    public static String getActivityWritablePath() {
        return sActivity.getFilesDir().getAbsolutePath();
    }

    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }
    
    public static String getDeviceModel(){
        return Build.MODEL;
    }

    public static AssetManager getAssetManager() {
        return PenHelper.sAssetManager;
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

 	public static String getVersion() {
 		try {
 			String version = MainActivity.getContext().getPackageManager().getPackageInfo(MainActivity.getContext().getPackageName(), 0).versionName;
 			return version;
 		} catch(Exception e) {
 			return "";
 		}
 	}

    public static boolean openURL(String url) { 
        boolean ret = false;
        try {
            Intent i = new Intent(Intent.ACTION_VIEW);
            i.setData(Uri.parse(url));
            sActivity.startActivity(i);
            ret = true;
        } catch (Exception e) {
        }
        return ret;
    }
    
    public static long[] getObbAssetFileDescriptor(final String path) {
        long[] array = new long[3];
        if (PenHelper.getObbFile() != null) {
            AssetFileDescriptor descriptor = PenHelper.getObbFile().getAssetFileDescriptor(path);
            if (descriptor != null) {
                try {
                    ParcelFileDescriptor parcel = descriptor.getParcelFileDescriptor();
                    Method method = parcel.getClass().getMethod("getFd", new Class[] {});
                    array[0] = (Integer)method.invoke(parcel);
                    array[1] = descriptor.getStartOffset();
                    array[2] = descriptor.getLength();
                } catch (NoSuchMethodException e) {
                    Log.e(PenHelper.TAG, "Accessing file descriptor directly from the OBB is only supported from Android 3.1 (API level 12) and above.");
                } catch (IllegalAccessException e) {
                    Log.e("", e.toString());
                } catch (InvocationTargetException e) {
                    Log.e(PenHelper.TAG, e.toString());
                }
            }
        }
        return array;
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

    public static int getDPI()
    {
        if (sActivity != null)
        {
            DisplayMetrics metrics = new DisplayMetrics();
            WindowManager wm = sActivity.getWindowManager();
            if (wm != null)
            {
                Display d = wm.getDefaultDisplay();
                if (d != null)
                {
                    d.getMetrics(metrics);
                    return (int)(metrics.density*160.0f);
                }
            }
        }
        return -1;
    }
    
    // ===========================================================
    // Functions for CCUserDefault
    // ===========================================================
    
    public static boolean getBoolForKey(String key, boolean defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        try {
            return settings.getBoolean(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String)
            {
                return  Boolean.parseBoolean(value.toString());
            }
            else if (value instanceof Integer)
            {
                int intValue = ((Integer) value).intValue();
                return (intValue !=  0) ;
            }
            else if (value instanceof Float)
            {
                float floatValue = ((Float) value).floatValue();
                return (floatValue != 0.0f);
            }
        }

        return defaultValue;
    }
    
    public static int getIntegerForKey(String key, int defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        try {
            return settings.getInt(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String) {
                return  Integer.parseInt(value.toString());
            }
            else if (value instanceof Float)
            {
                return ((Float) value).intValue();
            }
            else if (value instanceof Boolean)
            {
                boolean booleanValue = ((Boolean) value).booleanValue();
                if (booleanValue)
                    return 1;
            }
        }

        return defaultValue;
    }
    
    public static float getFloatForKey(String key, float defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        try {
            return settings.getFloat(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String) {
                return  Float.parseFloat(value.toString());
            }
            else if (value instanceof Integer)
            {
                return ((Integer) value).floatValue();
            }
            else if (value instanceof Boolean)
            {
                boolean booleanValue = ((Boolean) value).booleanValue();
                if (booleanValue)
                    return 1.0f;
            }
        }

        return defaultValue;
    }
    
    public static double getDoubleForKey(String key, double defaultValue) {
        // SharedPreferences doesn't support saving double value
        return getFloatForKey(key, (float) defaultValue);
    }
    
    public static String getStringForKey(String key, String defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        try {
            return settings.getString(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();
            
            return settings.getAll().get(key).toString();
        }
    }
    
    public static void setBoolForKey(String key, boolean value) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(key, value);
        editor.apply();
    }
    
    public static void setIntegerForKey(String key, int value) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt(key, value);
        editor.apply();
    }
    
    public static void setFloatForKey(String key, float value) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, value);
        editor.apply();
    }
    
    public static void setDoubleForKey(String key, double value) {
        // SharedPreferences doesn't support recording double value
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, (float)value);
        editor.apply();
    }
    
    public static void setStringForKey(String key, String value) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(key, value);
        editor.apply();
    }
    
    public static void deleteValueForKey(String key) {
        SharedPreferences settings = sActivity.getSharedPreferences(PenHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.remove(key);
        editor.apply();
    }

    public static byte[] conversionEncoding(byte[] text, String fromCharset,String newCharset)
    {
        try {
            String str = new String(text,fromCharset);
            return str.getBytes(newCharset);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        return null;
    }
    
    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static interface PenHelperListener {
        public void showDialog(final String pTitle, final String pMessage);

        public void runOnGLThread(final Runnable pRunnable);
    }

    /**
     * Returns whether the screen has a round shape. Apps may choose to change styling based
     * on this property, such as the alignment or layout of text or informational icons.
     *
     * @return true if the screen is rounded, false otherwise
     */
    public static boolean isScreenRound() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (sActivity.getResources().getConfiguration().isScreenRound()) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns whether the window is always allowed to extend into the DisplayCutout areas on the short edges of the screen.
     *
     * @return true if the window in display cutout mode on the short edges of the screen, false otherwise
     */
    @SuppressLint("InlinedApi")
    public static boolean isCutoutEnabled() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams lp = sActivity.getWindow().getAttributes();
            return lp.layoutInDisplayCutoutMode == WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        return false;
    }

    /**
     * Returns safe insets array.
     *
     * @return array of int with safe insets values
     */
    @SuppressLint("NewApi") 
    public static int[] getSafeInsets() {
        final int[] safeInsets = new int[]{0, 0, 0, 0};
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            Window penWindow = sActivity.getWindow();
            DisplayCutout displayCutout = penWindow.getDecorView().getRootWindowInsets().getDisplayCutout();
            // Judge whether it is cutouts (aka notch) screen phone by judge cutout equle to null
            if (displayCutout != null) {
                List<Rect> rects = displayCutout.getBoundingRects();
                // Judge whether it is cutouts (aka notch) screen phone by judge cutout rects is null or zero size
                if (rects != null && rects.size() != 0) {
                    safeInsets[0] = displayCutout.getSafeInsetBottom();
                    safeInsets[1] = displayCutout.getSafeInsetLeft();
                    safeInsets[2] = displayCutout.getSafeInsetRight();
                    safeInsets[3] = displayCutout.getSafeInsetTop();
                }
            }
        }

        return safeInsets;
    }

    /**
     * Queries about whether any physical keys exist on the
     * any keyboard attached to the device and returns <code>true</code>
     * if the device does not have physical keys
     *
     * @return Returns <code>true</code> if the device have no physical keys,
     * otherwise <code>false</code> will returned.
     */
    public static boolean hasSoftKeys() {
        boolean hasSoftwareKeys = true;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            Display display = sActivity.getWindowManager().getDefaultDisplay();

            DisplayMetrics realDisplayMetrics = new DisplayMetrics();
            display.getRealMetrics(realDisplayMetrics);

            int realHeight = realDisplayMetrics.heightPixels;
            int realWidth = realDisplayMetrics.widthPixels;

            DisplayMetrics displayMetrics = new DisplayMetrics();
            display.getMetrics(displayMetrics);

            int displayHeight = displayMetrics.heightPixels;
            int displayWidth = displayMetrics.widthPixels;

            hasSoftwareKeys = (realWidth - displayWidth) > 0 ||
                    (realHeight - displayHeight) > 0;
        } else {
            boolean hasMenuKey = false;
            boolean hasBackKey = KeyCharacterMap.deviceHasKey(KeyEvent.KEYCODE_BACK);
            hasSoftwareKeys = !hasMenuKey && !hasBackKey;
        }
        return hasSoftwareKeys;
    }

    //Enhance API modification end     
    public static float[] getAccelValue() {
        return PenHelper.getAccelerometer().accelerometerValues;
    }

    public static float[] getCompassValue() {
        return PenHelper.getAccelerometer().compassFieldValues;
    }

    public static int getSDKVersion() {
        return Build.VERSION.SDK_INT;
    }

    private static PenAccelerometer getAccelerometer() {
        if (null == sPenAccelerometer)
            PenHelper.sPenAccelerometer = new PenAccelerometer(sActivity);

        return sPenAccelerometer;
    }
}