#pragma once
#include <string>
#include <queue>

//Data structure to store file metadata
class Files {
	public:
		std::string filename;
		std::string IP;
		int port;
		int version; //Version to update when the file is changed
					 //Version is -1 if the file is not locally cached
		bool is_home; //Local or remote file
		bool is_cached; //File cached locally or not 
};

//File Service layer

//Functions:
//	1. Send file
//	2. Send Version
//	3. Receive updated files
class FileServices {
	
	public:
		FileServices();
		int start();
		int stop();
		static void * peerServer(void * arg); 
		int initialize();
		static Files* getFileinfo(char* fname);
		static int updatefilelist();
		static int getVersion(char* fname);	
	
	private:
		pthread_t sid;
		static int _numofFiles;
		static Files _files[100];


};

//Provides mechanism to abstract file distribution

//Functions:
//	1. Show logical view
//	2. File information
class Logical {
	private:
		int _numofFiles;
		Files _files[100];
		FileServices _fileServices;
		
	public:	
		int initialize();
		Files *getFileinfo(std::string fname);
		void list();
		Logical();
};

//User Interface layer and file operations

//Functions:
//	1. Create
//	2. Delete
//	3. Read
// 	4. Write
//	5. Caching & Syncronization
//	6. Logical view of system
//	7. Join & Leave
class FileOperations {
	private:
		static Logical _logical;
		static std::queue<std::string> updateQ;
		pthread_t uid;
	
	public:
		int createfile(std::string filename);
		int deletefile(std::string filename);
		static int readfile(std::string filename, int version, int mode); //Request for the file from the home device
		static int writefile(std::string filename); //write the file to the home device
		static int list();
		int commmit(std::string filename);
		static void * updateManager(void * arg);
		static int cachefile(std::string filename); 
		FileOperations();
};


