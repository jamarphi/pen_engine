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
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;


public class PenLocalStorage {

    private static final String TAG = "PenLocalStorage";

    private static String DATABASE_NAME = "jsb.sqlite";
    private static String TABLE_NAME = "data";
    private static final int DATABASE_VERSION = 1;
    
    private static DBOpenHelper mDatabaseOpenHelper = null;
    private static SQLiteDatabase mDatabase = null;
    /**
     * Constructor
     * @param context The Context within which to work, used to create the DB
     * @return 
     */
    public static boolean init(String dbName, String tableName) {
        if (MainActivity.getContext() != null) {
            DATABASE_NAME = dbName;
            TABLE_NAME = tableName;
            mDatabaseOpenHelper = new DBOpenHelper(MainActivity.getContext());
            mDatabase = mDatabaseOpenHelper.getWritableDatabase();
            return true;
        }
        return false;
    }
    
    public static void destroy() {
        if (mDatabase != null) {
            mDatabase.close();
        }
    }
    
    public static void setItem(String key, String value) {
        try {
            String sql = "replace into "+TABLE_NAME+"(key,value)values(?,?)";
            mDatabase.execSQL(sql, new Object[] { key, value });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static String getItem(String key) {
        String ret = null;
        try {
        String sql = "select value from "+TABLE_NAME+" where key=?";
        Cursor c = mDatabase.rawQuery(sql, new String[]{key});  
        while (c.moveToNext()) {
            // only return the first value
            if (ret != null) 
            {
                Log.e(TAG, "The key contains more than one value.");
                break;
            }
            ret = c.getString(c.getColumnIndex("value"));  
        }  
        c.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }
    
    public static void removeItem(String key) {
        try {
            String sql = "delete from "+TABLE_NAME+" where key=?";
            mDatabase.execSQL(sql, new Object[] {key});
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void clear() {
        try {
            String sql = "delete from "+TABLE_NAME;
            mDatabase.execSQL(sql);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    private static class DBOpenHelper extends SQLiteOpenHelper {

        DBOpenHelper(Context context) {
            super(context, DATABASE_NAME, null, DATABASE_VERSION);
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            db.execSQL("CREATE TABLE IF NOT EXISTS "+TABLE_NAME+"(key TEXT PRIMARY KEY,value TEXT);");
        }
        
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.w(TAG, "Upgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");
        }
    }
}