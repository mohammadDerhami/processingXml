#ifndef MAIN_H
#define MAIN_H
#include "database.h"
#include "parseXml.h"
#include "server.h"

void openDb();
void process(const std::string xmlData , sqlite3* db);

#endif
