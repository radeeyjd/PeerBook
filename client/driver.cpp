#include "PeerBook.h"
#include <iostream>

int main() {
	FileOperations fo;
	std::cout << "PeerBook VFS" << std::endl;
	std::cout << "1. Usage " << std::endl << "2. Read file " << std::endl;
	std::cout << "3. Write file " << std::endl;
	std::cout << "4. List files " << std::endl;
	std::cout << "5. Cache file " << std::endl;
	std::cout << "6. Exit PeerBook " << std::endl << std::endl;
	char req;
	std::string filename;
	while(1) {
		std::cout << "PeerBook> " ;
		std::cin >> req;
		switch(req) {
			case '1': {
				std::cout << std::endl << "PeerBook VFS" << std::endl;
				std::cout << "1. Usage " << std::endl << "2. Read file " << std::endl;
				std::cout << "3. Write file " << std::endl;
				std::cout << "4. List files " << std::endl;
				std::cout << "5. Cache file " << std::endl;
				std::cout << "5. Exit VFS " << std::endl << std::endl;
				break;
			}
			case '2': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.readfile(filename, 0, 0);
				break;
			}
			case '3': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.writefile(filename);
				break;
			}
			case '4': {
				fo.list();
				break;
			}
			case '5': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.cachefile(filename);
				break;
			}
			case '6': {
				return 0;
				break;
			}
			default: {
				std::cout << req << ": command not found" << std::endl;
				break;
			}
		}
	std::cout << std::endl;
	std::cin.ignore();
	}
}
