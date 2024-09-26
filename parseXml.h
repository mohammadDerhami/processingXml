#ifndef PARSEXML_H
#define PARSEXML_H

#include<iostream>
#include<vector>
#include<cstring>
#include<libxml/parser.h>
#include<libxml/tree.h>
#include <unordered_map>
#include <libxml/xmlmemory.h>

class Address
{
	public:
	std::string street;
	std::string city;
	std::string zip;
	std::string uuid;
};
class Book
{
	public:
	int id = 0;
	std::string author;
	int publication_year = 0;
	std::string title;
	std::string uuid;
};
class Library
{
	public:
	std::string uuid;
	std::string title;
	Address address;
	std::vector<Book> books;
};
void processXml(const std::string& xmlData, Address& address , std::string& uuid , std::vector<Book>& books , std::string& libraryTitle );
int inputType(const std::string& data);
const char * info(xmlNode* child);
void fillBookInfo(Book& book , xmlNode* bookNode);
void processNode(xmlNode* currentNode , Address& address , std::string& uuid , std::vector<Book>& books , std::string& libraryTitle);
void findUuid(std::string& uuid , xmlNode* currentNode);






#endif
