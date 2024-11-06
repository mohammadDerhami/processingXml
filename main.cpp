#include "main.h"
sqlite3* db = nullptr;
int main() {
	openDb(db);

	creatingSocket(
	    db);  // function to create socket and get data from client
	sqlite3_close(db);  // close the database connection
}

