#include"database.h"
void initDatabase(sqlite3* db) {
	const char* createLibraryTable =
	"CREATE TABLE IF NOT EXISTS library ("
	"uuid TEXT PRIMARY KEY, "
	"title TEXT );";

	const char* createBooksTable =
	"CREATE TABLE IF NOT EXISTS books ("
	"id INTEGER PRIMARY KEY, "
	"title TEXT , "
	"author TEXT , "
	"publication_year INTEGER , "
	"uuid TEXT , "
	"FOREIGN KEY (uuid) REFERENCES library(uuid));";

	const char* createAddressTable =
	"CREATE TABLE IF NOT EXISTS address ("
	"street TEXT , "
	"city TEXT , "
	"zip  TEXT , "
	"uuid TEXT PRIMARY KEY , "
	"FOREIGN KEY (uuid) REFERENCES library(uuid));";

	char* errMsg;

	if (sqlite3_exec(db, createLibraryTable, 0, 0, &errMsg) != SQLITE_OK)
       	{
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	if (sqlite3_exec(db, createBooksTable, 0, 0, &errMsg) != SQLITE_OK)
       	{
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	if (sqlite3_exec(db, createAddressTable, 0, 0, &errMsg) != SQLITE_OK)
       	{
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}
void insertLibrary(sqlite3* db, const std::string& uuid, const std::string& title) {
	const char* sql =
        "INSERT INTO library (uuid, title) VALUES (?, ?) "
        "ON CONFLICT(uuid) DO UPDATE SET "
        "title = COALESCE(NULLIF(excluded.title, ''), library.title);";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_STATIC);
        
		sqlite3_bind_text(stmt, 2, title.empty() ? nullptr : title.c_str(), -1, SQLITE_STATIC);
		
		std::cout<<"library added to db "<<std::endl;

		if (sqlite3_step(stmt) != SQLITE_DONE) {
		    std::cerr << "Error inserting/updating library: " << sqlite3_errmsg(db) << std::endl;
		}
		else
		{
			std::cout<<"address added to db "<<std::endl;
		}
		sqlite3_finalize(stmt);
	} else {
		std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
	}
}

void insertBooks(sqlite3* db , std::vector<Book>& books , const std::string& uuid)
{
	const char* sql =
        "INSERT INTO books (id, title, author, publication_year, uuid) VALUES (?, ?, ?, ?, ?) "
        "ON CONFLICT(id) DO UPDATE SET "
        "title = COALESCE(NULLIF(excluded.title, ''), books.title), "
        "author = COALESCE(NULLIF(excluded.author, ''), books.author), "
        "publication_year = excluded.publication_year;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {

		std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;

		return;

	}


	for (const auto& book : books) {
		sqlite3_bind_int(stmt,1 , book.id);
		sqlite3_bind_text(stmt, 2, book.title.empty() ? nullptr : book.title.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, book.author.empty() ? nullptr : book.author.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4, book.publication_year); 
		sqlite3_bind_text(stmt , 5 , book.uuid.c_str() , -1 , SQLITE_STATIC); 
		
		
		if (sqlite3_step(stmt) != SQLITE_DONE) {

			std::cerr << "Error inserting/updating book with uuid " << uuid << ": " << sqlite3_errmsg(db) << std::endl;
		}


		sqlite3_reset(stmt); 

	}


    sqlite3_finalize(stmt); 

}
void insertAddress(sqlite3* db, const std::string& uuid , Address& address) {

	const char* sql =

	"INSERT INTO address (city, street, zip, uuid) VALUES (?, ?, ?, ?) " 
	"ON CONFLICT(uuid) DO UPDATE SET "
	"city = COALESCE(NULLIF(excluded.city, ''), address.city), "
	"street = COALESCE(NULLIF(excluded.street, ''), address.street), "
	"zip = COALESCE(NULLIF(excluded.zip, ''), address.zip);";

	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, address.city.empty() ? nullptr : address.city.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, address.street.empty() ? nullptr : address.street.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, address.zip.empty() ? nullptr : address.zip.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, uuid.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) != SQLITE_DONE) {
		    std::cerr << "Error inserting/updating address: " << sqlite3_errmsg(db) << std::endl;
		}
		sqlite3_finalize(stmt);
    } else {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
    }
}
xmlNodePtr createLibraryNode(xmlDocPtr doc, const Library& library) {
        xmlNodePtr libraryNode = xmlNewNode(nullptr, BAD_CAST "library");

       	xmlNewChild(libraryNode, nullptr, BAD_CAST "uuid", BAD_CAST library.uuid.c_str());

	if(strcmp(library.title.c_str(),"")!=0)
		xmlNewChild(libraryNode, nullptr, BAD_CAST "title", BAD_CAST library.title.c_str());

        xmlNodePtr booksNode = xmlNewNode(nullptr, BAD_CAST "books");
        xmlAddChild(libraryNode, booksNode);

        for (const auto& book : library.books) {
                xmlNodePtr bookNode = xmlNewNode(nullptr, BAD_CAST "book");

                xmlNewChild(bookNode, nullptr, BAD_CAST "id", BAD_CAST std::to_string(book.id).c_str());

		if(strcmp(book.title.c_str(),"")!=0)
			xmlNewChild(bookNode, nullptr, BAD_CAST "title", BAD_CAST book.title.c_str());
		
		if(strcmp(book.author.c_str(),"")!=0)
			xmlNewChild(bookNode, nullptr, BAD_CAST "author", BAD_CAST book.author.c_str());
	
		if(book.publication_year!=0)
			xmlNewChild(bookNode, nullptr, BAD_CAST "publication_year", BAD_CAST std::to_string(book.publication_year).c_str());

                xmlAddChild(booksNode, bookNode);
        }

        xmlNodePtr addressNode = xmlNewNode(nullptr, BAD_CAST "address");
        xmlAddChild(libraryNode, addressNode);

	if(strcmp(library.address.street.c_str(), "")!=0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "street", BAD_CAST library.address.street.c_str());
	
	if(strcmp(library.address.city.c_str(), "")!=0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "city", BAD_CAST library.address.city.c_str());

	if(strcmp(library.address.zip.c_str(), "")!= 0)
		xmlNewChild(addressNode, nullptr, BAD_CAST "zip", BAD_CAST library.address.zip.c_str());

        return libraryNode;
}
xmlDocPtr generateXml(const std::vector<Library>& libraries) {
        xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "libraries");
        xmlDocSetRootElement(doc, rootNode);

        for (const auto& library : libraries) {
                xmlNodePtr libraryNode = createLibraryNode(doc, library);
                xmlAddChild(rootNode, libraryNode);
        }
          return doc;
}
void fillLibrary(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db)
{
        const char* libraryQuery =
        "SELECT uuid , title FROM library";

        sqlite3_stmt* libraryStmt;

        sqlite3_prepare_v2(db , libraryQuery , -1 , &libraryStmt , nullptr);

        while(sqlite3_step(libraryStmt) == SQLITE_ROW)
        {
                Library library;

                library.uuid = reinterpret_cast<const char*>(sqlite3_column_text(libraryStmt , 0));

                const char* title = reinterpret_cast<const char*>(sqlite3_column_text(libraryStmt , 1));
		library.title = title ? title : ""; 

                libraryMap[library.uuid] = library;
        }
        sqlite3_finalize(libraryStmt);
}
void fillAddress(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db)
{
        const char* addressQuery =
        "SELECT city , street , zip , uuid FROM address";

        sqlite3_stmt* addressStmt;

        sqlite3_prepare_v2(db , addressQuery , -1 , &addressStmt , nullptr);

	while(sqlite3_step(addressStmt) == SQLITE_ROW)
        {
                Address address;

                const char* city = reinterpret_cast<const char*>(sqlite3_column_text(addressStmt , 0));
		address.city = city ? city : "";

                const char* street = reinterpret_cast<const char*>(sqlite3_column_text(addressStmt , 1));
		address.street = street ? street : "";

               	const char* zip = reinterpret_cast<const char*>(sqlite3_column_text(addressStmt , 2));
		address.zip = zip ? zip : "";

                std::string uuid = reinterpret_cast<const char*>(sqlite3_column_text(addressStmt , 3));

                if(libraryMap.find(uuid) != libraryMap.end())
                {
                        libraryMap[uuid].address  = address;
                }
        }
        sqlite3_finalize(addressStmt);
}
void fillBooks(std::unordered_map<std::string , Library>& libraryMap , sqlite3* db)
{
	const char* bookQuery =
	"SELECT id, title, author, publication_year, uuid FROM books";
	sqlite3_stmt* bookStmt;
	sqlite3_prepare_v2(db, bookQuery, -1, &bookStmt, nullptr);

	while (sqlite3_step(bookStmt) == SQLITE_ROW) {
		Book book;

		book.id = sqlite3_column_int(bookStmt, 0);

		const char* title = reinterpret_cast<const char*>(sqlite3_column_text(bookStmt, 1));
		book.title = title ? title : ""; 

		const char* author = reinterpret_cast<const char*>(sqlite3_column_text(bookStmt, 2));
		book.author = author ? author : "";

		book.publication_year = sqlite3_column_int(bookStmt, 3);

		book.uuid = reinterpret_cast<const char*>(sqlite3_column_text(bookStmt, 4));

		if (libraryMap.find(book.uuid) != libraryMap.end()) {
		    libraryMap[book.uuid].books.push_back(book);

		}

	}
	sqlite3_finalize(bookStmt);
}
void fillLibraries(std::vector<Library>& libraries, sqlite3* db)
{
        std::unordered_map<std::string,Library> libraryMap;

        fillLibrary(libraryMap, db);

        fillAddress(libraryMap , db);

        fillBooks(libraryMap , db);

        for (const auto& pair : libraryMap) {

                libraries.push_back(pair.second);

        }
}
