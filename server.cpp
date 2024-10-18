#include "server.h"

#include "main.h"
std::mutex coutMutex;
void creatingSocket(sqlite3* db) {
	// Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// Check if the socket created correctly
	if (sockfd < 0) {
		std::cerr << "Error creating socket";
	}
	// Address setting
	struct sockaddr_in address;
	int addressLen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);
	// Connect the socket to the specified address
	if (bind(sockfd, (struct sockaddr*)&address, addressLen) < 0) {
		std::cerr << "bind failed";
		close(sockfd);
	}
	// Prepare the socket to listen to incoming connections
	if (listen(sockfd, 3) < 0) {
		std::cerr << "listen failed";
		close(sockfd);
	}
	// Unlimited loop to accept new connections
	while (true) {
		int newSocket = accept(sockfd, (struct sockaddr*)&address,
				       (socklen_t*)&addressLen);

		if (newSocket < 0) {
			std::cerr << "Accept failed";
			continue;
		}

		std::thread clientThread(dataInput, newSocket, db);
		clientThread.detach();
	}

	close(sockfd);
}
// Read size of data
int readDataLength(int clientSocket) {
	int inputLength = 15;  // data length -> first 15 characters
	char lengthBuffer[inputLength + 1] = {0};
	int bytesRead = recv(clientSocket, lengthBuffer, inputLength, 0);
	if (bytesRead <= 0) {
		std::cerr << "Failed to read data length from socket."
			  << std::endl;

		return -1;
	}
	lengthBuffer[inputLength] = '\0';
	std::string lengthString(lengthBuffer);
	return std::stoi(lengthString);
}
// Read data from socket
void dataInput(int clientSocket, sqlite3* db) {
	int dataLength = readDataLength(clientSocket);
	std::cout << "dataLength : " << dataLength << std::endl;
	char* dataBuffer = new char[dataLength + 1];
	int totalReceived = 0;
	while (totalReceived < dataLength) {
		int bytesRead = recv(clientSocket, dataBuffer + totalReceived,
				     dataLength - totalReceived, 0);
		// size of data less than dataLength
		if (bytesRead <= 0) {
			break;
		}

		totalReceived += bytesRead;
	}
	dataBuffer[totalReceived] = '\0';
	std::string xmlData = dataBuffer;
	delete[] dataBuffer;
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "received request : \n" << xmlData << '\n';
	}

	int request = inputType(xmlData);  // request = 0 -> xml data
	// request = 1 -> select
	if (request == 0) {
		process(xmlData, db);
	} else if (request == 1) {
		std::string xml = select(db);
		// send data to client
		send(clientSocket, xml.c_str(), xml.size(), 0);
	}
}
