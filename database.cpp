#include "database.h"
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

