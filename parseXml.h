#ifndef PARSEXML_H
#define PARSEXML_H

#include<iostream>
#include<vector>
#include<cstring>
#include<libxml/parser.h>
#include<libxml/tree.h>

class Address
{
	public:
	std::string street;
	std::string city;
	std::string zip;
	std::string uuid;
}
class Book
{
	public:
	int id;
	std::string author;
	int publication_year;
	std::string title;
	std::string uuid;
}
class Library
{
	public:
	std::string uuid;
	std::string title;
}
void processXml(std::string xmlData , sqlite3* db);
int inputType(const std::string& data);
const char * info(xmlNode* child);
void fillAddressInfo(Address& address , xmlNode* addressNode);
void fillBookInfo(Book& book , xmlNode* bookNode);
void fillLibraryInfo(Library& library , xmlNode* libraryNode);









#endif