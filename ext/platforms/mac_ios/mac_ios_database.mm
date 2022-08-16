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

#include "mac_ios_database.h"

#ifdef __PEN_MAC_IOS__

static PenMacIOSDatabase* instance;
static sqlite3* database;
static sqlite3_stmt* statement;

@implementation PenMacIOSDatabase
- (void) StartDatabase: (NSString*) databaseName :(NSString*) tableName {
    /*Creates a database*/
    NSString* documentDirectory;
    NSArray* directoryPaths;
    directoryPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    documentDirectory = [directoryPaths objectAtIndex:0];
    self.databasePath = [[NSString alloc] initWithString:[documentDirectory stringByAppendingPathComponent:databaseName]];
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    if([fileManager fileExistsAtPath:self.databasePath] == NO){
        const char* dbPath = [self.databasePath UTF8String];
        if(sqlite3_open(dbPath, &database) == SQLITE_OK){
            char* error;
            NSString* tableStatement = [NSString stringWithFormat:@"create table if not exists %@ (key text primary key, value text)", tableName];
            const char* tableStatementStr = [tableStatement UTF8String];
            if(sqlite3_exec(database, tableStatementStr, nil, nil, &error) != SQLITE_OK){
                NSLog(@"Failed to create table in database");
            }else{
                sqlite3_close(database);
            }
        }else{
            NSLog(@"Failed to open database");
        }
    }
}

- (void) CreateTable: (NSString*) table{
    /*Creates a table in the database if it does not exist already*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* tableStatement = [NSString stringWithFormat:@"create table if not exists %@ (key text primary key, value text)", table];
        const char* tableStatementStr = [tableStatement UTF8String];
        sqlite3_prepare_v2(database, tableStatementStr, -1, &statement, nil);
    }
}

- (void) Insert: (NSString*) table :(NSString*) key :(NSString*) value {
    /*Inserts a key value pair into the given table*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* insertStatement = [NSString stringWithFormat:@"insert into %@ (key,value) values (\"%@\",\"%@\")", table, key, value];
        const char* insertStatementStr = [insertStatement UTF8String];
        sqlite3_prepare_v2(database, insertStatementStr, -1, &statement, nil);
    }
}

- (char*) GetItem: (NSString*) table :(NSString*) key {
    /*Retrieves an item based on the key*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* readStatement = [NSString stringWithFormat:@"select value from %@ where key=\"%@\"", table, key];
        const char* readStatementStr = [readStatement UTF8String];
        sqlite3_prepare_v2(database, readStatementStr, -1, &statement, nil);
        if(sqlite3_step(statement) == SQLITE_ROW){
            char* value = (char*) sqlite3_column_text(statement, 0);
            return value;
        }
        return nil;
    }
    return nil;
}

- (void) UpdateItem: (NSString*) table :(NSString*) key :(NSString*) value {
    /*Updates an item based on the key*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* updateStatement = [NSString stringWithFormat:@"update %@ set value = \"%@\" where key=\"%@\"", table, value, key];
        const char* updateStatementStr = [updateStatement UTF8String];
        sqlite3_prepare_v2(database, updateStatementStr, -1, &statement, nil);
    }
}

- (void) DeleteItem: (NSString*) table :(NSString*) key {
    /*Deletes item based on the key*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* deleteStatement = [NSString stringWithFormat:@"delete from %@ where key = \"%@\"", table, key];
        const char* deleteStatementStr = [deleteStatement UTF8String];
        sqlite3_prepare_v2(database, deleteStatementStr, -1, &statement, nil);
    }
}

- (void) ClearTable: (NSString*) table {
    /*Deletes everything from a table*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* deleteStatement = [NSString stringWithFormat:@"delete from %@", table];
        const char* deleteStatementStr = [deleteStatement UTF8String];
        sqlite3_prepare_v2(database, deleteStatementStr, -1, &statement, nil);
    }
}

- (void) DropTable: (NSString*) table {
    /*Deletes a table*/
    const char* dbPath = [self.databasePath UTF8String];
    if(sqlite3_open(dbPath, &database) == SQLITE_OK){
        NSString* dropStatement = [NSString stringWithFormat:@"drop table %@", table];
        const char* dropStatementStr = [dropStatement UTF8String];
        sqlite3_prepare_v2(database, dropStatementStr, -1, &statement, nil);
    }
}

+(PenMacIOSDatabase*) Get{
    /*Returns an instance of PenMacIOSDatabase*/
    if (!instance)
        instance = [[PenMacIOSDatabase alloc] init];
    return instance;
}
@end

void MapMacPenMacIOSDatabaseStart(const char* database, const char* table){
    /*Creates a database with the given name*/
    [[PenMacIOSDatabase Get] StartDatabase:[NSString stringWithUTF8String:database] :[NSString stringWithUTF8String:table]];
}

void MapMacPenMacIOSDatabaseCreateTable(const char* table){
    /*Creates a table in the database*/
    [[PenMacIOSDatabase Get] CreateTable:[NSString stringWithUTF8String:table]];
}

void MapMacPenMacIOSDatabaseInsert(const char* table, const char* key, const char* value){
    /*Inserts a key value pair into the given table*/
    [[PenMacIOSDatabase Get] Insert:[NSString stringWithUTF8String:table] :[NSString stringWithUTF8String:key] :[NSString stringWithUTF8String:value]];
}

char* MapMacPenMacIOSDatabaseGet(const char* table, const char* key){
    /*Retrieves an item from the table based on the given key*/
    return [[PenMacIOSDatabase Get] GetItem:[NSString stringWithUTF8String:table] :[NSString stringWithUTF8String:key]];
}

void MapMacPenMacIOSDatabaseUpdate(const char* table, const char* key, const char* value){
    /*Updates an item based on the key*/
    [[PenMacIOSDatabase Get] UpdateItem:[NSString stringWithUTF8String:table] :[NSString stringWithUTF8String:key] :[NSString stringWithUTF8String:value]];
}

void MapMacPenMacIOSDatabaseDelete(const char* table, const char* key){
    /*Deletes item based on the key*/
    [[PenMacIOSDatabase Get] DeleteItem:[NSString stringWithUTF8String:table] :[NSString stringWithUTF8String:key]];
}

void MapMacPenMacIOSDatabaseClear(const char* table){
    /*Deletes everything from a table*/
    [[PenMacIOSDatabase Get] ClearTable:[NSString stringWithUTF8String:table]];
}

void MapMacPenMacIOSDatabaseDrop(const char* table){
    /*Deletes a table*/
    [[PenMacIOSDatabase Get] DropTable:[NSString stringWithUTF8String:table]];
}

#endif
