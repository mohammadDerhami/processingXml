#include "server.h"
#include "database.h"
#include "parseXml.h"

sqlite3* db = nullptr ;

void handleClient(int);

int main()
{	//open db
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
		handleClient(new_socket);
        }

        sqlite3_close(db);
        close(sockfd);
	
	return 0;
}
//read data from socket
void handleClient(int clientSocket)
{
	std::string uuid;
	std::vector<Book> books;
	Address address;
	std::string libraryTitle ;

        char buffer[1024];
        std::string xmlData;
        int bytesReceived = 0;

        while((bytesReceived = read(clientSocket , buffer ,sizeof(buffer) -1 )) > 0)
        {
                buffer[bytesReceived] = '\0';
                xmlData += buffer;
        }
        
	std::cout<<"received request : \n" <<xmlData<<'\n';

        int request = inputType(xmlData);

        if(request == 0)
        {
		processXml(xmlData , address , uuid , books , libraryTitle );
		if(uuid.empty())
		{
			std::cout<<"Wrong input (uuid is null) "<<std::endl;
		}
		else{


			insertLibrary(db, uuid, libraryTitle);

			if(!books.empty())
			{
				insertBooks(db , books , uuid);
			}
			if(!address.city.empty() || !address.street.empty() || !address.zip.empty())
			{
				insertAddress(db , uuid ,address);
			}
		}
        }
        else if(request==1)
        {
		std::vector<Library> libraries ;
		fillLibraries(libraries , db);

		if(libraries.empty())
		{
			std::cout<<"no library "<<std::endl;
		}else
		{

			xmlChar* xmlBuff;
			int buffersize;

			xmlDocPtr xmlDoc = generateXml(libraries);
			xmlDocDumpFormatMemory(xmlDoc, &xmlBuff, &buffersize , 1); 

			std::cout.write((const char*)xmlBuff, buffersize);

			xmlFree(xmlBuff); 
			xmlFreeDoc(xmlDoc); 
			xmlCleanupParser();
		}
        }
}
