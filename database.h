#ifndef DATABASE_H
#define DATABASE_H

#include<sqlite3.h>
#include<string>
#include<iostream>
#include"parseXml.h"

void initDatabase(sqlite3* db);
void insertLibrary(sqlite3* db, const std::string& uuid, const std::string& title);
void insertBooks(sqlite3* db , std::vector<Book>& books , std::string& uuid);
void insertAddress(sqlite3* db, const std::string& uuid , Address& address);
#endif
