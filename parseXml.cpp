#include "parseXml.h"
// Pass the uuid and xmlData(string) to processNode function
void processXml(const std::string& xmlData, sqlite3* db) {
	std::string mainTable;
	std::string uuid;

	xmlDocPtr doc = xmlReadMemory(xmlData.c_str(), xmlData.length(),
				      nullptr, nullptr, 0);
	// Check if the XML parsing failed
	if (doc == nullptr) {
		std::cerr << "failed to parse xml " << std::endl;
		return;
	}
	xmlNode* root = xmlDocGetRootElement(doc);
	findUuid(uuid, root, mainTable);
	// pass atributes to processNode function , this
	// function process nodes extract information
	processNode(root, uuid, db, mainTable);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}
// process nodes recursive
void processNode(xmlNode* currentNode, std::string& uuid, sqlite3* db,
		 std::string& mainTable) {
	while (currentNode) {
		// Check if the current node is an
		// element node
		if (isElementNode(currentNode)) {
			if (hasAttribute(currentNode) &&
			    isObjectNode(currentNode)) {
				if (existTable(nodeName(currentNode), db)) {
					insertInDb(currentNode, uuid, db);
				} else {
					bool check = (mainTable ==
						      nodeName(currentNode));
					std::vector<std::string> properties =
					    propertyName(currentNode);
					createTable(nodeName(currentNode),
						    properties, db, check,
						    mainTable);
					insertInDb(currentNode, uuid, db);
				}
			}
		}
		processNode(currentNode->children, uuid, db, mainTable);
		currentNode = currentNode->next;
	}
}
// Finds UUID in XML nodes
void findUuid(std::string& uuid, xmlNode* currentNode, std::string& mainTable) {
	while (currentNode) {
		if (isElementNode(currentNode)) {
			if (strcmp((const char*)currentNode->name, "uuid") ==
			    0) {
				uuid = info(currentNode);
				mainTable = nodeName(currentNode->parent);
				return;
			}
		}
		// If the current node has children,
		// recursively search in the children
		if (currentNode->children) {
			findUuid(uuid, currentNode->children, mainTable);
		}

		currentNode = currentNode->next;
	}
}
// Check all children of node
// If has text node return true
// Else return false
bool hasAttribute(xmlNode* currentNode) {
	for (xmlNode* child = currentNode->children; child;
	     child = child->next) {
		if (isAttributeNode(child) && isElementNode(child)) {
			return true;
		}
	}
	return false;
}
// Cast xmlChar -> std::string
std::string casting(const xmlChar* s) {
	return std::string(reinterpret_cast<const char*>(s));
}  // Retrun name of node
std::string nodeName(xmlNode* node) { return casting(node->name); }
// Return value of node
std::string nodeValue(xmlNode* node) {
	return casting(xmlNodeGetContent(node));
}

// Create two vector
// And pass them to insert db
void insertInDb(xmlNode* currentNode, const std::string& uuid, sqlite3* db) {
	std::string tableName = nodeName(currentNode);
	std::vector<std::string> names = propertyName(currentNode);
	std::vector<std::string> values = propertyValue(currentNode);
	insert(db, uuid, names, values, tableName);
}
// If node is attribute -> true
// Else -> false
bool isAttributeNode(xmlNode* node) { return !isObjectNode(node); }
// If has element child -> object node
bool isObjectNode(xmlNode* node) {
	for (xmlNode* child = node->children; child; child = child->next) {
		if (isElementNode(child)) {
			return true;
		}
	}
	return false;
}
bool isElementNode(xmlNode* node) { return node->type == XML_ELEMENT_NODE; }

// Return names of property
std::vector<std::string> propertyName(xmlNode* currentNode) {
	std::vector<std::string> propertyName;
	for (xmlNode* child = currentNode->children; child;
	     child = child->next) {
		if (isAttributeNode(child) && isElementNode(child)) {
			propertyName.push_back(nodeName(child));
		}
	}
	return propertyName;
}
// Return values of property
std::vector<std::string> propertyValue(xmlNode* currentNode) {
	std::vector<std::string> propertyValue;
	for (xmlNode* child = currentNode->children; child;
	     child = child->next) {
		if (isAttributeNode(child)) {
			propertyValue.push_back(nodeValue(child));
		}
	}
	return propertyValue;
}
/*
std::string select(sqlite3* db) {
	// db is empty
		xmlChar* xmlBuff;
		int buffersize;

		xmlDocPtr xmlDoc = generateXml();
		xmlDocDumpFormatMemory(xmlDoc, &xmlBuff,
&buffersize, 1);
		// convert xmlChar* -> std::string
		std::string xmlString =
		    std::string(reinterpret_cast<const
char*>(xmlBuff)); xmlFree(xmlBuff); xmlFreeDoc(xmlDoc);
		xmlCleanupParser();
		return xmlString;

}
*/

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

	std::string operationType;  //  Variable to store the
				    //  operation type
	// Traverse the child nodes of the root to find
	// the operation type
	for (xmlNode* node = root->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE &&
		    xmlStrcmp(node->name, BAD_CAST "operation") == 0) {
			xmlChar* type = xmlGetProp(node, BAD_CAST "type");
			if (type != nullptr) {
				operationType =
				    reinterpret_cast<const char*>(type);
				xmlFree(type);
			}
			break;	//// Break after finding
				/// the operation type
		}
	}
	xmlFreeDoc(doc);
	// Return 1 if the operation type is "select",
	// otherwise return 0
	if (strcmp(operationType.c_str(), "select") == 0) {
		return 1;
	} else {
		return 0;
	}
}
