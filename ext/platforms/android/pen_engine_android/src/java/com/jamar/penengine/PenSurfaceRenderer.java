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
import android.content.res.Resources;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.graphics.Matrix;

import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class PenSurfaceRenderer implements GLSurfaceView.Renderer {

    private final static long NANOSECONDSPERSECOND = 1000000000L;
    private final static long NANOSECONDSPERMICROSECOND = 1000000L;

    // The final animation interval which is used in 'onDrawFrame'
    private static long sAnimationInterval = (long) (1.0f / 60f * PenSurfaceRenderer.NANOSECONDSPERSECOND);

    private long mLastTickInPenSeconds;
    private int mScreenWidth;
    private int mScreenHeight;
    private boolean mNativeInitCompleted = false;
    private int[] mTexture = new int[8];
    private Context mContext;
    private static MainActivity mainActivity;
    public static Bitmap pixelBitmap = null;
    private static Bitmap spriteBitmap = null;
    private AssetManager assetManager;

    public PenSurfaceRenderer(Context context, MainActivity appMainActivity){

        mContext = context;
        mainActivity = appMainActivity;
    }

    public static void setAnimationInterval(float interval) {
        sAnimationInterval = (long) (interval * PenSurfaceRenderer.NANOSECONDSPERSECOND);
    }

    public void setScreenWidthAndHeight(final int surfaceWidth, final int surfaceHeight) {
        this.mScreenWidth = surfaceWidth;
        this.mScreenHeight = surfaceHeight;
    }

    private static native void loadAssetManager(AssetManager assetManager);

    @Override
    public void onSurfaceCreated(final GL10 gl, final EGLConfig EGLConfig) {
        PenSurfaceRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
        loadTextures(gl, mContext);
        this.mLastTickInPenSeconds = System.nanoTime();
        mNativeInitCompleted = true;
        assetManager = mContext.getResources().getAssets();
        loadAssetManager(assetManager);
    }

    @Override
    public void onSurfaceChanged(final GL10 GL10, final int width, final int height) {
        PenSurfaceRenderer.nativeOnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(final GL10 gl) {
        /*
         * No need to use algorithm in default(60 FPS) situation,
         * since onDrawFrame() was called by system 60 times per second by default.
         */

        if (PenSurfaceRenderer.sAnimationInterval <= 1.0f / 60f * PenSurfaceRenderer.NANOSECONDSPERSECOND) {
            PenSurfaceRenderer.nativeRender();
            updatePixels(gl);
        } else {
            final long now = System.nanoTime();
            final long interval = now - this.mLastTickInPenSeconds;
        
            if (interval < PenSurfaceRenderer.sAnimationInterval) {
                try {
                    Thread.sleep((PenSurfaceRenderer.sAnimationInterval - interval) / PenSurfaceRenderer.NANOSECONDSPERMICROSECOND);
                } catch (final Exception e) {
                }
            }
            /*
             * Render time MUST be counted in, or the FPS will slower than appointed.
            */
            this.mLastTickInPenSeconds = System.nanoTime();
            PenSurfaceRenderer.nativeRender();
            updatePixels(gl);
        }
    }

    private static native boolean isPixelDrawn();

    private void updatePixels(final GL10 gl){
        /*Updates pixel buffer texture*/
        if(isPixelDrawn()){
            gl.glActiveTexture(GL10.GL_TEXTURE0 + 2);
            gl.glBindTexture(GL10.GL_TEXTURE_2D, mTexture[2]);
            GLUtils.texSubImage2D(GL10.GL_TEXTURE_2D, 0, 0, 0, pixelBitmap);
        }
    }

    private Bitmap loadBitmap(Context context, int resourceId){
        /*Load in bitmaps via drawables for textures*/
        InputStream is = context.getResources().openRawResource(resourceId);
        Matrix texOrientation = new Matrix();
        texOrientation.postScale(1.0f, -1.0f);
        Bitmap bitmap = null;
        Bitmap returnBitmap = null;
        try{
            bitmap = BitmapFactory.decodeStream(is);
        }finally{
            try{
                is.close();
                returnBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), texOrientation, true);
            }catch(IOException e){
                e.printStackTrace();
            }
        }
        return returnBitmap;
    }

    private Bitmap loadBitmapViaPixelArray(){
        /*Load in the pixel buffer on java side*/
        Bitmap bitmap = null;
        int[] pixelBuffer = new int[2000000];
        try{
            for(int i = 0; i < pixelBuffer.length; i++){
                pixelBuffer[i] = 1;
            }
            bitmap = Bitmap.createBitmap(pixelBuffer, 0, 1000, 1000, 500, Bitmap.Config.valueOf("ARGB_8888"));
        }catch(Exception e){
            e.printStackTrace();
        }
        return bitmap;
    }

    public static byte[] loadSprite(String name){
        /*Load in a drawable to be sent to the native side*/
        Resources resources = mainActivity.getBaseContext().getResources();
        int resourceId = resources.getIdentifier(name, "drawable",
                mainActivity.getBaseContext().getPackageName());
        Bitmap tempBitmap = null;

        InputStream is = mainActivity.getBaseContext().getResources().openRawResource(resourceId);
        Matrix texOrientation = new Matrix();
        texOrientation.postScale(1.0f, 1.0f);
        Bitmap bitmap = null;
        try{
            bitmap = BitmapFactory.decodeStream(is);
        }finally{
            try{
                is.close();
                tempBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), texOrientation, true);
            }catch(IOException e){
                e.printStackTrace();
            }
        }

        /*Convert bitmap to byte array*/
        int[] spriteIntArr = new int[tempBitmap.getWidth() * tempBitmap.getHeight()];
        tempBitmap.getPixels(spriteIntArr, 0, tempBitmap.getWidth(), 0, 0, tempBitmap.getWidth(), tempBitmap.getHeight());

        byte[] spriteArr = new byte[tempBitmap.getWidth() * tempBitmap.getHeight() * 4];
        for(int i = 0; i < spriteIntArr.length; i++){
            spriteArr[i * 4] = (byte) Color.red(spriteIntArr[i]);
            spriteArr[i * 4 + 1] = (byte) Color.green(spriteIntArr[i]);
            spriteArr[i * 4 + 2] = (byte) Color.blue(spriteIntArr[i]);
            spriteArr[i * 4 + 3] = (byte) Color.alpha(spriteIntArr[i]);
        }
        spriteBitmap = tempBitmap.copy(Bitmap.Config.ARGB_8888,true);
        tempBitmap.recycle();

        return spriteArr;
    }

    public static int getSpriteWidth(){
        /*Grab the width of the recently loaded sprite*/
        return spriteBitmap.getWidth();
    }

    public static int getSpriteHeight(){
        /*Grab the height of the recently loaded sprite*/
        int height = spriteBitmap.getHeight();

        /*Remove the sprite bitmap*/
        spriteBitmap.recycle();
        spriteBitmap = null;

        return height;
    }

    private static native String nativeImageName(final int id);

    private void loadTextures(GL10 gl, Context context){
        /*Load in textures on java side similar to native side*/
        gl.glGenTextures(8, mTexture, 0);

        for(int i = 0; i < mTexture.length; i++){
            /*Bind texture to a slot*/
            gl.glActiveTexture(GL10.GL_TEXTURE0 + i);
            gl.glBindTexture(GL10.GL_TEXTURE_2D, mTexture[i]);

            if (i == 0){
                /*Default texture*/
                Bitmap bitmap = loadBitmap(context, R.drawable.default_tex);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap , 0);
                bitmap.recycle();
            } else if (i == 1){
                /*Load in the bitmap font*/
                Bitmap bitmap = loadBitmap(context, R.drawable.bitmap);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap , 0);
                bitmap.recycle();
            }else if (i == 2){
                /*Pixel buffer*/
                Bitmap bitmap = loadBitmapViaPixelArray();
                pixelBitmap = bitmap.copy(Bitmap.Config.ARGB_8888,true);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D,GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, pixelBitmap , 0);
                bitmap.recycle();
            }else{
                /*Load in the texture*/
                String name = nativeImageName(i);
                if(!name.equals("")) {
                    Resources resources = context.getResources();
                    int resourceId = resources.getIdentifier(name, "drawable",
                            context.getPackageName());
                    Bitmap bitmap = loadBitmap(context, resourceId);
                    gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                    gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
                    GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
                    bitmap.recycle();
                }
            }
        }
    }

    private static native void nativeTouchesBegin(final int id, final float x, final float y);
    private static native void nativeTouchesEnd(final int id, final float x, final float y);
    private static native void nativeTouchesMove(final int[] ids, final float[] xs, final float[] ys);
    private static native boolean nativeKeyEvent(final int keyCode,boolean isPressed);
    private static native void nativeRender();
    private static native void nativeInit(final int width, final int height);
    private static native void nativeOnSurfaceChanged(final int width, final int height);
    private static native void nativeOnPause();
    private static native void nativeOnResume();

    public void handleActionDown(final int id, final float x, final float y) {
        PenSurfaceRenderer.nativeTouchesBegin(id, x, y);
    }

    public void handleActionUp(final int id, final float x, final float y) {
        PenSurfaceRenderer.nativeTouchesEnd(id, x, y);
    }

    public void handleActionMove(final int[] ids, final float[] xs, final float[] ys) {
        PenSurfaceRenderer.nativeTouchesMove(ids, xs, ys);
    }

    public void handleKeyDown(final int keyCode) {
        PenSurfaceRenderer.nativeKeyEvent(keyCode, true);
    }

    public void handleKeyUp(final int keyCode) {
        PenSurfaceRenderer.nativeKeyEvent(keyCode, false);
    }

    public void handleOnPause() {
        /**
         * onPause may be invoked before onSurfaceCreated, 
         * and engine will be initialized correctly after
         * onSurfaceCreated is invoked. Can not invoke any
         * native method before onSurfaceCreated is invoked
         */
        if (! mNativeInitCompleted)
            return;

        PenSurfaceRenderer.nativeOnPause();
    }

    public void handleOnResume() {
        if (! mNativeInitCompleted)
            return;

        PenSurfaceRenderer.nativeOnResume();
    }

    public static void drawPixels(int x, int y, float r, float g, float b, float a){
        /*Draws pixel to pixel buffer bitmap*/
        int color = ((int) (a * 255.0f + 0.5f) << 24) |
                ((int) (r   * 255.0f + 0.5f) << 16) |
                ((int) (g * 255.0f + 0.5f) <<  8) |
                (int) (b  * 255.0f + 0.5f);
        pixelBitmap.setPixel(x,y, color);
    }

    public static void flushPixels(){
        /*Clear the pixel buffer*/
        int[] pixels = new int[2000000];
        for(int i = 0; i < pixels.length; i++){
            pixels[i] = 0;
        }
        pixelBitmap.setPixels(pixels, 0, 1000, 0, 0, 1000, 500);
    }
}