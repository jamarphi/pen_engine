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

    private static String DATABASE_NAME = "pen.sqlite";
    private static String INITIAL_TABLE_NAME = "data";
    private static final int DATABASE_VERSION = 1;  
    private static DBOpenHelper mDatabaseOpenHelper = null;
    private static SQLiteDatabase mDatabase = null;

    public static void init(String dbName, String tableName) {
        if (MainActivity.getContext() != null) {
            DATABASE_NAME = dbName;
            TABLE_NAME = tableName;
            mDatabaseOpenHelper = new DBOpenHelper(MainActivity.getContext());
            mDatabase = mDatabaseOpenHelper.getWritableDatabase();
        }
    }
    
    public static void destroy() {
        if (mDatabase != null) {
            mDatabase.close();
        }
    }
    
    public static void createTable(String table) {
        try {
            String sql = "create table if not exists " + table + "(key text primary key, value text);";
            mDatabase.execSQL(sql, new Object[] { key, value });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void setItem(String table, String key, String value) {
        try {
            String sql = "replace into " + table + "(key,value) values (?,?)";
            mDatabase.execSQL(sql, new Object[] { key, value });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static String getItem(String table, String key) {
        String ret = null;
        try {
        String sql = "select value from " + table + " where key=?";
        Cursor c = mDatabase.rawQuery(sql, new String[]{key});  
        while (c.moveToNext()) {
            if (ret != null) 
            {
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
    
    public static void removeItem(String table, String key) {
        try {
            String sql = "delete from " + table + " where key=?";
            mDatabase.execSQL(sql, new Object[] {key});
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void clearTable(String table) {
        try {
            String sql = "delete from " + table;
            mDatabase.execSQL(sql);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void dropTable(String table) {
        try {
            String sql = "drop table " + table;
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
            db.execSQL("create table if not exists " + INITIAL_TABLE_NAME + "(key integer primary key, value text);");
        }
        
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {}
    }
}
