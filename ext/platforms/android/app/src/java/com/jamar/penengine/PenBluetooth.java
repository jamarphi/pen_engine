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

public class PenBluetooth {

    public final String PEN_UUID = "01234567-0123-0123-0123-012345678901"

    public static BluetoothDevice connectedDevice = null;
    
    public static manageSocketConnection(BluetoothSocket socket){
        /*Manages the established connection*/

    }

    public static connect(BluetoothDevice device){
        /*Connect to a given device*/
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        BluetoothDevice target = null;
        if (pairedDevices.size() > 0) {
           // There are paired devices. Get the name and address of each paired device.
           for (BluetoothDevice device : pairedDevices) {
               String deviceName = device.getName();
               String deviceHardwareAddress = device.getAddress(); // MAC address
               if(device.getName() == deviceName){
                    target = device;
                    break;
               }
           }
        }

        if (target == null){
            /*New device to connect to*/
            ConnectThread conn = new ConnectThread(device);
            conn.run();
        }
    }

    public static connect(String deviceNameStr){
        /*Connect to a given device*/
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        BluetoothDevice target = null;
        if (pairedDevices.size() > 0) {
           // There are paired devices. Get the name and address of each paired device.
           for (BluetoothDevice device : pairedDevices) {
               String deviceName = device.getName();
               String deviceHardwareAddress = device.getAddress(); // MAC address
               if(deviceNameStr == deviceName){
                    target = device;
                    break;
               }
           }
        }

        if (target != null){
            /*New device to connect to*/
            ConnectThread conn = new ConnectThread(target);
            conn.run();
        }
    }

    private class AcceptThread extends Thread {
       /*Thread for establishing connection as a server*/
       private final BluetoothServerSocket mmServerSocket;

       public AcceptThread() {
           // Use a temporary object that is later assigned to mmServerSocket
           // because mmServerSocket is final.
           BluetoothServerSocket tmp = null;
           try {
               // PEN_UUID is the app's UUID string, also used by the client code.
               tmp = bluetoothAdapter.listenUsingRfcommWithServiceRecord(NAME, PEN_UUID);
           } catch (IOException e) {
               Log.e(TAG, "Socket's listen() method failed", e);
           }
           mmServerSocket = tmp;
       }

       public void run() {
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
                   PenBluetooth.manageSocketConnection(socket);
                   mmServerSocket.close();
                   break;
               }
           }
       }

       // Closes the connect socket and causes the thread to finish.
       public void cancel() {
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

        public ConnectThread(BluetoothDevice device) {
            // Use a temporary object that is later assigned to mmSocket
            // because mmSocket is final.
            BluetoothSocket tmp = null;
            mmDevice = device;

            try {
                // Get a BluetoothSocket to connect with the given BluetoothDevice.
                // PEN_UUID is the app's UUID string, also used in the server code.
                tmp = device.createRfcommSocketToServiceRecord(PEN_UUID);
            } catch (IOException e) {
                Log.e(TAG, "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            // Cancel discovery because it otherwise slows down the connection.
            bluetoothAdapter.cancelDiscovery();

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
            PenBluetooth.manageSocketConnection(mmSocket);
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
       private Handler handler; // handler that gets info from Bluetooth service
       public static ConnectedThread conn = null;

       // Defines several constants used when transmitting messages between the
       // service and the UI.
       private interface MessageConstants {
           public static final int MESSAGE_READ = 0;
           public static final int MESSAGE_WRITE = 1;
           public static final int MESSAGE_TOAST = 2;
       }

       PenBluetoothService(BluetoothSocket socket) {
            if(socket == null){
                conn = new ConnectedThread(PenBluetooth.connectedDevice.createRfcommSocketToServiceRecord(PEN_UUID));
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
           }

           public void write() {
               /*Write data to a connected device*/
               try {
                   mmOutStream.write(writeBuffer);

                   // Share the sent message with the UI activity.
                   Message writtenMsg = handler.obtainMessage(
                           MessageConstants.MESSAGE_WRITE, -1, -1, mmBuffer);
                   writtenMsg.sendToTarget();
               } catch (IOException e) {
                   Log.e(TAG, "Error occurred when sending data", e);

                   // Send a failure message back to the activity.
                   Message writeErrorMsg =
                           handler.obtainMessage(MessageConstants.MESSAGE_TOAST);
                   Bundle bundle = new Bundle();
                   bundle.putString("toast",
                           "Couldn't send data to the other device");
                   writeErrorMsg.setData(bundle);
                   handler.sendMessage(writeErrorMsg);
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