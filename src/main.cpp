#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

#include "Dissasembler.h"


// literally a hexdump lol
template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec)
{
	size_t size = vec.size();
	for(int i= 0; i < size; i+= 16) {
		stream << std::hex << std::setw(7) << std::setfill('0') << i << std::dec << "  ";
		for (int j = 0; j < 16; j++) {
			stream << std::hex << std::setw(2) << std::setfill('0') << (int)vec[i + j] << std::dec << " ";
		}
		stream << "\n";
	}
	return stream;
}


int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " filename" << std::endl;
		return 0;
	}
	std::ifstream file(argv[1], std::ios::binary | std::ios::in);
	std::vector<uint8_t> codeBuffer(
		(std::istreambuf_iterator<char>(file)), 
		std::istreambuf_iterator<char>()
	);
	file.close();
	
	size_t pc = 0;
	while(pc < codeBuffer.size()) {
		pc += dissasemble::dissasemble(codeBuffer, pc).size;
	}
	return 0;
}