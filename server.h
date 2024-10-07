#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include<thread>
#include<mutex>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include "database.h"
void creatingSocket(std::string& xmlData , sqlite3* db);
void dataInput(int clientSocket, sqlite3* db , std::string& xmlData);


#endif

