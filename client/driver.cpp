#include "PeerBook.h"
#include <iostream>

int main() {
	FileOperations fo;
	std::cout << "PeerBook VFS" << std::endl;
	std::cout << "1. Usage " << std::endl << "2. Read file " << std::endl;
	std::cout << "3. Read stable file" << std::endl;
	std::cout << "4. Write file " << std::endl;
	std::cout << "5. List files " << std::endl;
	std::cout << "6. Cache file " << std::endl;
	std::cout << "7. Commit file" << std::endl;
	std::cout << "8. Exit PeerBook " << std::endl << std::endl;
	char req;
	std::string filename;
	bool cont = true;
	while(cont) {
		std::cout << "PeerBook> " ;
		std::cin >> req;
		switch(req) {
			case '1': {
				std::cout << std::endl << "PeerBook VFS" << std::endl;
				std::cout << "1. Usage " << std::endl << "2. Read file " << std::endl;
				std::cout << "3. Read stable file " << std::endl;
				std::cout << "4. Write file " << std::endl;
				std::cout << "5. List files " << std::endl;
				std::cout << "6. Cache file " << std::endl;
				std::cout << "7. Commit file" << std::endl;
				std::cout << "8. Exit VFS " << std::endl << std::endl;
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
				fo.readfile(filename, 0, 2);
				break;
			}
			case '4': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.writefile(filename);
				break;
			}
			case '5': {
				fo.list();
				break;
			}
			case '6': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.cachefile(filename);
				break;
			}
			case '7': {
				std::cout << "Please enter the file name " << std::endl;
				std::cin >> filename;
				fo.commit(filename);
				break;
			}
			case '8': {
				//fo.shutdown();
				cont = false;
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
