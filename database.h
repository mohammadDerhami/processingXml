#ifndef DATABASE_H
#define DATABASE_H

#include<sqlite3.h>
#include<string>
#include<iostream>
#include"parseXml.h"

void initDatabase(sqlite3* db);
void insertLibrary(sqlite3* db, const std::string& uuid, const std::string& title);
void insertBooks(sqlite3* db , std::vector<Book>& books , const std::string& uuid);
void insertAddress(sqlite3* db, const std::string& uuid , Address& address);
xmlNodePtr createLibraryNode(xmlDocPtr doc, const Library& library);
xmlDocPtr generateXml(const std::vector<Library>& libraries);
void fillLibrary(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db);
void fillAddress(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db);
void fillBooks(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db);
void fillLibraries(std::vector<Library>& libraries, sqlite3* db);

#endif
