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
#include <stdlib.h>
#include <signal.h>

int FileServices::_numofFiles = 0;
int FileServices::_numofPeers = 0;
Files FileServices::_files[100];
Peer FileServices::_peers[6];
int keepalive = 1;
int serverSock;
//Start the peer server
void * FileServices::peerServer(void * arg) {
//std::cout << "Starting to listen " << std::endl;
	int sent, newsockfd, oldtype;
	struct hostent *serv_addr;
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(10090);

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

	//create a socket
	if((serverSock = socket( AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "PeerServer socket call failed" << std::endl;
	}	
	
	//bind the socket
	if((bind(serverSock, (struct sockaddr *)&server, sizeof(server))) == -1) {
		std::cout << "PeerServer Bind Call failed" << std::endl;
	}

	//Start Listening to connections
	if(listen(serverSock, 5) == -1) {
		std::cout << "PeerServer Listen call failed" << std::endl;
	}
	while(keepalive == 1) {
			
	//Accept connection
		if((newsockfd = accept(serverSock, NULL, NULL)) == -1) {
			std::cout << "PeerServer Accept call failed " << std::endl;

		}	
			if(fork() == 0) {
			std::cout << "New Request! " << std::endl;
			int request,rec, sent;
			rec = recv(newsockfd, &request, sizeof(int), 0);
			if(rec == -1) {
				std::cout << "Receive error " << std::endl;
			}
		std::cout << "Request type " << request << std::endl;
			//Switch on the request type
			switch(request) {
				case 0: {
				//Read file request
					std::cout << "New file read request " << std::endl;
					int fnameSize, ver, recvd, bypass;
					char fname[20];
					char fdir[20] = "home/current/";
					rec = recv(newsockfd, &fnameSize, sizeof(size_t), 0);
					rec = recv(newsockfd, fname, fnameSize, 0); //Assume it already has the file
					strcat(fdir,fname);
				
					ver = getVersion(fname);	
					sent = send(newsockfd, &ver, sizeof(int), 0);
				
					rec = recv(newsockfd, &bypass, sizeof(int), 0);				
					
					if(bypass == 0) {
						FILE *pFile;
						char buf[65536], fname1[100];
						int fileSize, n_chunks;				//Get the file size
						std::cout <<"File "<< fdir <<" is requested." << std::endl;
						pFile = fopen(fdir, "rb");
						fseek(pFile, 0, SEEK_END);
						fileSize = ftell(pFile);
						rewind(pFile);
						sent = send(newsockfd, &fileSize, sizeof(int), 0);
						fread(buf,1, fileSize,pFile);
						fclose(pFile);
						sent = send(newsockfd, buf, fileSize, 0);
						std::cout << "File is sent" << std::endl;
					}
					else {
						std::cout << "File not sent, already the version is latest " << std::endl;
					}
					close(newsockfd);
					break;	
					}
				case 1: {
				//Write file request
					std::cout << "New file write request " << std::endl;
					int fnameSize, recvd, fileSize;
					char fname[20];
					char fdir[20] = "home/current/";
					rec = recv(newsockfd, &fnameSize, sizeof(size_t), 0);
					rec = recv(newsockfd, fname, fnameSize, 0); //Assume it already has the file
					strcat(fdir,fname);
					FILE *pFile;
					char buf[65536];
					recvd = recv(newsockfd, &fileSize, sizeof(int), 0);
					recvd = recv(newsockfd, buf, fileSize, 0);
					pFile = fopen(fdir, "w");
					fwrite(buf, 1, fileSize, pFile);
					fclose(pFile);
					close(newsockfd);	
					std::cout <<"File "<< fdir <<" is updated." << std::endl;
					break;
					}
				case 2: {
				//Commit file to stable version
					initialize();
					std::cout << "A new commit request" << std::endl;
					int fnameSize, recvd, fileSize;
					char fname[] = " ";
					char fdir1[] = "home/current/";
					char fdir2[] = "home/stable/";
					char space[] = " ";

					char dot[] = ".";
					rec = recv(newsockfd, &fnameSize, sizeof(int), 0);
					rec = recv(newsockfd, fname, fnameSize, 0); //Assume it already has the file

					Files *file = new Files;
					file = getFileinfo(fname);
				
					strcat(fdir1,fname);
					strcat(fdir2,fname);
	
					char command[] = " ";	
					char cmd[] = "cp ";
					strcat(command, cmd);
					strcat(command, "home/current/");
					strcat(command, fname);
					strcat(command, space);
					strcat(command, "home/stable/");
					strcat(command, fname);
					std::string cmmd(command);
					char ver[] = " ";
					sprintf(ver, "%d", file->version);	
					char mvcmd[] = " ";
					char mv[] = "mv ";
					strcat(mvcmd, mv);
					strcat(mvcmd, fdir2);
					strcat(mvcmd, space);
					strcat(mvcmd, fdir2);
					strcat(mvcmd, dot);
					strcat(mvcmd, ver);
					system(mvcmd);
					system(cmmd.c_str());				
					file->version++;
					updatefilelist();

					close(newsockfd);
					break;
				}
				case 3: {
				//New file being added to the VFS
					std::cout << "New file is added to the PeerBook" << std::endl;
					int fnameSize;	
					char fname[] = " ";
					rec = recv(newsockfd, &fnameSize, sizeof(int), 0);
					rec = recv(newsockfd, fname, fnameSize, 0); //Assume it already has the file
	
					char client_IP[20];
					struct sockaddr_in client;
					socklen_t len = sizeof(client);		
					if(getpeername(newsockfd, (struct sockaddr*)&client, &len) == -1) {
						std::cout << "Cannot get IP of Client" << std::endl;
					}
					inet_ntop(AF_INET, &client.sin_addr, client_IP, sizeof(client_IP));	
			
					try {
						std::ofstream outfile ("fileslist", std::ofstream::out | std::ofstream::app);	
						outfile << fname << " " << client_IP << " 10090 " << "0 " << "0 " << "0 " << std::endl;
					}
					catch (std::ofstream::failure e) {
    					std::cout << "Exception opening/reading/closing file" << std::endl;
					}
					break;
				}	
				default: {
					std::cout << "Illegal request" << std::endl;
					break;
				}
			}
			close(newsockfd);
		}	
	
   	}
	close(serverSock);			  
	//pthread_exit(NULL);	
}

FileServices::FileServices() {
}

int FileServices::start() {
	initialize();
	pthread_create(&sid, NULL, &FileServices::peerServer, NULL);
	return 1;
}

int FileServices::stop() {
	int stat;
	pthread_cancel(sid);
	pthread_kill(sid, stat);
	pthread_join(sid,NULL);
	close(serverSock);	
	pthread_exit(&sid);
	return 1;
}

int FileServices::killserver() {
	std::cout <<"Dead" <<std::endl;
	keepalive = 0;
}

int FileServices::initialize() {
	std::ifstream fileList;		  		   //Create a new Stream
	_numofFiles = 0;
	_numofPeers = 0;
	try {	
		fileList.open("fileslist", std::ifstream::in);  //Open the stream
		if(fileList.is_open()) {
			while(fileList.good()) {
				fileList >> _files[_numofFiles].filename;
				fileList >>	_files[_numofFiles].IP;
				fileList >>	_files[_numofFiles].port;
				fileList >> _files[_numofFiles].version;
				fileList >> _files[_numofFiles].is_home;
				fileList >> _files[_numofFiles].is_cached;
				_numofFiles++;						//Set number of files 
			}
			fileList.close();						//Close file
		}

		std::ifstream peersList;
		peersList.open("peerslist", std::ifstream::in);
		if(peersList.is_open()) {
			while(peersList.good()) {
				peersList >> _peers[_numofPeers].IP;
				peersList >> _peers[_numofPeers].port;
				_numofPeers++;
			}
		peersList.close();
		}
	}
	catch (std::ifstream::failure e) {
    	std::cout << "Exception opening/reading/closing file" << std::endl;
	}
	
//	for(int iii = 0; iii < _numofFiles - 1; iii++)
//		std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << " " << _files[iii].version  << " " << _files[iii].is_home << " " << _files[iii].is_cached << std::endl;
//for(int iii = 0; iii < _numofPeers - 1; iii++)
//	std::cout << _peers[iii].IP << " " << _peers[iii].port << std::endl;
}

Files *FileServices::getFileinfo(char* fname) {
	for(int iii = 0; iii < _numofFiles - 1; iii++) {
		if(fname == _files[iii].filename) {
	//std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << std::endl;
			return &_files[iii];
		}
	}
	return NULL;
}

int FileServices::updatefilelist() {
	std::cout << "Updating list" << std::endl;
	std::ofstream outfile ("fileslist", std::ofstream::binary);	
	for(int iii = 0; iii <_numofFiles - 1; iii++) {
			outfile << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << " " << _files[iii].version  << " " << _files[iii].is_home << " " << _files[iii].is_cached << std::endl;
	}
}

int FileServices::getVersion(char* fname) {
		for(int iii = 0; iii < _numofFiles - 1; iii++) {
		if(fname == _files[iii].filename) {
			//std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << std::endl;
			return _files[iii].version;
		}
	}
	return -1;
}

int FileServices::create(std::string filename) {
	for(int iii = 0; iii < _numofPeers - 1; iii++) {
		
	}
}
