#include "database.h"
// Check the table exist
bool existTable(const std::string& name, sqlite3* db) {
	std::string query =
	    "SELECT name FROM sqlite_master WHERE type='table' AND name='" +
	    name + "';";
	sqlite3_stmt* stmt;
	std::cout << "query of exist table : " << query << std::endl;
	// Prepare the sql statement
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) !=
	    SQLITE_OK) {
		std::cerr << "Error preparing existTable " << sqlite3_errmsg(db)
			  << std::endl;
		return false;
	}
	bool exists = false;
	// Execute the statement and check the table exist
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		exists = true;	// If we get a row , the table exist
	}
	// Clean up
	sqlite3_finalize(stmt);
	return exists;
}
// Creating Table
// name -> name of the table
// bool mainTable -> specifies whether it is the main table or not
// mainTableName -> name of the mainTable
void createTable(const std::string& name, const std::vector<std::string>& properties,
		 sqlite3* db, bool mainTable, const std::string& mainTableName) {
	std::string query = "CREATE TABLE IF NOT EXISTS " + name + " (";
	std::string propertyQuery;
	if (mainTable) {
		for (std::vector<std::string>::size_type i = 0;
		     i < properties.size(); i++) {
			if (properties[i] == "uuid") {
				propertyQuery = properties[i] +
						" TEXT PRIMARY KEY NOT NULL  ";
				query = query + propertyQuery;
			} else {
				propertyQuery =
				    ", " + properties[i] + " TEXT NOT NULL  ";
				query = query + propertyQuery;
			}
		}
		query = query + ");";
	} else {
		for (std::vector<std::string>::size_type i = 0;
		     i < properties.size(); i++) {
			propertyQuery = properties[i] + " TEXT NOT NULL , ";
			query = query + propertyQuery;
		}
		query = query +
			"uuid TEXT PRIMARY KEY , "
			"FOREIGN KEY (uuid) REFERENCES " +
			mainTableName + " (uuid));";
	}
	std::cout << "query of create table : " << query << std::endl;
	char* errMsg;
	// Execute a SQL statement to create a table in the
	// SQLite database.
	if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) !=
	    SQLITE_OK) {
		std::cerr<<" in create table : "<<sqlite3_errmsg(db)<<std::endl;
		sqlite3_free(errMsg);
		return;
	}
	std::cout << "create table : " << name << " :) " << std::endl;
}
bool existUuid(sqlite3* db, const std::string& uuid) {
	// Retrieve the names of all tables
	std::string query =
	    "SELECT name FROM sqlite_master WHERE type='table';";
	sqlite3_stmt* stmt;
	// Prepare the query to retrive table names
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) !=
	    SQLITE_OK) {
		std::cerr << "Error preparing in existUUid  "
			  << sqlite3_errmsg(db) << std::endl;
		return false;
	}
	bool exists = false;
	// Check each table to see if the uuid exists
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const char* tableName =
		    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		std::string uuidQuery = "SELECT COUNT(*) FROM " +
					std::string(tableName) +
					" WHERE uuid = ?;";
		sqlite3_stmt* uuidStmt;
		if (sqlite3_prepare_v2(db, uuidQuery.c_str(), -1, &uuidStmt,
				       nullptr) != SQLITE_OK) {
			std::cerr << "Error preparing  in exist uuid "
				  << sqlite3_errmsg(db) << std::endl;
			return false;
		}
		sqlite3_bind_text(uuidStmt, 1, uuid.c_str(), -1, SQLITE_STATIC);
		// Execute the query
		if (sqlite3_step(uuidStmt) == SQLITE_ROW) {
			int count = sqlite3_column_int(uuidStmt, 0);
			if (count > 0) {
				exists = true;
				break;
			}
		}
		// Clean up
		sqlite3_finalize(uuidStmt);
	}
	sqlite3_finalize(stmt);
	return exists;
}
void update(sqlite3* db, const std::string& uuid,
	    const std::vector<std::string>& propertyName,
	    const std::vector<std::string>& propertyValue,
	    const std::string& tableName) {
	// Build query for update
	std::string query = "UPDATE " + tableName + " SET ";
	for (std::vector<std::string>::size_type i = 0; i < propertyName.size();
	     i++) {
		query = query + propertyName[i] + " = ?";
		if (i < propertyName.size() - 1) {
			query += ", ";
		}
	}
	query += " WEHRE uuid = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) !=
	    SQLITE_OK) {
		std::cerr << "Error to preparing in update  "
			  << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return;
	}
	// Bind property
	for (std::vector<std::string>::size_type i = 0; i < propertyName.size();
	     i++) {
		sqlite3_bind_text(stmt, i + 1, propertyValue[i].c_str(), -1,
				  SQLITE_STATIC);
	}
	sqlite3_bind_text(stmt, propertyName.size() + 1, uuid.c_str(), -1,
			  SQLITE_STATIC);
	// Execute
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "Error executing update " << sqlite3_errmsg(db)
			  << std::endl;
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);
}
void insert(sqlite3* db, const std::string& uuid,
	    const std::vector<std::string>& propertyName,
	    const std::vector<std::string>& propertyValue,
	    const std::string& tableName) {
	// Build query for insert
	std::string query = "INSERT INTO " + tableName + " (uuid";
	for (const auto& name : propertyName) {
		query += ", " + name;
	}
	query += ") VALUES (?, ";
	for (std::vector<std::string>::size_type i = 0; i < propertyName.size();
	     i++) {
		query += "?";
		if (i < propertyName.size() - 1) {
			query += ", ";
		}
	}
	query += ");";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) !=
	    SQLITE_OK) {
		std::cerr << "Error for preaparing in insert  "
			  << sqlite3_errmsg(db) << std::endl;
		return;
	}
	// Bind uuid
	sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_STATIC);
	// Bind property
	for (std::vector<std::string>::size_type i = 0;
	     i < propertyValue.size(); i++) {
		sqlite3_bind_text(stmt, i + 2, propertyValue[i].c_str(), -1,
				  SQLITE_STATIC);
	}
	// Execute
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "Error executing insert " << sqlite3_errmsg(db)
			  << std::endl;
		return;
	}
	sqlite3_finalize(stmt);
	std::cout << "insert in the : " << tableName << std::endl;
}

// function to open the Sqlite database
void openDb(sqlite3* db) {
	if (sqlite3_open("library.db", &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db)
			  << std::endl;
		return;
	}
}

/*xmlNodePtr createLibraryNode(xmlDocPtr doc, const Library& library) {
	xmlNodePtr libraryNode = xmlNewNode(nullptr, BAD_CAST "library");

	xmlNewChild(libraryNode, nullptr, BAD_CAST "uuid",
		    BAD_CAST library.uuid.c_str());

	if (strcmp(library.title.c_str(), "") != 0)
		xmlNewChild(libraryNode, nullptr, BAD_CAST "title",
			    BAD_CAST library.title.c_str());

	xmlNodePtr booksNode = xmlNewNode(nullptr, BAD_CAST "books");
	xmlAddChild(libraryNode, booksNode);

	for (const auto& book : library.books) {
		xmlNodePtr bookNode = xmlNewNode(nullptr, BAD_CAST "book");

		xmlNewChild(bookNode, nullptr, BAD_CAST "id",
			    BAD_CAST std::to_string(book.id).c_str());

		if (strcmp(book.title.c_str(), "") != 0)
			xmlNewChild(bookNode, nullptr, BAD_CAST "title",
				    BAD_CAST book.title.c_str());

		if (strcmp(book.author.c_str(), "") != 0)
			xmlNewChild(bookNode, nullptr, BAD_CAST "author",
				    BAD_CAST book.author.c_str());

		if (book.publication_year != 0)
			xmlNewChild(
			    bookNode, nullptr, BAD_CAST "publication_year",
			    BAD_CAST std::to_string(book.publication_year)
				.c_str());

		xmlAddChild(booksNode, bookNode);
	}

	xmlNodePtr addressNode = xmlNewNode(nullptr, BAD_CAST "address");
	xmlAddChild(libraryNode, addressNode);

	if (strcmp(library.address.street.c_str(), "") != 0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "street",
			    BAD_CAST library.address.street.c_str());

	if (strcmp(library.address.city.c_str(), "") != 0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "city",
			    BAD_CAST library.address.city.c_str());

	if (strcmp(library.address.zip.c_str(), "") != 0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "zip",
			    BAD_CAST library.address.zip.c_str());

	return libraryNode;
}
xmlDocPtr generateXml() {
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "libraries");
	xmlDocSetRootElement(doc, rootNode);

	for (const auto& library : libraries) {
		xmlNodePtr libraryNode = createLibraryNode(doc, library);
		xmlAddChild(rootNode, libraryNode);
	}
	return doc;
}
void fillLibrary(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db) {
	const char* libraryQuery = "SELECT uuid , title FROM library";

	sqlite3_stmt* libraryStmt;

	sqlite3_prepare_v2(db, libraryQuery, -1, &libraryStmt, nullptr);

	while (sqlite3_step(libraryStmt) == SQLITE_ROW) {
		Library library;

		library.uuid = reinterpret_cast<const char*>(
		    sqlite3_column_text(libraryStmt, 0));

		const char* title = reinterpret_cast<const char*>(
		    sqlite3_column_text(libraryStmt, 1));
		library.title = title ? title : "";

		libraryMap[library.uuid] = library;
	}
	sqlite3_finalize(libraryStmt);
}
// Fill the address details of libraries from the database into
// the libraryMap
void fillAddress(std::unordered_map<std::string, Library>& libraryMap,
		 sqlite3* db) {
	// SQL query to select city, street, zip, and uuid from
	// address table
	const char* addressQuery =
	    "SELECT city , street , zip , uuid FROM address";

	sqlite3_stmt* addressStmt;

	sqlite3_prepare_v2(db, addressQuery, -1, &addressStmt, nullptr);

	while (sqlite3_step(addressStmt) == SQLITE_ROW) {
		Address address;  // create address object

		const char* city = reinterpret_cast<const char*>(
		    sqlite3_column_text(addressStmt, 0));
		address.city = city ? city : "";

		const char* street = reinterpret_cast<const char*>(
		    sqlite3_column_text(addressStmt, 1));
		address.street = street ? street : "";

		const char* zip = reinterpret_cast<const char*>(
		    sqlite3_column_text(addressStmt, 2));
		address.zip = zip ? zip : "";

		std::string uuid = reinterpret_cast<const char*>(
		    sqlite3_column_text(addressStmt, 3));
		// Check if the UUID exists in the libraryMap,
		// and if so, assign the address
		if (libraryMap.find(uuid) != libraryMap.end()) {
			libraryMap[uuid].address = address;
		}
	}
	sqlite3_finalize(addressStmt);
}
// Fill the book details of libraries from the database into the
// libraryMap
void fillBooks(std::unordered_map<std::string, Library>& libraryMap,
	       sqlite3* db) {
	// SQL query to select id, title, author,
	// publication_year, and uuid from books table

	const char* bookQuery =
	    "SELECT id, title, author, publication_year, uuid "
	    "FROM books";
	sqlite3_stmt* bookStmt;
	sqlite3_prepare_v2(db, bookQuery, -1, &bookStmt, nullptr);

	while (sqlite3_step(bookStmt) == SQLITE_ROW) {
		Book book;

		book.id = sqlite3_column_int(bookStmt, 0);

		const char* title = reinterpret_cast<const char*>(
		    sqlite3_column_text(bookStmt, 1));
		book.title = title ? title : "";

		const char* author = reinterpret_cast<const char*>(
		    sqlite3_column_text(bookStmt, 2));
		book.author = author ? author : "";

		book.publication_year = sqlite3_column_int(bookStmt, 3);

		book.uuid = reinterpret_cast<const char*>(
		    sqlite3_column_text(bookStmt, 4));
		// Check if the book's UUID exists in the
		// libraryMap and add the book to the
		// corresponding library
		if (libraryMap.find(book.uuid) != libraryMap.end()) {
			libraryMap[book.uuid].books.push_back(book);
		}
	}
	sqlite3_finalize(bookStmt);
}
// Fill the libraries vector with libraries and their associated
// addresses and books
void fillLibraries(std::vector<Library>& libraries, sqlite3* db) {
	std::unordered_map<std::string, Library> libraryMap;

	fillLibrary(libraryMap,
		    db);  // Fill the libraryMap with libraries

	fillAddress(libraryMap,
		    db);  // Fill addresses for libraries in the map

	fillBooks(libraryMap,
		  db);	// Fill books for libraries in the map

	for (const auto& pair : libraryMap) {
		libraries.push_back(pair.second);
	}
}*/
