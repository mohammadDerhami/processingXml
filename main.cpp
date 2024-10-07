#include "server.h"
#include "database.h"
#include "parseXml.h"
sqlite3* db = nullptr;
void openDb();
void process(const std::string xmlData);

int main()
{
	std::string xmlData;

	openDb();    
        initDatabase(db);

	creatingSocket(xmlData , db);


        sqlite3_close(db);
}
void openDb()
{
	if (sqlite3_open("library.db", &db) != SQLITE_OK)
        {
                std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
        }
}
void process(const std::string xmlData)
{
	std::string uuid;
        std::vector<Book> books;
        Address address;
        std::string libraryTitle ;

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
