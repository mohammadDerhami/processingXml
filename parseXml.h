#ifndef PARSEXML_H
#define PARSEXML_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "database.h"
void processXml(const std::string& xmlData, Address& address, std::string& uuid,
		std::vector<Book>& books, std::string& libraryTitle);
int inputType(const std::string& data);
const char* info(xmlNode* child);
void fillBookInfo(Book& book, xmlNode* bookNode);
void processNode(xmlNode* currentNode, Address& address, std::string& uuid,
		 std::vector<Book>& books, std::string& libraryTitle);
void findUuid(std::string& uuid, xmlNode* currentNode);
void process(const std::string xmlData, sqlite3* db);
std::string select(sqlite3* db);

#endif
