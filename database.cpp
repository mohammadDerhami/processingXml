#include"database.h"
void initDatabase(sqlite3* db)
{
	const char* createLibraryTable =
	"CREATE TABLE IF NOT EXISTS library ("
	"uuid TEXT PRIMARY KEY, "
	"title TEXT NOT NULL);";

	const char* createBooksTable =
	"CREATE TABLE IF NOT EXISTS books ("
	"id INTEGER PRIMARY KEY AUTOINCREMENT, "
	"title TEXT NOT NULL, "
	"author TEXT NOT NULL, "
	"publication_year INTEGER NOT NULL, "
	"library_uuid TEXT, "
	"FOREIGN KEY (library_uuid) REFERENCES library(uuid));";

	const char* createAddressTable =
	"CREATE TABLE IF NOT EXISTS address ("
	"street TEXT NOT NULL, "
	"city TEXT NOT NULL, "
	"zip  TEXT NOT NULL, "
	"library_uuid TEXT, "
	"FOREIGN KEY (library_uuid) REFERENCES library(uuid));";

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
		sqlite3_finalize(stmt);
	} else {
		std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
	}
}
void insertBooks(sqlite3* db , std::vector<Book>& books , const std::string& uuid)
{
	char* errMsg = nullptr;
	const char* sql = 
	"INSERT INTO books ( id, title, author, publication_year , uuid) VALUES (?, ?, ?, ?, ?) "
        "ON CONFLICT(uuid) DO UPDATE SET "
        "id = excluded.id, "
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
		sqlite3_bind_text(stmt , 5 , uuid.c_str() , -1 , SQLITE_STATIC); 
		
		
		if (sqlite3_step(stmt) != SQLITE_DONE) {

			std::cerr << "Error inserting/updating book with uuid " << book.uuid << ": " << sqlite3_errmsg(db) << std::endl;

		}


		sqlite3_reset(stmt); 

	}
	std::cout<<"books added to db "<<std::endl;


    sqlite3_finalize(stmt); 

}
void insertAddress(sqlite3* db, const std::string& uuid , Address& address) {
	const char* sql = 

        "ON CONFLICT(uuid) DO UPDATE SET "
        "city = COALESCE(excluded.city, addresses.city), "
        "street = COALESCE(excluded.street, addresses.street), "
        "zip = COALESCE(excluded.zip, addresses.zip);";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, address.city.empty() ? nullptr : address.city.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, address.street.empty() ? nullptr : address.street.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, address.zip.empty() ? nullptr : address.zip.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, uuid.c_str(), -1, SQLITE_STATIC);

		std::cout<<"address added to db "<<std::endl;
		if (sqlite3_step(stmt) != SQLITE_DONE) {
		    std::cerr << "Error inserting/updating address: " << sqlite3_errmsg(db) << std::endl;
		}
		sqlite3_finalize(stmt);
    } else {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
    }
}
