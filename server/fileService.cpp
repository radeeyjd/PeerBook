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

//Start the peer server
void * peerServer(void * arg) {
//std::cout << "Starting to listen " << std::endl;
	int serverSock, sent, newsockfd;
	struct hostent *serv_addr;
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(10090);

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
	while(1) {
			
	//Accept connection
		if((newsockfd = accept(serverSock, NULL, NULL)) == -1) {
			std::cout << "PeerServer Accept call failed " << std::endl;

		}	
			if(fork() == 0) {
		std::cout << "New File Request! " << std::endl;
			int request,rec, sent;
			rec = recv(newsockfd, &request, sizeof(int), 0);
			if(rec == -1) {
				std::cout << "Receive error " << std::endl;
			}
		std::cout << "Request type " << request << std::endl;
			//Switch on the request type
			switch(request) {
				case 0: {
				//Request for a file
					int fnameSize;
					char fname[20];
					char fdir[20] = "home/current/";
					rec = recv(newsockfd, &fnameSize, sizeof(size_t), 0);
					rec = recv(newsockfd, fname, fnameSize, 0); //Assume it already has the file
					strcat(fdir,fname);
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
					close(newsockfd);
					break;	
					}
				}
			}				
   		}
  	}


int main() {
	pthread_t sid;
	pthread_create(&sid, NULL, peerServer, NULL);
	pthread_join(sid, NULL);
	
}
