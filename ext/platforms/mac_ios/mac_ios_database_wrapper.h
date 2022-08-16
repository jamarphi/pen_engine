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
#pragma once

#include "../../../src/state/config.h"
#include "../../../src/state/state.h"
#include "mac_ios_cpp_objective_c_mapping.h"

namespace pen {
    namespace ios {
        namespace db {
            static void Start(const char* database, const char* table){
                /*Starts a database with the given name*/
#ifndef __PEN_MAC_IOS__
                MapMacPenMacIOSDatabaseStart(databaseName, tableName);
#endif
            }
        
            static void CreateTable(const char* table){
                /*Creates a table in the database*/
#ifndef __PEN_MAC_IOS__
                MapMacPenMacIOSDatabaseCreateTable(table);
#endif
            }
        
            static void Insert(const char* table, const char* key, const char* value){
                /*Inserts a key value pair into the table*/
#ifndef __PEN_MAC_IOS__
                MapMacPenMacIOSDatabaseInsert(table, key, value);
#endif
            }
        
            static char* Get(const char* table, const char* key){
                /*Retrieves an item from the table based on the key*/
#ifndef __PEN_MAC_IOS__
                return MapMacPenMacIOSDatabaseGet(table, key);
#endif
            }
        
            static void Update(const char* table, const char* key, const char* value){
                /*Updates an item based on the key*/
#ifndef __PEN_MAC_IOS__
                return MapMacPenMacIOSDatabaseUpdate(table, key, value);
#endif
            }
            
            static void Delete(const char* table, const char* key){
                /*Deletes item based on the key*/
#ifndef __PEN_MAC_IOS__
                return MapMacPenMacIOSDatabaseDelete(table, key);
#endif
            }
            
            static void Clear(const char* table){
                /*Deletes everything from a table*/
#ifndef __PEN_MAC_IOS__
                return MapMacPenMacIOSDatabaseClear(table);
#endif
            }
            
            static void Drop(const char* table){
                /*Deletes a table*/
#ifndef __PEN_MAC_IOS__
                return MapMacPenMacIOSDatabaseDrop(table);
#endif
            }
        }
    }
}

