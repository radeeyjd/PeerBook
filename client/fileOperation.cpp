#include "PeerBook.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <sstream>
#include <netdb.h>
#include <bitset>
#include <sys/types.h> 

#define addrSize sizeof(struct sockaddr_in)


FileOperations::FileOperations() {
	//*_logical = new Logical;
}

int FileOperations::readfile(std::string filename, int version = 0, int mode = 0) {
	int serverSock, sent;		//Create a new server sock to connect to tracker
	struct hostent *serv_addr;	
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	Files *file = new Files;
	file = _logical.getFileinfo(filename);
	//Dynamically lookup the IP and port number for the filename
	server.sin_addr.s_addr = inet_addr((file->IP).c_str());	//IP address of tracker	
	server.sin_port = htons(file->port);			//Trackers Port

	//Contact the Tracker to get files and peers List
	if( (serverSock = socket( AF_INET, SOCK_STREAM, 0)) == -1 ) {
		std::cout << "Socket call failed" << std::endl;
	}	
	std::cout << "Connecting to the PeerServer" << std::endl;

	if( (connect(serverSock, (struct sockaddr *)&server, addrSize)) == -1) {
		std::cout << "Connection failed" << std::endl;
	}
	else {
	//'0' -- Joining the system
		int req = 0;
		sent = send(serverSock, &req, sizeof(int), 0);

		if(sent == -1) {
			std::cout << "Send Error" << std::endl;
		}

		int fnameSize = filename.size();
		sent = send(serverSock, &fnameSize, sizeof(size_t), 0); //Send file size
		sent = send(serverSock, filename.c_str(), fnameSize, 0);	//Send file name
		int fileSize,recvd;
		recvd = recv(serverSock, &fileSize, sizeof(int), 0);
		char buf[65536];
		char fdir[] = "temp/";
		strcat(fdir,filename.c_str());
		FILE *pFile;	
		pFile = fopen(fdir, "w");
		recvd = recv(serverSock, buf, fileSize, 0);
		fwrite(buf, 1, fileSize, pFile);
		fclose(pFile);
		}
		close(serverSock);	
}

int FileOperations::list() {
	_logical.list();
}

int main() {
	FileOperations fo;
//	fo.readfile("file1", 0, 0);
	fo.list();
}
