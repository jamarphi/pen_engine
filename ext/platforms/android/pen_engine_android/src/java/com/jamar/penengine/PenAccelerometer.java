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
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.view.Display;
import android.view.Surface;
import android.view.WindowManager;

public class PenAccelerometer implements SensorEventListener {

    private final Context mContext;
    private final SensorManager mSensorManager;
    private final Sensor mAccelerometer;
    private final Sensor mCompass;
    private final int mNaturalOrientation;
    final float[] accelerometerValues = new float[3];
    final float[] compassFieldValues = new float[3];
    static final float ALPHA = 0.25f; // if ALPHA = 1 OR 0, no filter applies.

    public PenAccelerometer(final Context context) {
        this.mContext = context;

        this.mSensorManager = (SensorManager) this.mContext.getSystemService(Context.SENSOR_SERVICE);
        this.mAccelerometer = this.mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        this.mCompass = this.mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

        final Display display = ((WindowManager) this.mContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        this.mNaturalOrientation = display.getOrientation();
    }

    public void enableCompass() {
        this.mSensorManager.registerListener(this, this.mCompass, SensorManager.SENSOR_DELAY_GAME);
    }

    public void enableAccel() {
        this.mSensorManager.registerListener(this, this.mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
    }

    public void setInterval(float interval) {
        // Honeycomb version is 11
        if(android.os.Build.VERSION.SDK_INT < 11) {
            this.mSensorManager.registerListener(this, this.mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        } else {
            //convert seconds to microseconds
            this.mSensorManager.registerListener(this, this.mAccelerometer, (int)(interval*1000000));
        }
    }

    public void disable() {
        this.mSensorManager.unregisterListener(this);
    }

    @Override
    public void onSensorChanged(final SensorEvent sensorEvent) {
        if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {

            float x = sensorEvent.values[0];
            float y = sensorEvent.values[1];
            final float z = sensorEvent.values[2];

            // needed by VR code
            this.accelerometerValues[0] = x;
            this.accelerometerValues[1] = y;
            this.accelerometerValues[2] = z;

            /*
             * Because the axes are not swapped when the device's screen orientation
             * changes. So we should swap it here. In tablets such as Motorola Xoom,
             * the default orientation is landscape, so should consider this.
             */
            final int orientation = this.mContext.getResources().getConfiguration().orientation;

            if ((orientation == Configuration.ORIENTATION_LANDSCAPE) && (this.mNaturalOrientation != Surface.ROTATION_0)) {
                final float tmp = x;
                x = -y;
                y = tmp;
            } else if ((orientation == Configuration.ORIENTATION_PORTRAIT) && (this.mNaturalOrientation != Surface.ROTATION_0)) {
                final float tmp = x;
                x = y;
                y = -tmp;
            }

            // Invert axes for reverse landscape and reverse portrait
            int rotation =  PenHelper.getActivity().getWindowManager().getDefaultDisplay().getRotation();
            if (rotation == Surface.ROTATION_180 || rotation == Surface.ROTATION_270) {
                x = -x;
                y = -y;
            }

            PenSurfaceView.queueAccelerometer(x,y,z,sensorEvent.timestamp);
        }
        else if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
            // needed by VR code
            this.compassFieldValues[0] = sensorEvent.values[0];
            this.compassFieldValues[1] = sensorEvent.values[1];
            this.compassFieldValues[2] = sensorEvent.values[2];
        }
    }

    @Override
    public void onAccuracyChanged(final Sensor sensor, final int accuracy) {
    }

    public static native void onSensorChanged(final float x, final float y, final float z, final long timestamp);
}