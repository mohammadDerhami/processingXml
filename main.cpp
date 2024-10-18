#include "main.h"
sqlite3* db = nullptr;
void openDb();
void process(const std::string xmlData);

int main() {
	openDb();
	initDatabase(db);  // Initialize the database

	creatingSocket(
	    db);  // function to create socket and get data from client
	sqlite3_close(db);  // close the database connection
}
// function to open the Sqlite database
void openDb() {
	if (sqlite3_open("library.db", &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db)
			  << std::endl;
		return;
	}
}

