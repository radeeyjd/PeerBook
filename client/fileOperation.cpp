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
		pthread_create(&uid, NULL, &FileOperations::updateManager, NULL);
}

std::queue<std::string> FileOperations::updateQ;
Logical FileOperations::_logical;

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
	//'0' -- Reading a file from the home device
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
		std::string fdir;
		if(mode == 0)
			fdir = "home/temp/";
		else if(mode == 1)
			fdir = "home/cache/";
		fdir.append(filename);	
		FILE *pFile;	
		pFile = fopen(fdir.c_str(), "w");
		recvd = recv(serverSock, buf, fileSize, 0);
		fwrite(buf, 1, fileSize, pFile);
		fclose(pFile);
		}
		close(serverSock);	
}

int FileOperations::writefile(std::string filename) {
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
		std::cout << "Home device is down, Updated the transmission queue" << std::endl;
		//Possibly device is offline update the update queue and retry every 10 minutes
		updateQ.push(filename);
		std::cout << updateQ.size() << std::endl;
	}
	else {
	//'1' -- Writing a file to the home device
		int req = 1;
		sent = send(serverSock, &req, sizeof(int), 0);
		if(sent == -1) {
			std::cout << "Send Error" << std::endl;
		}
		
		char fdir[20] = "home/temp/";
		strcat(fdir,filename.c_str());
		int fnameSize = filename.size();
		sent = send(serverSock, &fnameSize, sizeof(size_t), 0); //Send file size
		sent = send(serverSock, filename.c_str(), fnameSize, 0);	//Send file name
		int fileSize;
		FILE *pFile;
		char buf[65536];				//Get the file size
		pFile = fopen(fdir, "rb");
		fseek(pFile, 0, SEEK_END);
		fileSize = ftell(pFile);
		rewind(pFile);
		sent = send(serverSock, &fileSize, sizeof(int), 0);
		fread(buf,1, fileSize,pFile);
		fclose(pFile);
		sent = send(serverSock, buf, fileSize, 0);
		std::cout << "File" << fdir << " is sent" << std::endl;
		}
		close(serverSock);		
}

int FileOperations::list() {
	_logical.list();
}

void * FileOperations::updateManager(void * arg) {
	std::string fname;
	while(1) {
		int qsize = updateQ.size();
		for(int iii = 0; iii < qsize; iii++) {
			std::cout << "Inconsistent files are updated" << std::endl;
			fname = updateQ.front();
			updateQ.pop();
			std::cout << "file " << fname << std::endl;
			writefile(fname);
		}
		sleep(100);
	}
}

//Copy the file to local Cached folder
//1. get the version and store the version
int FileOperations::cachefile(std::string filename) {
	 readfile(filename, 0, 1); 
}
