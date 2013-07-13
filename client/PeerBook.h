#pragma once
#include <string>

//Data structure to store file metadata
class Files {
	public:
		std::string filename;
		std::string IP;
		int port;
};

//Provides mechanism to abstract file distribution
class Logical {
	private:
		int _numofFiles;
		Files _files[100];

	public:	
		int initialize();
		Files *getFileinfo(std::string fname);
		void list();
		Logical();
};

//User Interface layer and file operations
class FileOperations {
	private:
		Logical _logical;

	public:
		int createfile(std::string filename);
		int deletefile(std::string filename);
		int readfile(std::string filename, int version, int mode); //Request for the file from the home device
		int writefile(std::string filename); //write the file to the home device
		int list();
		int commmit(std::string filename);
		FileOperations();
};

