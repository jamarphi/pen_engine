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
package com.jamar.pen_engine;

import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.media.AudioManager;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.os.PowerManager;
import android.preference.PreferenceManager.OnActivityResultListener;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import com.jamar.pen_engine.PenHelper.PenHelperListener;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLContext;

public class MainActivity extends Activity implements PenHelperListener {
    // ===========================================================
    // Constants
    // ===========================================================

    private final static String TAG = MainActivity.class.getSimpleName();

    // ===========================================================
    // Fields
    // ===========================================================
    
    private PenSurfaceView mGLSurfaceView = null;
    private int[] mGLContextAttrs = null;
    private static MainActivity sContext = null;
    private PenVideoHelper mVideoHelper = null;
    private boolean hasFocus = false;
    private boolean showVirtualButton = false;
    private boolean gainAudioFocus = false;
    private boolean paused = true;

    public PenSurfaceView getGLSurfaceView(){
        return  mGLSurfaceView;
    }

    public static Context getContext() {
        return sContext;
    }

    public static void appLog(String message){
        System.out.println(message);
    }

    public void setKeepScreenOn(boolean value) {
        final boolean newValue = value;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mGLSurfaceView.setKeepScreenOn(newValue);
            }
        });
    }

    public void setEnableVirtualButton(boolean value) {
        this.showVirtualButton = value;
    }

    public void setEnableAudioFocusGain(boolean value) {
        if(gainAudioFocus != value) {
            if(!paused) {
                if (value)
                    PenAudioFocusManager.registerAudioFocusListener(this);
                else
                    PenAudioFocusManager.unregisterAudioFocusListener(this);
            }
            gainAudioFocus = value;
        }
    }

    protected void onLoadNativeLibraries() {
        try {
            //ApplicationInfo ai = getPackageManager().getApplicationInfo(getPackageName(), PackageManager.GET_META_DATA);
            //Bundle bundle = ai.metaData;
            //String libName = bundle.getString("lib_name");
            /*Path to generated lib*/
            System.loadLibrary("PEN_ENGINE_ANDROID_LIB");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    // ===========================================================
    // Constructors
    // ===========================================================

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            finish();
            Log.w(TAG, "[Workaround] Ignore the activity started from icon!");
            return;
        }

        this.hideVirtualButton();

        onLoadNativeLibraries();

        sContext = this;
        
        PenHelper.init(this);
        
        this.mGLContextAttrs = getGLContextAttrs();
        this.init();

        if (mVideoHelper == null) {
            mVideoHelper = new PenVideoHelper(this, mFrameLayout);
        }

        Window window = this.getWindow();
        window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);

        // Audio configuration
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
    }

    //native method,call GLViewImpl::getGLContextAttrs() to get the OpenGL ES context attributions
    private static native int[] getGLContextAttrs();

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    protected void onResume() {
    	Log.d(TAG, "onResume()");
        paused = false;
        super.onResume();
        if(gainAudioFocus)
            PenAudioFocusManager.registerAudioFocusListener(this);
        this.hideVirtualButton();
       	resumeIfHasFocus();
    }
    
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
    	Log.d(TAG, "onWindowFocusChanged() hasFocus=" + hasFocus);
        super.onWindowFocusChanged(hasFocus);
        
        this.hasFocus = hasFocus;
        resumeIfHasFocus();
    }
    
    private void resumeIfHasFocus() {
        //It is possible for the app to receive the onWindowsFocusChanged(true) event
        //even though it is locked or asleep
        boolean readyToPlay = !isDeviceLocked() && !isDeviceAsleep();

        if(hasFocus && readyToPlay) {
            this.hideVirtualButton();
        	PenHelper.onResume();
        	mGLSurfaceView.onResume();
        }
    }

    @Override
    protected void onPause() {
    	Log.d(TAG, "onPause()");
        paused = true;
        super.onPause();
        if(gainAudioFocus)
            PenAudioFocusManager.unregisterAudioFocusListener(this);
        PenHelper.onPause();
        mGLSurfaceView.onPause();
    }
    
    @Override
    protected void onDestroy() {
        if(gainAudioFocus)
            PenAudioFocusManager.unregisterAudioFocusListener(this);
        super.onDestroy();
    }

    @Override
    public void showDialog(final String pTitle, final String pMessage) {
        Message msg = new Message();
    }
    
    @Override
    public void runOnGLThread(final Runnable pRunnable) {
        this.mGLSurfaceView.queueEvent(pRunnable);
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        for (OnActivityResultListener listener : PenHelper.getOnActivityResultListeners()) {
            listener.onActivityResult(requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);
    }


    protected ResizeLayout mFrameLayout = null;
    // ===========================================================
    // Methods
    // ===========================================================
    public void init() {
        
        // FrameLayout
        ViewGroup.LayoutParams framelayout_params =
            new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                       ViewGroup.LayoutParams.MATCH_PARENT);

        mFrameLayout = new ResizeLayout(this);

        mFrameLayout.setLayoutParams(framelayout_params);

        // PenSurfaceView
        this.mGLSurfaceView = this.onCreateView();

        // ...add to FrameLayout
        mFrameLayout.addView(this.mGLSurfaceView);

        // Switch to supported OpenGL (ARGB888) mode on emulator
        // this line is not needed on new emulators and also it breaks stencil buffer
        //if (isAndroidEmulator())
           //this.mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        this.mGLSurfaceView.setEGLContextClientVersion(2);
        this.mGLSurfaceView.setPenRenderer(new PenSurfaceRenderer(sContext, this));

        // Set framelayout as the content view
        setContentView(mFrameLayout);
    }

    
    public PenSurfaceView onCreateView() {
        PenSurfaceView glSurfaceView = new PenSurfaceView(this);
        //this line is need on some device if we specify an alpha bits
        // FIXME: is it needed? And it will cause afterimage.
         //if(this.mGLContextAttrs[3] > 0) glSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);

        // use custom EGLConfigureChooser
        PenEGLConfigChooser chooser = new PenEGLConfigChooser(this.mGLContextAttrs);
        glSurfaceView.setEGLConfigChooser(chooser);

        return glSurfaceView;
    }

    protected void hideVirtualButton() {
        if (showVirtualButton) {
            return;
        }

        if (Build.VERSION.SDK_INT >= 19) {
            // use reflection to remove dependence of API level

            Class viewClass = View.class;

            try {
                final int SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION");
                final int SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN");
                final int SYSTEM_UI_FLAG_HIDE_NAVIGATION = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION");
                final int SYSTEM_UI_FLAG_FULLSCREEN = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN");
                final int SYSTEM_UI_FLAG_IMMERSIVE_STICKY = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY");
                final int SYSTEM_UI_FLAG_LAYOUT_STABLE = PenReflectionHelper.<Integer>getConstantValue(viewClass, "SYSTEM_UI_FLAG_LAYOUT_STABLE");

                // getWindow().getDecorView().setSystemUiVisibility();
                final Object[] parameters = new Object[]{SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                        | SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                        | SYSTEM_UI_FLAG_IMMERSIVE_STICKY};
                PenReflectionHelper.<Void>invokeInstanceMethod(getWindow().getDecorView(),
                        "setSystemUiVisibility",
                        new Class[]{Integer.TYPE},
                        parameters);
            } catch (NullPointerException e) {
                Log.e(TAG, "hideVirtualButton", e);
            }
        }
    }

   private static boolean isAndroidEmulator() {
      String model = Build.MODEL;
      Log.d(TAG, "model=" + model);
      String product = Build.PRODUCT;
      Log.d(TAG, "product=" + product);
      boolean isEmulator = false;
      if (product != null) {
         isEmulator = product.equals("sdk") || product.contains("_sdk") || product.contains("sdk_");
      }
      Log.d(TAG, "isEmulator=" + isEmulator);
      return isEmulator;
   }

    private static boolean isDeviceLocked() {
        KeyguardManager keyguardManager = (KeyguardManager)getContext().getSystemService(Context.KEYGUARD_SERVICE);
        boolean locked = keyguardManager.inKeyguardRestrictedInputMode();
        return locked;
    }

    private static boolean isDeviceAsleep() {
        PowerManager powerManager = (PowerManager)getContext().getSystemService(Context.POWER_SERVICE);
        if(powerManager == null) {
            return false;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT_WATCH) {
            return !powerManager.isInteractive();
        } else {
            return !powerManager.isScreenOn();
        }
    }
    
    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    private class PenEGLConfigChooser implements GLSurfaceView.EGLConfigChooser
    {
        private int[] mConfigAttributes;
        private  final int EGL_OPENGL_ES2_BIT = 0x04;
        private  final int EGL_OPENGL_ES3_BIT = 0x40;
        public PenEGLConfigChooser(int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize, int multisamplingCount)
        {
            mConfigAttributes = new int[] {redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize, multisamplingCount};
        }
        public PenEGLConfigChooser(int[] attributes)
        {
            mConfigAttributes = attributes;
        }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
        {
            int[][] EGLAttributes = {
                {
                    // GL ES 2 with user set
                    EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                    EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                    EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                    EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                    EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4],
                    EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                    EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                    EGL10.EGL_SAMPLES, mConfigAttributes[6],
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL10.EGL_NONE
                },
                {
                     // GL ES 2 with user set 16 bit depth buffer
                     EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                     EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                     EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                     EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                     EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                     EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                     EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                     EGL10.EGL_SAMPLES, mConfigAttributes[6],
                     EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                     EGL10.EGL_NONE
                },
                {
                     // GL ES 2 with user set 16 bit depth buffer without multisampling
                     EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                     EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                     EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                     EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                     EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                     EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                     EGL10.EGL_SAMPLE_BUFFERS, 0,
                     EGL10.EGL_SAMPLES, 0,
                     EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                     EGL10.EGL_NONE
                },
                {
                    // GL ES 2 by default
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL10.EGL_NONE
                }
            };

            EGLConfig result = null;
            for (int[] eglAtribute : EGLAttributes) {
                result = this.doChooseConfig(egl, display, eglAtribute);
                if (result != null)
                    return result;
            }

            Log.e(DEVICE_POLICY_SERVICE, "Can not select an EGLConfig for rendering.");
            return null;
        }

        private EGLConfig doChooseConfig(EGL10 egl, EGLDisplay display, int[] attributes) {
            EGLConfig[] configs = new EGLConfig[1];
            int[] matchedConfigNum = new int[1];
            boolean result = egl.eglChooseConfig(display, attributes, configs, 1, matchedConfigNum);
            if (result && matchedConfigNum[0] > 0) {
                return configs[0];
            }
            return null;
        }
    }
}