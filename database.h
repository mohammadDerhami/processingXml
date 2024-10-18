#ifndef DATABASE_H
#define DATABASE_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <sqlite3.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include<cstring>
class Address {
       public:
	std::string street;
	std::string city;
	std::string zip;
	std::string uuid;
};
class Book {
       public:
	int id = 0;
	std::string author;
	int publication_year = 0;
	std::string title;
	std::string uuid;
};
class Library {
       public:
	std::string uuid;
	std::string title;
	Address address;
	std::vector<Book> books;
};

void initDatabase(sqlite3* db);
void insertLibrary(sqlite3* db, const std::string& uuid,
		   const std::string& title);
void insertBooks(sqlite3* db, std::vector<Book>& books,
		 const std::string& uuid);
void insertAddress(sqlite3* db, const std::string& uuid, Address& address);
xmlNodePtr createLibraryNode(xmlDocPtr doc, const Library& library);
xmlDocPtr generateXml(const std::vector<Library>& libraries);
void fillLibrary(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db);
void fillAddress(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db);
void fillBooks(std::unordered_map<std::string, Library>& libraryMap,
	       sqlite3* db);
void fillLibraries(std::vector<Library>& libraries, sqlite3* db);

#endif
