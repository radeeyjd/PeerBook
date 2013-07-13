#include "PeerBook.h"
#include <iostream>
#include <fstream>
#include <string>

Logical::Logical() {
	initialize();
}

int Logical::initialize() {
	std::ifstream fileList;		  		   //Create a new Stream
	_numofFiles = 0;	
	fileList.open("fileslist", std::ifstream::in);  //Open the stream
	while(fileList.good()) {
		fileList >> _files[_numofFiles].filename;
		fileList >>	_files[_numofFiles].IP;
		fileList >>	_files[_numofFiles].port;
		_numofFiles++;						//Set number of files 
	}

	fileList.close();						//Close file
	for(int iii = 0; iii < _numofFiles - 1; iii++)
		std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << std::endl;
}

Files *Logical::getFileinfo(std::string fname) {
	for(int iii = 0; iii < _numofFiles - 1; iii++) {
		if(fname == _files[iii].filename) {
	std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << std::endl;
			return &_files[iii];
		}
	}
	return NULL;
}

void Logical::list() {
	for(int iii = 0; iii < _numofFiles - 1; iii++)
		std::cout << iii << ". " << _files[iii].filename << std::endl;

}


/*int main() {
	Logical l;
	l.initialize();
	l.getFileinfo("file1");
}*/

