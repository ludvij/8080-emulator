#pragma once

#include <string>
#include <vector>

struct OpcodeData {
	std::string symbol;
	uint16_t size;
};

namespace dissasemble {
	OpcodeData dissasemble(std::vector<uint8_t>& codeBuffer, int pc);
}