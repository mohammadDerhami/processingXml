#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "database.h"
#include "parseXml.h"
void creatingSocket(sqlite3* db);
void dataInput(int clientSocket, sqlite3* db);
int readDataLength(int clientSocket);


#endif

