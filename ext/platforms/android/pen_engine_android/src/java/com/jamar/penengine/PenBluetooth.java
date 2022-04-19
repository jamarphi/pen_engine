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

import static android.content.ContentValues.TAG;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class PenBluetooth {

    public static final UUID PEN_UUID = UUID.fromString("01234567-0123-0123-0123-012345678901");

    public static BluetoothDevice connectedDevice = null;
    public static BluetoothAdapter adapter = null;
    public static PenBluetoothService service = null;

    public PenBluetooth(){

    }

    public void manageSocketConnection(BluetoothSocket socket){
        /*Manages the established connection*/
        PenBluetoothService penBluetoothService = new PenBluetoothService();
        penBluetoothService.connect(this, socket);
    }

    @SuppressLint("MissingPermission")
    public void connect(BluetoothDevice targetDevice){
        /*Connect to a given device*/
        Set<BluetoothDevice> pairedDevices = adapter.getBondedDevices();
        BluetoothDevice target = null;
        if (pairedDevices.size() > 0) {
           // There are paired devices. Get the name and address of each paired device.
           for (BluetoothDevice device : pairedDevices) {
               String deviceName = device.getName();
               String deviceHardwareAddress = device.getAddress(); // MAC address
               if(targetDevice.getName() == deviceName){
                    target = device;
                    break;
               }
           }
        }

        if (target == null){
            /*New device to connect to*/
            ConnectThread conn = new ConnectThread(targetDevice);
            conn.run();
        }
    }

    @SuppressLint("MissingPermission")
    public void connect(String targetDeviceName){
        /*Connect to a given device*/
        Set<BluetoothDevice> pairedDevices = adapter.getBondedDevices();
        BluetoothDevice target = null;
        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
                if(targetDeviceName == deviceName){
                    target = device;
                    break;
                }
            }
        }

        if (target != null){
            /*New device to connect to*/
            ConnectThread conn = new ConnectThread(target);
            conn.run(this);
        }
    }

    private class AcceptThread extends Thread {
       /*Thread for establishing connection as a server*/
       private final BluetoothServerSocket mmServerSocket;

       @SuppressLint("MissingPermission")
       public AcceptThread() {
           // Use a temporary object that is later assigned to mmServerSocket
           // because mmServerSocket is final.
           BluetoothServerSocket tmp = null;
           try {
               // PEN_UUID is the pen_engine_android's UUID string, also used by the client code.
               tmp = adapter.listenUsingRfcommWithServiceRecord("PEN_ENGINE", PEN_UUID);
           } catch (IOException e) {
               Log.e(TAG, "Socket's listen() method failed", e);
           }
           mmServerSocket = tmp;
       }

       public void run(PenBluetooth penBluetooth) {
           BluetoothSocket socket = null;
           // Keep listening until exception occurs or a socket is returned.
           while (true) {
               try {
                   socket = mmServerSocket.accept();
               } catch (IOException e) {
                   Log.e(TAG, "Socket's accept() method failed", e);
                   break;
               }

               if (socket != null) {
                   // A connection was accepted. Perform work associated with
                   // the connection in a separate thread.
                   penBluetooth.manageSocketConnection(socket);
                   try {
                       mmServerSocket.close();
                   } catch (IOException e) {
                       e.printStackTrace();
                   }
                   break;
               }
           }
       }

       // Closes the connect socket and causes the thread to finish.
       public void close() {
           try {
               mmServerSocket.close();
           } catch (IOException e) {
               Log.e(TAG, "Could not close the connect socket", e);
           }
       }
    }

	private class ConnectThread extends Thread {
        /*Thread for establishing connection as a client*/
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        @SuppressLint("MissingPermission")
        public ConnectThread(BluetoothDevice device) {
            // Use a temporary object that is later assigned to mmSocket
            // because mmSocket is final.
            BluetoothSocket tmp = null;
            mmDevice = device;

            try {
                // Get a BluetoothSocket to connect with the given BluetoothDevice.
                // PEN_UUID is the pen_engine_android's UUID string, also used in the server code.
                tmp = device.createRfcommSocketToServiceRecord(PEN_UUID);
            } catch (IOException e) {
                Log.e(TAG, "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        @SuppressLint("MissingPermission")
        public void run(PenBluetooth penBluetooth) {
            // Cancel discovery because it otherwise slows down the connection.
            adapter.cancelDiscovery();

            try {
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                mmSocket.connect();
            } catch (IOException connectException) {
                // Unable to connect; close the socket and return.
                try {
                    mmSocket.close();
                } catch (IOException closeException) {
                    Log.e(TAG, "Could not close the client socket", closeException);
                }
                return;
            }

            // The connection attempt succeeded. Perform work associated with
            // the connection in a separate thread.
            penBluetooth.manageSocketConnection(mmSocket);
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the client socket", e);
            }
        }
    }

    public class PenBluetoothService {
       private static final String TAG = "PEN_BLUETOOTH_SERVICE";
       public ConnectedThread conn = null;

       @SuppressLint("MissingPermission")
       public void connect(PenBluetooth penBluetooth, BluetoothSocket socket) {
            if(penBluetooth.service.conn != null){
                penBluetooth.service.conn.close();
                penBluetooth.service.conn = null;
            }

            if(socket == null){
                try {
                    conn = new ConnectedThread(penBluetooth.connectedDevice.createRfcommSocketToServiceRecord(PEN_UUID));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }else {
                conn = new ConnectedThread(socket);
            }
       }

       public class ConnectedThread extends Thread {
           private final BluetoothSocket mmSocket;
           private final InputStream mmInStream;
           private final OutputStream mmOutStream;
           private byte[] mmBuffer; // mmBuffer store for the stream
           public int numBytes = 0;
           public byte[] writeBuffer;
           public int writeBufferNumBytes = 0;

           public ConnectedThread(BluetoothSocket socket) {
               mmSocket = socket;
               InputStream tmpIn = null;
               OutputStream tmpOut = null;

               // Get the input and output streams; using temp objects because
               // member streams are final.
               try {
                   tmpIn = socket.getInputStream();
               } catch (IOException e) {
                   Log.e(TAG, "Error occurred when creating input stream", e);
               }
               try {
                   tmpOut = socket.getOutputStream();
               } catch (IOException e) {
                   Log.e(TAG, "Error occurred when creating output stream", e);
               }

               mmInStream = tmpIn;
               mmOutStream = tmpOut;
           }

           public byte[] read() {
               /*Read data from a connected device*/
               mmBuffer = new byte[1024];

               // Keep listening to the InputStream until an exception occurs.
               while (true) {
                   try {
                       numBytes = mmInStream.read(mmBuffer);
                       
                   } catch (IOException e) {
                       Log.d(TAG, "Input stream was disconnected", e);
                       break;
                   }
               }
               return mmBuffer;
           }

           public void write() {
               /*Write data to a connected device*/
               try {
                   mmOutStream.write(writeBuffer);
               } catch (IOException e) {
                   Log.e(TAG, "Error occurred when sending data", e);
               }
           }

           // Call this method from the main activity to shut down the connection.
           public void close() {
               try {
                   mmSocket.close();
               } catch (IOException e) {
                   Log.e(TAG, "Could not close the connect socket", e);
               }
           }
       }
    }
}