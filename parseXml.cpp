#include "parseXml.h"
void process(const std::string xmlData, sqlite3* db) {
	std::string uuid;
	std::vector<Book> books;
	Address address;
	std::string libraryTitle;

	// pass attributes to processXml function
	processXml(xmlData, address, uuid, books, libraryTitle);
	// if input doesnt have uuid
	if (uuid.empty()) {
		std::cout << "Wrong input (uuid is null) " << std::endl;
	} else {
		insertLibrary(db, uuid, libraryTitle);

		if (!books.empty()) {
			insertBooks(db, books, uuid);
			std::cout << "add book" << std::endl;
		}
		if (!address.city.empty() || !address.street.empty() ||
		    !address.zip.empty()) {
			insertAddress(db, uuid, address);
		}
	}
}
std::string select(sqlite3* db) {
	std::vector<Library> libraries;
	fillLibraries(libraries, db);
	// db is empty
	if (libraries.empty()) {
		return "no library ";
	} else {
		xmlChar* xmlBuff;
		int buffersize;

		xmlDocPtr xmlDoc = generateXml(libraries);
		xmlDocDumpFormatMemory(xmlDoc, &xmlBuff, &buffersize, 1);
		// convert xmlChar* -> std::string
		std::string xmlString =
		    std::string(reinterpret_cast<const char*>(xmlBuff));
		xmlFree(xmlBuff);
		xmlFreeDoc(xmlDoc);
		xmlCleanupParser();
		return xmlString;
	}
}

// Pass the uuid and xmlData(string) to processNode function
void processXml(const std::string& xmlData, Address& address, std::string& uuid,
		std::vector<Book>& books, std::string& libraryTitle) {
	xmlDocPtr doc = xmlReadMemory(xmlData.c_str(), xmlData.length(),
				      nullptr, nullptr, 0);
	// Check if the XML parsing failed
	if (doc == nullptr) {
		std::cerr << "failed to parse xml " << std::endl;
		return;
	}

	xmlNode* root = xmlDocGetRootElement(doc);
	findUuid(uuid, root);
	// pass atributes to processNode function , this function process nodes
	// extract information
	processNode(root, address, uuid, books, libraryTitle);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}
// Finds UUID in XML nodes
void findUuid(std::string& uuid, xmlNode* currentNode) {
	while (currentNode) {
		if (currentNode->type == XML_ELEMENT_NODE) {
			if (strcmp((const char*)currentNode->name, "uuid") ==
			    0) {
				uuid = info(currentNode);
				return;
			}
		}
		// If the current node has children, recursively search in the
		// children
		if (currentNode->children) {
			findUuid(uuid, currentNode->children);

			if (!uuid.empty()) {
				return;	 // Exit if UUID is found in children
			}
		}

		currentNode = currentNode->next;
	}
}
// process nodes recursive
void processNode(xmlNode* currentNode, Address& address, std::string& uuid,
		 std::vector<Book>& books, std::string& libraryTitle) {
	while (currentNode) {
		// Check if the current node is an element node
		if (currentNode->type == XML_ELEMENT_NODE) {
			// If the node name is "title" and its parent is
			// "library", set the library title
			if (strcmp((const char*)currentNode->name, "title") ==
			    0) {
				if (currentNode->parent &&
				    strcmp(
					(const char*)currentNode->parent->name,
					"library") == 0) {
					libraryTitle = info(currentNode);
				}
			}
			// Fill address information based on the node names

			else if (strcmp((const char*)currentNode->name,
					"city") == 0) {
				address.city = info(currentNode);
			} else if (strcmp((const char*)currentNode->name,
					  "street") == 0) {
				address.street = info(currentNode);
			} else if (strcmp((const char*)currentNode->name,
					  "zip") == 0) {
				address.zip = info(currentNode);
			}
			// If the node name is "book", extract book information
			else if (strcmp((const char*)currentNode->name,
					"book") == 0) {
				Book book;
				fillBookInfo(book,
					     currentNode);  // Fill book info
							    // from XML node
				book.uuid = uuid;
				// Check if book has a id
				if (book.id == 0) {
					std::cout
					    << "Wrong input (book without id) "
					    << std::endl;
				} else {
					books.push_back(book);	// Add the book
								// to the vector
				}
			}
		}
		processNode(currentNode->children, address, uuid, books,
			    libraryTitle);
		currentNode = currentNode->next;
	}
}

// Fill book information from the XML node
void fillBookInfo(Book& book, xmlNode* bookNode) {
	for (xmlNode* bookChild = bookNode->children; bookChild;
	     bookChild = bookChild->next) {
		if (strcmp((const char*)bookChild->name, "id") == 0) {
			book.id = std::stoi(info(bookChild));
		} else if (strcmp((const char*)bookChild->name, "title") == 0) {
			book.title = info(bookChild);
		} else if (strcmp((const char*)bookChild->name, "author") ==
			   0) {
			book.author = info(bookChild);
		} else if (strcmp((const char*)bookChild->name,
				  "publication_year") == 0) {
			book.publication_year = std::stoi(info(bookChild));
		}
	}
}
// Get content from an XML node
const char* info(xmlNode* child) {
	return (const char*)xmlNodeGetContent(child);
}
// Determines the type of input (xml data or select)
int inputType(const std::string& xmlData) {
	xmlDocPtr doc = xmlReadMemory(xmlData.c_str(), xmlData.length(),
				      nullptr, nullptr, 0);
	xmlNode* root = xmlDocGetRootElement(doc);
	// Check if the document is empty
	if (root == nullptr) {
		std::cerr << "empty xml document " << std::endl;
		xmlFreeDoc(doc);
		return -1;
	}

	std::string operationType;  //  Variable to store the operation type
	// Traverse the child nodes of the root to find the operation type
	for (xmlNode* node = root->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE &&
		    xmlStrcmp(node->name, BAD_CAST "operation") == 0) {
			xmlChar* type = xmlGetProp(node, BAD_CAST "type");
			if (type != nullptr) {
				operationType =
				    reinterpret_cast<const char*>(type);
				xmlFree(type);
			}
			break;	//// Break after finding the operation type
		}
	}
	xmlFreeDoc(doc);
	// Return 1 if the operation type is "select", otherwise return 0
	if (strcmp(operationType.c_str(), "select") == 0) {
		return 1;
	} else {
		return 0;
	}
}
