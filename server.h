#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include<thread>
#include<mutex>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
void handleClient(int clientSocket);


#endif

