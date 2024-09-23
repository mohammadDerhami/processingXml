#include "server.h"
#include "database.h"
#include "parseXml.h"
std::mutex coutMutex;
sqlite3* db = nullptr ;
void handleClient(int);
int main()
{
        if (sqlite3_open("library.db", &db) != SQLITE_OK) 
	{
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return -1 ;  
	}



        initDatabase(db);

        //Creating Socket
        int sockfd = socket(AF_INET , SOCK_STREAM , 0 );

        if(sockfd < 0 )
        {
                std::cerr<<"Error creating socket";
        }

        //Setting

        struct sockaddr_in address;
        int addressLen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);

        //bind

        if(bind(sockfd , (struct sockaddr *)&address , addressLen ) < 0 )
        {
                std::cerr<<"bind failed";
		close(sockfd);
                return -1;
        }

        //Listen

        if(listen(sockfd , 3 ) < 0 )
        {
                std::cerr<<"listen failed";
                close(sockfd);
                return -1;
        }

        //Accept

        while (true)
        {
                int new_socket = accept(sockfd , (struct sockaddr *)&address , (socklen_t*)&addressLen);

                if(new_socket < 0)
                {
                        std::cerr<<"Accept failed";
                        continue;
                }
                std::thread clientThread(handleClient , new_socket);
                clientThread.detach();
        }

        sqlite3_close(db);
        close(sockfd);
	
	return 0;
}

void handleClient(int clientSocket)
{
        char buffer[1024];
        std::string xmlData;
        int bytesReceived = 0;

        //read data from socket
        while((bytesReceived = read(clientSocket , buffer ,sizeof(buffer) -1 )) > 0)
        {
                buffer[bytesReceived] = '\0';
                xmlData += buffer;
        }

        {
                std::lock_guard<std::mutex> guard(coutMutex);
                std::cout<<"received request : \n" <<xmlData<<'\n';
        }

        //processing XML
        int request = inputType(xmlData);
        if(request == 0)
        {
		processXml(xmlData ,db);
                std::cout<<"Seccesfully "<<std::endl;


        }
        else
        {
                //std::cout<<getLibraryAsXml(db);
        }
}
