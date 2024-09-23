#include"parseXml.h"
void processXml(const std::string& xmlData , sqlite3* db)
{
	xmlDocPtr doc = xmlReadMemory(xmlData.c_str(),xmlData.length() , nullptr , nullptr , 0);

	if(doc == nullptr)
	{
		std::cerr<<"failed to parse xml "<<std::endl;
		return;
	}

	xmlNode* root = xmlDocGetRootElement(doc);


	processNode(root , db);

	xmlFreeDoc(doc);
	xmlCleanupParser();	
}
void processNode(xmlNode* currentNode , sqlite3* db)
{
	Address address;
	std::string uuid;
	std::vector<Book> books;
	std::string libraryTitle;

	while(currentNode)
	{
		if(currentNode -> type == XML_ELEMENT_NODE)
		{
			if(strcmp((const char *)currentNode->name , "uuid") == 0)
			{
				uuid = info(currentNode);
			}	
			else if( strcmp((const char *)currentNode->name , "title") == 0)
			{

				if(currentNode->parent && strcmp((const char *)currentNode -> parent -> name , "library") == 0)
				{
					libraryTitle = info(currentNode);
				}

			}else if(strcmp((const char *)currentNode->name , "city")==0)
			{
				address.city = info(currentNode);
			}else if(strcmp((const char *)currentNode->name , "street")==0)
			{
				address.street = info(currentNode);
			}else if(strcmp((const char *)currentNode->name , "zip")==0)
			{
				address.zip = info(currentNode);
			}else if(strcmp((const char *)currentNode -> name , "book") == 0)
			{
				Book book;
				fillBookInfo(book ,currentNode);
				books.push_back(book);
			}

				
		}
		processNode(currentNode -> children , db);
		currentNode = currentNode -> next;

	}
	if (!libraryTitle.empty()) {

		insertLibrary(db, uuid, libraryTitle);

	}
	insertAddress(db , uuid ,address);
	insertBooks(db , books , uuid);
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
		if(node->type  == XML_ELEMENT_NODE &&xmlStrcmp(node->name , BAD_CAST"operation") == 0)
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

	if(strcmp(operationType.c_str() , "select"  ) == 0 )
	{
		return 1;
	}else
	{
	
		return 0;
	}


}
