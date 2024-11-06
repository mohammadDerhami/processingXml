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
int inputType(const std::string& data);
const char* info(xmlNode* child);
// std::string select(sqlite3* db);
void processXml(const std::string& xmlData, sqlite3* db);
void processNode(xmlNode* currentNode, std::string& uuid, sqlite3* db,
		 std::string& mainTable);
void findUuid(std::string& uuid, xmlNode* currentNode, std::string& mainTable);
bool hasAttribute(xmlNode* currentNode);
bool isAttributeNode(xmlNode* node);
std::string casting(const xmlChar* s);
bool isObjectNode(xmlNode* node);
bool isElementNode(xmlNode* node);
std::string nodeName(xmlNode* node);
std::string nodeValue(xmlNode* node);
void insertInDb(xmlNode* currentNode, const std::string& uuid, sqlite3* db);
std::vector<std::string> propertyName(xmlNode* currentNode);
std::vector<std::string> propertyValue(xmlNode* currentNode);
// std::string select(sqlite3* db);
#endif
