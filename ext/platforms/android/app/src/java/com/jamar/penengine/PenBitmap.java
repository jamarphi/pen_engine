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
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.text.TextUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public final class PenBitmap {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final int HORIZONTAL_ALIGN_LEFT = 1;
    private static final int HORIZONTAL_ALIGN_RIGHT = 2;
    private static final int HORIZONTAL_ALIGN_CENTER = 3;
    private static final int VERTICAL_ALIGN_TOP = 1;
    private static final int VERTICAL_ALIGN_BOTTOM = 2;
    private static final int VERTICAL_ALIGN_CENTER = 3;

    // ===========================================================
    // Fields
    // ===========================================================

    private static Context sContext;

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    public static void setContext(final Context context) {
        PenBitmap.sContext = context;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeInitBitmapDC(final int width,
            final int height, final byte[] pixels);

    //http://egoco.de/post/19077604048/calculating-the-height-of-text-in-android
    public static int getTextHeight(String text, int maxWidth, float textSize, Typeface typeface) {
        TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG | Paint.SUBPIXEL_TEXT_FLAG);
        paint.setTextSize(textSize);
        paint.setTypeface(typeface);

        int lineCount = 0;

        int index = 0;
        int length = text.length();

        while(index < length) {
            final int charsToAdvance = paint.breakText(text, index, length, true, maxWidth, null);
            if(charsToAdvance == 0) {
                index++;
            }
            else {
                index += charsToAdvance;
                lineCount++;
            }
        }

        float actualHeight = (Math.abs(paint.ascent()) + Math.abs(paint.descent()));

        return (int)Math.floor(lineCount * actualHeight);
    }

    public static Typeface calculateShrinkTypeFace(String text, int width, int height, Layout.Alignment hAlignment, float textSize, TextPaint paint, boolean enableWrap)
    {
        if (width == 0 || height == 0) {
            return  paint.getTypeface();
        }
        float actualWidth = width + 1;
        float actualHeight = height + 1;
        float fontSize = textSize + 1;

        if (!enableWrap) {
            while (actualWidth > width || actualHeight > height) {
                fontSize = fontSize - 1;

                actualWidth = (int)Math.ceil( StaticLayout.getDesiredWidth(text, paint));
                actualHeight = getTextHeight(text, (int)actualWidth, fontSize, paint.getTypeface());

                paint.setTextSize(fontSize);
                if (fontSize <= 0) {
                    paint.setTextSize(textSize);
                    break;
                }
            }
        } else {
            while (actualHeight > height || actualWidth > width) {
                fontSize = fontSize - 1;

                Layout layout = new StaticLayout(text, paint, (int) width, hAlignment,1.0f,0.0f,false);
                actualWidth = layout.getWidth();
                actualHeight = layout.getLineTop(layout.getLineCount());

                paint.setTextSize(fontSize);

                if (fontSize <= 0) {
                    paint.setTextSize(textSize);
                    break;
                }
            }

        }
        return paint.getTypeface();
    }

    public static boolean createTextBitmapShadowStroke(byte[] bytes,  final String fontName, int fontSize,
                                                    int fontTintR, int fontTintG, int fontTintB, int fontTintA,
                                                    int alignment, int width, int height, 
                                                    boolean shadow, float shadowDX, float shadowDY, float shadowBlur, float shadowOpacity, 
                                                    boolean stroke, int strokeR, int strokeG, int strokeB, int strokeA, float strokeSize, boolean enableWrap, int overflow) {
        String string;
        if (bytes == null || bytes.length == 0) {
          return false;
        } else {
          string = new String(bytes);
        }

        Layout.Alignment hAlignment = Layout.Alignment.ALIGN_NORMAL;
        int horizontalAlignment = alignment & 0x0F;
        switch (horizontalAlignment) {
            case HORIZONTAL_ALIGN_CENTER:
                hAlignment = Layout.Alignment.ALIGN_CENTER;
                break;
            case HORIZONTAL_ALIGN_RIGHT:
                hAlignment = Layout.Alignment.ALIGN_OPPOSITE;
                break;
            case HORIZONTAL_ALIGN_LEFT:
                break;
            default:
                break;
        }

        TextPaint paint = PenBitmap.newPaint(fontName, fontSize);

        if (stroke) {
            paint.setStyle(TextPaint.Style.STROKE);
            paint.setStrokeWidth(strokeSize);
        }

        int maxWidth = width;

        if (maxWidth <= 0) {
            maxWidth = (int)Math.ceil( StaticLayout.getDesiredWidth(string, paint));
        }

        Layout layout = null;
        int layoutWidth = 0;
        int layoutHeight = 0;


        if (overflow == 1 && !enableWrap){
            int widthBoundary = (int)Math.ceil( StaticLayout.getDesiredWidth(string, paint));
            layout = new StaticLayout(string, paint, widthBoundary , hAlignment,1.0f,0.0f,false);
        }else {
            if (overflow == 2) {
                calculateShrinkTypeFace(string, width, height, hAlignment, fontSize, paint, enableWrap);
            }
            layout = new StaticLayout(string, paint, maxWidth , hAlignment,1.0f,0.0f,false);
        }

        layoutWidth = layout.getWidth();
        layoutHeight = layout.getLineTop(layout.getLineCount());

        int bitmapWidth = Math.max(layoutWidth, width);
        int bitmapHeight = layoutHeight;
        if (height > 0) {
            bitmapHeight = height;
        }

        if (overflow == 1 && !enableWrap) {
            if (width > 0) {
                bitmapWidth = width;
            }
        }

        if (bitmapWidth == 0 || bitmapHeight == 0) {
            return false;
        }

        int offsetX = 0;
        if (horizontalAlignment == HORIZONTAL_ALIGN_CENTER) {
            offsetX = (bitmapWidth - layoutWidth) / 2;
        }
        else if (horizontalAlignment == HORIZONTAL_ALIGN_RIGHT) {
            offsetX = bitmapWidth - layoutWidth;
        }

        int offsetY = 0;
        int verticalAlignment   = (alignment >> 4) & 0x0F;
        switch (verticalAlignment)
        {
            case VERTICAL_ALIGN_CENTER:
                offsetY = (bitmapHeight - layoutHeight) / 2;
                break;
            case VERTICAL_ALIGN_BOTTOM:
                offsetY = bitmapHeight - layoutHeight;
                break;
        }



        Bitmap bitmap = Bitmap.createBitmap(bitmapWidth, bitmapHeight, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        canvas.translate(offsetX, offsetY);
        if ( stroke )
        {
            paint.setARGB(strokeA, strokeR, strokeG, strokeB);
            layout.draw(canvas);
        }
        paint.setStyle(TextPaint.Style.FILL);
        paint.setARGB(fontTintA, fontTintR, fontTintG, fontTintB);
        layout.draw(canvas);

        PenBitmap.initNativeObject(bitmap);
        return true;
    }

    private static TextPaint newPaint(final String fontName, final int fontSize) {
        final TextPaint paint = new TextPaint();
        paint.setTextSize(fontSize);
        paint.setAntiAlias(true);

        paint.setTypeface(Typeface.create(fontName, Typeface.NORMAL));

        return paint;
    }

    private static void initNativeObject(final Bitmap bitmap) {
        final byte[] pixels = PenBitmap.getPixels(bitmap);
        if (pixels == null) {
            return;
        }

        PenBitmap.nativeInitBitmapDC(bitmap.getWidth(),
                bitmap.getHeight(), pixels);
    }

    private static byte[] getPixels(final Bitmap bitmap) {
        if (bitmap != null) {
            final byte[] pixels = new byte[bitmap.getWidth()
                    * bitmap.getHeight() * 4];
            final ByteBuffer buf = ByteBuffer.wrap(pixels);
            buf.order(ByteOrder.nativeOrder());
            bitmap.copyPixelsToBuffer(buf);
            return pixels;
        }

        return null;
    }

    public static int getFontSizeAccordingHeight(int height) {
        TextPaint paint = new TextPaint();
        Rect bounds = new Rect();

        paint.setTypeface(Typeface.DEFAULT);
        int text_size = 1;
        boolean found_desired_size = false;

        while (!found_desired_size) {
            paint.setTextSize(text_size);
            String text = "SghMNy";
            paint.getTextBounds(text, 0, text.length(), bounds);

            text_size++;

            if (height - bounds.height() <= 2) {
                found_desired_size = true;
            }
        }
        return text_size;
    }

    private static String getStringWithEllipsis(String string, float width,
                                                float fontSize) {
        if (TextUtils.isEmpty(string)) {
            return "";
        }

        TextPaint paint = new TextPaint();
        paint.setTypeface(Typeface.DEFAULT);
        paint.setTextSize(fontSize);

        return TextUtils.ellipsize(string, paint, width,
                TextUtils.TruncateAt.END).toString();
    }
}