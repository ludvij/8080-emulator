#pragma once

#include <string>

struct OpcodeData {
	std::string symbol;
	uint16_t size;
};

namespace dissasemble {
	OpcodeData dissasemble(uint8_t* codeBuffer, int pc);
}