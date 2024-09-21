#include "parseXml.h"
void processXml(const std::string& xmlData , sqlite3* db)
{
	xmlDocPtr doc = xmlReadMemory(xmlData.c_str(),xmlData.length() , nullptr , nullptr , 0);

	if(doc == null)
	{
		std::cerr<<"failed to parse xml "<<std::endl;
		return;
	}

	xmlNode* root = xmlDocGetRootElement(doc);
	searchLibrary(root);
	searchBooks(root);
	searchAddress(root);

	xmlFreeDoc(doc);
	xmlCleanupParser();	
}
void processNode(xmlNode* currentNode)
{
	while(currentNode)
	{
		if(currentNode -> type == XML_ELEMENT_NODE && strcmp((const char *)currentNode->name , "library") == 0)
		{
			Library library;
			fillLibraryInfor(library, currentNode);
			//add library to db
		}else if(currentNode -> type == XML_ELEMENT_NODE && strcmp((const char *)currentNode->name , "book") == 0)
		{
			Book book;
			fillBookInfo(book , currentNode);
			//add book to db
		}else if(currentNode -> type == XML_ELEMENT_NODE && strcmp((const char *)currentNode->name , "address") == 0)
		{
			Address address;
			fillAddressInfo(address, currentNode);
			//add address to db
		}

		processNode(currentNode -> children);
		currentNode = currentNode -> next;
	}
}
void fillLibraryInfo(Library& library , xmlNode* libraryNode)
{
	for(xmlNode* libraryChild = libraryNode->children ; libraryChild ; libraryChild = libraryChild -> next)
	{
		if(strcmp((const char *)libraryChild->name , "uuid") == 0 )
		{
			library.uuid = info(libraryChild);
		}else if(strcmp((const char *)libraryChild->name , "title") == 0)
		{
			library.title = info(libraryChild);
		}
	}
}
void fillBookInfo(Book& book , xmlNode* bookNode)
{
	for(xmlNode* bookChild = bookNode->children ; bookChild ; bookChild = bookChild ->next)
	{
		if(strcmp((const char *)bookChild->name , "id" ) ==0 )
		{
			book.id = std::stoi(info(bookChild));
		}else if(strcmp((const char *)bookChild->name , "title") == 0)
		{
			book.title = info(bookChild);
		}else if(strcmp((const char *)bookChild->name , "author") == 0)
		{
			book.author = info(bookChild);
		}else if(strcmp((const char *)bookChild->name , "pulication_year") == 0)
		{
			book.publication_year = std::stoi(info(bookChild));
		}
	}


}
void fillAddressInfo(Address& address , xmlNode* addressNode)
{
	for(xmlNode* addressChild = addressNode -> children ; addressChild ; addressChild -> next)
	{
		if(strcmp((const char *)addressChild->name , "street") == 0)
		{
			address.street = info(addressChild);
		}else if(strcmp((const char *)addressChild->name , "city") == 0)
		{
			address.city = info(addressChild);
		}else if(strcmp((const char *)addressChild->name , "zip") == 0)
		{
			address.zip = info(addressChild);
		}
	}
}
const char * info(xmlNode* child)
{
	return (const char *)xmlNodeGetContent(child);
}	
int inputType(const std::string& xmlData)
{

	xmlDocPtr doc = xmlReadMemory(xmlData.c_str() , xmlData.length() , nullptr , nullptr , 0);
	xmlNode* root = xmlDocGetRootElement(doc);

	if(root == nullptr)
	{
		std::cerr<<"empty xml document "<<std::endl;
		xmlFreeDoc(doc);
		return -1;
	}

	std::string operationType;

	for(xmlNode* node = root->children ; node ; node = node -> next)
	{
		if(node->type  == XML_ELEMENT_NODE &&xmlstrcmp(node->name , BAD_CAST"operation") == 0)
		{
			xmlChar* type = xmlGetProp(node , BAD_CAST"type");
			if(type != nullptr)
			{
				operationType = reinterpret_cast<const char*>(type);
				xmlFree(type);
			}
			break;
		}
	}
	xmlFreeDoc(doc);

	if(strcmp(operationType.c_str() , "select" == 0 )
	{
		return 1;
	}
	else
		return 0;



}
