#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
//#include <cstddef>
#include <string>
#include <stdio.h>
#include <netdb.h>
#include <iostream>

#define SOCKET_ERROR -1
#define BUFFER_SIZE 100
#define HOST_NAME_SIZE 255 

main(int argc, char **argv) {
//	std::cout << "Begin";
	int hSocket,hServerSocket;
	struct hostent* pHostInfo;
	struct sockaddr_in Address;
	long nHostAddress;
	char pBuffer[BUFFER_SIZE];
	unsigned nReadAmount;
//	char strHostName[HOST_NAME_SIZE];
//	int portno;
	extern char *optarg;
	int c, times_to_download = 1, err = 0;
	bool debug = false;
	if(argc < 3) {
		printf("\nUsage: [c-d] client host-name host-port\n");
		return 0;
	}
	while((c = getopt(argc, argv, "c:d")) != -1) {
			switch (c) {
				case 'c':
					times_to_download = atoi(optarg);
					break;
				case 'd':
					debug = true;
					break;
				case '?':
					err = 1;
					break;
			}
		}
	std::string page = argv[optind + 2];
	std::string strHostName = argv[optind];
	int portno = atoi(argv[optind + 1]);

//printf("times: %d",times_to_download);
//return 0;
	//std::cout << "hostname:" << strHostName << '\n';
	int timesDownloaded = 0;
for(int i = 0; i < times_to_download; i++) {
	// Making a socket
	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(hSocket == SOCKET_ERROR) {
		printf("\nCould not make a socket\n");
		return 0;
	}

	// Get IP address from name
 	pHostInfo = gethostbyname(strHostName.c_str());
	// copy address into long
	memcpy(&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);	

	// Fill address struct
//	printf("Fill address struct\n");
	Address.sin_addr.s_addr = nHostAddress;
	Address.sin_port = htons(portno);
	Address.sin_family = AF_INET;

//	printf("connect\n");
	if(connect(hSocket, (struct sockaddr*)&Address, sizeof(Address)) == SOCKET_ERROR) {
		printf("\nCould not connect to host\n");
		return 0;
	}

	// Create request
//	printf("Create request\n");
	std::string request = std::string("GET ") + page + std::string(" HTTP/1.1\nHost: ") + strHostName + std::string("\nAccept: */*\nContent-Type: text/html\nContent-Length: 0\r\n\r\n");

	
	// Write request
//	printf("Write request\n");
	write(hSocket, request.c_str(), request.length());
	if(debug) {
		printf(request.c_str());
	}
//return 0;

	// Read
	memset(&pBuffer[0], '\0', BUFFER_SIZE);		//check this and the one a few lines down
	nReadAmount = read(hSocket, pBuffer, BUFFER_SIZE);
	std::string readHeader(pBuffer);	//make sure this works
	memset(&pBuffer[0], '\0', BUFFER_SIZE);		//check this and the one a few lines down
//printf(readHeader.c_str());
//return 0;

	std::string content = "";
	bool headerDone = false;
//	std::string readHeader(pBuffer);	//make sure this works
//printf(readHeader.c_str());
//return 0;
	while(!headerDone) {
		std::size_t end = readHeader.find("\r\n\r\n");
		if(end == std::string::npos) {
			nReadAmount = read(hSocket, pBuffer, BUFFER_SIZE);
			readHeader.append(pBuffer);
			memset(&pBuffer[0], '\0', BUFFER_SIZE);
			//printf(readHeader.c_str());
			//return 0;
		} else {
			headerDone = true;
			content.append(readHeader.substr(end));
			readHeader.erase(end + 4);
		}
	}
//printf("header:\n");
//printf(readHeader.c_str());
//printf("content:\n");
//printf(content.c_str() + '\n');
//return 0;

	if(debug) {
		printf(readHeader.c_str());
	}
	std::size_t contLengthLoc = readHeader.find("Content-Length:") + 17;
	std::size_t endOfLengthLoc = readHeader.find("Content-Type:") - 2;//"\n", contLengthLoc);
//printf("%d  %d\n",contLengthLoc, endOfLengthLoc);
	std::string contLenStr = readHeader.substr(contLengthLoc, endOfLengthLoc - contLengthLoc );
	int contentLength = atoi(contLenStr.c_str());
//printf(contLenStr.c_str());
//printf("\n%d\n",contentLength);
//return 0;
	int amountRead = 0;
	while(amountRead < contentLength) {
		nReadAmount += read(hSocket, pBuffer, BUFFER_SIZE);
		content.append(pBuffer);
		memset(&pBuffer[0], '\0', BUFFER_SIZE);
		amountRead += BUFFER_SIZE;
}

	if(close(hSocket) == SOCKET_ERROR) {
		printf("\nCOuld not close socket\n");
		return 0;
	}

	if(debug || times_to_download == 1) {
		printf(content.c_str());
	}
//printf("times: %d",times_to_download);
	timesDownloaded++;
}
	if(times_to_download != 1) {
		printf("File downloaded %d times\n",timesDownloaded);
	}
return 0;
}
