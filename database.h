#ifndef DATABASE_H
#define DATABASE_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
void openDb(sqlite3* db);
bool existTable(const std::string& name, sqlite3* db);
void createTable(const std::string& name, const std::vector<std::string>& properties, sqlite3* db,
		 bool mainTable, const std::string& mainTableName);
bool existUuid(sqlite3* db, const std::string& uuid);
void update(sqlite3* db, const std::string& uuid,
	    const std::vector<std::string>& propertyName, const std::vector<std::string>& propertyValue,
	    const std::string& tableName);
void insert(sqlite3* db, const std::string& uuid,
	    const std::vector<std::string>& propertyName, const std::vector<std::string>& propertyValue,
	    const std::string& tableName);
/*xmlNodePtr createLibraryNode(xmlDocPtr doc, const Library& library);
xmlDocPtr generateXml(const std::vector<Library>& libraries);
void fillLibrary(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db);
void fillAddress(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db);
void fillBooks(std::unordered_map<std::string, Library>& libraryMap,
	       sqlite3* db);
void fillLibraries(std::vector<Library>& libraries, sqlite3* db);
*/
#endif
