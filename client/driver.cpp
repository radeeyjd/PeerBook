#include "PeerBook.h"
#include <iostream>

int main() {
	FileOperations fo;
	fo.readfile("file1", 0, 0);
	fo.list();
}
