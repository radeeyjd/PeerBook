#include "PeerBook.h"
#include <iostream>
#include <fstream>
#include <string>

Logical::Logical() {
	initialize();
	_fileServices.start();
}

//int Logical::_numofFiles;
//Files Logical::_files[100];
//FileServices Logical::_fileServices;

int Logical::initialize() {
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
}

Files *Logical::getFileinfo(std::string fname) {
	for(int iii = 0; iii < _numofFiles - 1; iii++) {
		if(fname == _files[iii].filename) {
//	std::cout << _files[iii].filename << " " << _files[iii].IP << " " << _files[iii].port << std::endl;
			return &_files[iii];
		}
	}
	return NULL;
}

void Logical::list() {
	initialize();
	int flength = 0;
	for(int i = 0; i < _numofFiles - 2; i++)
		flength += _files[i].filename.size() + 3;
	flength -= 3;
	for(int i = 0; i < flength; i++) {
		if(i == flength/2) {
			std::cout << "home" << std::endl;
			break;
		}
		else
			std::cout << " ";
	}
	for(int i = 0; i < flength; i++) {
		if(i == flength/2) {
			std::cout << "  |" << std::endl;
			break;
		}
		else
			std::cout << " ";
	}	
	for(int i = 0; i < flength + _files[_numofFiles-2].filename.size() ; i++)
		std::cout << "_";
	std::cout << std::endl;
	
	for(int iii = 0; iii < _numofFiles - 1; iii++) {
		std::cout << "|";
		for(int i = 0; i < _files[iii].filename.size()+2; i++)
			std::cout <<" ";
		//	std::cout << _files[iii].filename << "   ";
	}
	std::cout << std::endl;
	for(int iii = 0; iii < _numofFiles - 1; iii++) {
		std::cout << _files[iii].filename << "   ";
	}
	std::cout << std::endl;
}

int Logical::shutdown() {
	_fileServices.stop();
}

int Logical::create(std::string filename) {
	_fileServices.create(filename);
}
