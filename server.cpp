#include "server.h"
#include "main.h"
std::mutex coutMutex;
void creatingSocket(std::string& xmlData , sqlite3* db)
{
        int sockfd = socket(AF_INET , SOCK_STREAM , 0 );

        if(sockfd < 0 )
        {
                std::cerr<<"Error creating socket";
        }


        struct sockaddr_in address;
        int addressLen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);


        if(bind(sockfd , (struct sockaddr *)&address , addressLen ) < 0 )
        {
                std::cerr<<"bind failed";
		close(sockfd);
        }


        if(listen(sockfd , 3 ) < 0 )
        {
                std::cerr<<"listen failed";
                close(sockfd);
        }


        while (true)
        {
                int new_socket = accept(sockfd , (struct sockaddr *)&address , (socklen_t*)&addressLen);

                if(new_socket < 0)
                {
                        std::cerr<<"Accept failed";
                        continue;
                }

		std::thread clientThread([=, &xmlData]() { 
		dataInput(new_socket, db, xmlData);
		});


                clientThread.detach();

		

        }

        close(sockfd);
	

}
//read data from socket
void dataInput(int clientSocket , sqlite3* db , std::string& xmlData)
{

        char buffer[1024];
        int bytesReceived = 0;
	std::string data;
        while((bytesReceived = read(clientSocket , buffer ,sizeof(buffer) -1 )) > 0)
        {
                buffer[bytesReceived] = '\0';
                data += buffer;
        }
	xmlData = data;
	{
                std::lock_guard<std::mutex> guard(coutMutex);
                std::cout<<"received request : \n" <<xmlData<<'\n';
        }
	process(xmlData);
	close(clientSocket);
}
