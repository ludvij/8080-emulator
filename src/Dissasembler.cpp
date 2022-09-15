#include "Dissasembler.h"
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <iomanip>

#if defined DEBUG || defined _DEBUG
	#define LOG_TRACE(x) std::cout << x;
#else
	#define LOG_TRACE(x)
#endif


namespace {
	template<typename ... Args>
	std::string format(const std::string& format, Args ... args) {
		// get string size
		int opcode_size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (opcode_size <= 0) { 
			throw std::runtime_error( "Error during formatting."); 
		}
		size_t size = static_cast<size_t>(opcode_size);
		// create buffer
		char* buffer = new char[size];
		// fill buffer
		std::snprintf(buffer, size, format.c_str(), args ...);

		// convert char* to std string
		std::string res = std::string(buffer, buffer + size - 1);
		// free memory
		delete buffer;
		return res;
	}

	OpcodeData makeOpcode(const char* opcode, const char* argPrefix) 
	{
		std::string symbol = format("%-7s%s", opcode, argPrefix);
		return {symbol, 1};
	}

	OpcodeData makeOpcode2b(const char* opcode, const char* argPrefix, uint8_t data)
	{
		std::string symbol = format("%-7s%s$%02x", opcode, argPrefix, data);
		return {symbol, 2};
	}

	OpcodeData makeOpcode4b(const char* opcode, const char* argPrefix, uint8_t dataHigh, uint8_t dataLow)
	{
		std::string symbol = format("%-7s%s$%02x%02x", opcode, argPrefix, dataHigh, dataLow);
		return {symbol, 3};
	}


	OpcodeData opcodeNotFound()
	{
	#ifdef THROW_ON_UNSUPPORTED_OPCODE
		throw std::runtime_error("unsupported opcode");
	#endif//THROW_ON_UNSUPPORTED_OPCODE
	#ifdef NOP_ON_UNSUPPORTED_OPCODE
		return {"NOP", 1};
	#else
		return {"-", 1};
	#endif//NOP_ON_UNSUPPORTED_OPCODE
	}
}


OpcodeData dissasemble::dissasemble(std::vector<uint8_t>& code, int pc)
{

	OpcodeData opcode;

	switch(code[pc])
	{
		case 0x00: opcode = {"NOP", 1}; break;
		case 0x01: opcode = makeOpcode4b("LXI","B, #", code[pc + 2], code[pc + 1]); break;
		case 0x02: opcode = makeOpcode("STAX", "B"); break;
		case 0x03: opcode = makeOpcode("INX", "B"); break;
		case 0x04: opcode = makeOpcode("INR", "B"); break;
		case 0x05: opcode = makeOpcode("DCR", "B"); break;
		case 0x06: opcode = makeOpcode2b("MVI","B, #", code[pc + 1]); break;
		case 0x07: opcode = {"RLC", 1}; break;

		case 0x08: opcode = opcodeNotFound(); break;
		case 0x09: opcode = makeOpcode("DAD", "B"); break;
		case 0x0A: opcode = makeOpcode("LDAX", "B"); break;
		case 0x0B: opcode = makeOpcode("DCX", "B"); break;
		case 0x0C: opcode = makeOpcode("INR", "C"); break;
		case 0x0D: opcode = makeOpcode("DCR", "C"); break;
		case 0x0E: opcode = makeOpcode2b("MVI", "C, #", code[pc + 1]); break;
		case 0x0F: opcode = {"RRC", 1}; break;

		case 0x10: opcode = opcodeNotFound(); break;
		case 0x11: opcode = makeOpcode4b("LXI", "D, #", code[pc + 2], code[pc + 1]); break;
		case 0x12: opcode = makeOpcode("STAX", "D"); break;
		case 0x13: opcode = makeOpcode("INX", "D"); break;
		case 0x14: opcode = makeOpcode("INR", "D"); break;
		case 0x15: opcode = makeOpcode("DCR", "D"); break;
		case 0x16: opcode = makeOpcode2b("MVI", "D, #", code[pc + 1]); break;
		case 0x17: opcode = {"RAL", 1}; break;

		case 0x18: opcode = opcodeNotFound(); break;
		case 0x19: opcode = makeOpcode("DAD", "D"); break;
		case 0x1A: opcode = makeOpcode("LDAX", "D"); break;
		case 0x1B: opcode = makeOpcode("DCX", "D"); break;
		case 0x1C: opcode = makeOpcode("INR", "E"); break;
		case 0x1D: opcode = makeOpcode("DCR", "E"); break;
		case 0x1E: opcode = makeOpcode2b("MVI", "E, #", code[pc + 1]); break;
		case 0x1F: opcode = {"RAR", 1}; break;

		case 0x20: opcode = opcodeNotFound(); break;
		case 0x21: opcode = makeOpcode4b("LXI", "H, #", code[pc + 2], code[pc + 1]); break;
		case 0x22: opcode = makeOpcode4b("SHLD", "", code[pc + 2], code[pc + 1]); break;
		case 0x23: opcode = makeOpcode("INX", "H"); break;
		case 0x24: opcode = makeOpcode("INR", "H"); break;
		case 0x25: opcode = makeOpcode("DCR", "H"); break;
		case 0x26: opcode = makeOpcode2b("MVI", "H, #", code[pc + 1]); break;
		case 0x27: opcode = {"DAA", 1}; break;

		case 0x28: opcode = opcodeNotFound(); break;
		case 0x29: opcode = makeOpcode("DAD", "H"); break;
		case 0x2A: opcode = makeOpcode4b("LHLD", "", code[pc + 2], code[pc + 1]); break;
		case 0x2B: opcode = makeOpcode("DCX", "H"); break;
		case 0x2C: opcode = makeOpcode("INR", "L"); break;
		case 0x2D: opcode = makeOpcode("DCR", "L"); break;
		case 0x2E: opcode = makeOpcode2b("MVI", "L, #", code[pc + 1]); break;
		case 0x2F: opcode = {"CMA", 1}; break;

		case 0x30: opcode = opcodeNotFound(); break;
		case 0x31: opcode = makeOpcode4b("LXI","SP, #", code[pc + 2], code[pc + 1]); break;
		case 0x32: opcode = makeOpcode4b("STA", "", code[pc + 2], code[pc + 1]); break;
		case 0x33: opcode = makeOpcode("INX", "SP"); break;
		case 0x34: opcode = makeOpcode("INR", "M"); break;
		case 0x35: opcode = makeOpcode("DCR", "M"); break;
		case 0x36: opcode = makeOpcode2b("MVI","M, #", code[pc + 1]); break;
		case 0x37: opcode = {"STC", 1}; break;

		case 0x38: opcode = opcodeNotFound(); break;
		case 0x39: opcode = makeOpcode("DAD", "SP"); break;
		case 0x3A: opcode = makeOpcode4b("LDA", "", code[pc + 2], code[pc + 1]); break;
		case 0x3B: opcode = makeOpcode("DCX", "SP"); break;
		case 0x3C: opcode = makeOpcode("INR", "A"); break;
		case 0x3D: opcode = makeOpcode("DCR", "A"); break;
		case 0x3E: opcode = makeOpcode2b("MVI", "A, #", code[pc + 1]); break;
		case 0x3F: opcode = {"CMC", 1}; break;

		case 0x40: opcode = makeOpcode("MOV", "B, B"); break;
		case 0x41: opcode = makeOpcode("MOV", "B, C"); break;
		case 0x42: opcode = makeOpcode("MOV", "B, D"); break;
		case 0x43: opcode = makeOpcode("MOV", "B, E"); break;
		case 0x44: opcode = makeOpcode("MOV", "B, H"); break;
		case 0x45: opcode = makeOpcode("MOV", "B, L"); break;
		case 0x46: opcode = makeOpcode("MOV", "B, M"); break;
		case 0x47: opcode = makeOpcode("MOV", "B, A"); break;

		case 0x48: opcode = makeOpcode("MOV", "C, B"); break;
		case 0x49: opcode = makeOpcode("MOV", "C, C"); break;
		case 0x4A: opcode = makeOpcode("MOV", "C, D"); break;
		case 0x4B: opcode = makeOpcode("MOV", "C, E"); break;
		case 0x4C: opcode = makeOpcode("MOV", "C, H"); break;
		case 0x4D: opcode = makeOpcode("MOV", "C, L"); break;
		case 0x4E: opcode = makeOpcode("MOV", "C, M"); break;
		case 0x4F: opcode = makeOpcode("MOV", "C, A"); break;

		case 0x50: opcode = makeOpcode("MOV", "D, B"); break;
		case 0x51: opcode = makeOpcode("MOV", "D, C"); break;
		case 0x52: opcode = makeOpcode("MOV", "D, D"); break;
		case 0x53: opcode = makeOpcode("MOV", "D, E"); break;
		case 0x54: opcode = makeOpcode("MOV", "D, H"); break;
		case 0x55: opcode = makeOpcode("MOV", "D, L"); break;
		case 0x56: opcode = makeOpcode("MOV", "D, M"); break;
		case 0x57: opcode = makeOpcode("MOV", "D, A"); break;
		
		case 0x58: opcode = makeOpcode("MOV", "E, B"); break;
		case 0x59: opcode = makeOpcode("MOV", "E, C"); break;
		case 0x5A: opcode = makeOpcode("MOV", "E, D"); break;
		case 0x5B: opcode = makeOpcode("MOV", "E, E"); break;
		case 0x5C: opcode = makeOpcode("MOV", "E, H"); break;
		case 0x5D: opcode = makeOpcode("MOV", "E, L"); break;
		case 0x5E: opcode = makeOpcode("MOV", "E, M"); break;
		case 0x5F: opcode = makeOpcode("MOV", "E, A"); break;

		case 0x60: opcode = makeOpcode("MOV", "H, B"); break;
		case 0x61: opcode = makeOpcode("MOV", "H, C"); break;
		case 0x62: opcode = makeOpcode("MOV", "H, D"); break;
		case 0x63: opcode = makeOpcode("MOV", "H, E"); break;
		case 0x64: opcode = makeOpcode("MOV", "H, H"); break;
		case 0x65: opcode = makeOpcode("MOV", "H, L"); break;
		case 0x66: opcode = makeOpcode("MOV", "H, M"); break;
		case 0x67: opcode = makeOpcode("MOV", "H, A"); break;
		
		case 0x68: opcode = makeOpcode("MOV", "L, B"); break;
		case 0x69: opcode = makeOpcode("MOV", "L, C"); break;
		case 0x6A: opcode = makeOpcode("MOV", "L, D"); break;
		case 0x6B: opcode = makeOpcode("MOV", "L, E"); break;
		case 0x6C: opcode = makeOpcode("MOV", "L, H"); break;
		case 0x6D: opcode = makeOpcode("MOV", "L, L"); break;
		case 0x6E: opcode = makeOpcode("MOV", "L, M"); break;
		case 0x6F: opcode = makeOpcode("MOV", "L, A"); break;

		case 0x70: opcode = makeOpcode("MOV", "M, B"); break;
		case 0x71: opcode = makeOpcode("MOV", "M, C"); break;
		case 0x72: opcode = makeOpcode("MOV", "M, D"); break;
		case 0x73: opcode = makeOpcode("MOV", "M, E"); break;
		case 0x74: opcode = makeOpcode("MOV", "M, H"); break;
		case 0x75: opcode = makeOpcode("MOV", "M, L"); break;
		case 0x76: opcode = {"HLT", 1}; break;
		case 0x77: opcode = makeOpcode("MOV", "M, A"); break;
		
		case 0x78: opcode = makeOpcode("MOV", "A, B"); break;
		case 0x79: opcode = makeOpcode("MOV", "A, C"); break;
		case 0x7A: opcode = makeOpcode("MOV", "A, D"); break;
		case 0x7B: opcode = makeOpcode("MOV", "A, E"); break;
		case 0x7C: opcode = makeOpcode("MOV", "A, H"); break;
		case 0x7D: opcode = makeOpcode("MOV", "A, L"); break;
		case 0x7E: opcode = makeOpcode("MOV", "A, M"); break;
		case 0x7F: opcode = makeOpcode("MOV", "A, A"); break;

		case 0x80: opcode = makeOpcode("ADD", "B"); break;
		case 0x81: opcode = makeOpcode("ADD", "C"); break;
		case 0x82: opcode = makeOpcode("ADD", "D"); break;
		case 0x83: opcode = makeOpcode("ADD", "E"); break;
		case 0x84: opcode = makeOpcode("ADD", "H"); break;
		case 0x85: opcode = makeOpcode("ADD", "L"); break;
		case 0x86: opcode = makeOpcode("ADD", "M"); break;
		case 0x87: opcode = makeOpcode("ADD", "A"); break;
		
		case 0x88: opcode = makeOpcode("ADC", "B"); break;
		case 0x89: opcode = makeOpcode("ADC", "C"); break;
		case 0x8A: opcode = makeOpcode("ADC", "D"); break;
		case 0x8B: opcode = makeOpcode("ADC", "E"); break;
		case 0x8C: opcode = makeOpcode("ADC", "H"); break;
		case 0x8D: opcode = makeOpcode("ADC", "L"); break;
		case 0x8E: opcode = makeOpcode("ADC", "M"); break;
		case 0x8F: opcode = makeOpcode("ADC", "A"); break;

		case 0x90: opcode = makeOpcode("SUB", "B"); break;
		case 0x91: opcode = makeOpcode("SUB", "C"); break;
		case 0x92: opcode = makeOpcode("SUB", "D"); break;
		case 0x93: opcode = makeOpcode("SUB", "E"); break;
		case 0x94: opcode = makeOpcode("SUB", "H"); break;
		case 0x95: opcode = makeOpcode("SUB", "L"); break;
		case 0x96: opcode = makeOpcode("SUB", "M"); break;
		case 0x97: opcode = makeOpcode("SUB", "A"); break;
		
		case 0x98: opcode = makeOpcode("SBB", "B"); break;
		case 0x99: opcode = makeOpcode("SBB", "C"); break;
		case 0x9A: opcode = makeOpcode("SBB", "D"); break;
		case 0x9B: opcode = makeOpcode("SBB", "E"); break;
		case 0x9C: opcode = makeOpcode("SBB", "H"); break;
		case 0x9D: opcode = makeOpcode("SBB", "L"); break;
		case 0x9E: opcode = makeOpcode("SBB", "M"); break;
		case 0x9F: opcode = makeOpcode("SBB", "A"); break;

		case 0xA0: opcode = makeOpcode("ANA", "B"); break;
		case 0xA1: opcode = makeOpcode("ANA", "C"); break;
		case 0xA2: opcode = makeOpcode("ANA", "D"); break;
		case 0xA3: opcode = makeOpcode("ANA", "E"); break;
		case 0xA4: opcode = makeOpcode("ANA", "H"); break;
		case 0xA5: opcode = makeOpcode("ANA", "L"); break;
		case 0xA6: opcode = makeOpcode("ANA", "M"); break;
		case 0xA7: opcode = makeOpcode("ANA", "A"); break;
		
		case 0xA8: opcode = makeOpcode("XRA", "B"); break;
		case 0xA9: opcode = makeOpcode("XRA", "C"); break;
		case 0xAA: opcode = makeOpcode("XRA", "D"); break;
		case 0xAB: opcode = makeOpcode("XRA", "E"); break;
		case 0xAC: opcode = makeOpcode("XRA", "H"); break;
		case 0xAD: opcode = makeOpcode("XRA", "L"); break;
		case 0xAE: opcode = makeOpcode("XRA", "M"); break;
		case 0xAF: opcode = makeOpcode("XRA", "A"); break;

		case 0xB0: opcode = makeOpcode("ORA", "B"); break;
		case 0xB1: opcode = makeOpcode("ORA", "C"); break;
		case 0xB2: opcode = makeOpcode("ORA", "D"); break;
		case 0xB3: opcode = makeOpcode("ORA", "E"); break;
		case 0xB4: opcode = makeOpcode("ORA", "H"); break;
		case 0xB5: opcode = makeOpcode("ORA", "L"); break;
		case 0xB6: opcode = makeOpcode("ORA", "M"); break;
		case 0xB7: opcode = makeOpcode("ORA", "A"); break;
		
		case 0xB8: opcode = makeOpcode("CMP", "B"); break;
		case 0xB9: opcode = makeOpcode("CMP", "C"); break;
		case 0xBA: opcode = makeOpcode("CMP", "D"); break;
		case 0xBB: opcode = makeOpcode("CMP", "E"); break;
		case 0xBC: opcode = makeOpcode("CMP", "H"); break;
		case 0xBD: opcode = makeOpcode("CMP", "L"); break;
		case 0xBE: opcode = makeOpcode("CMP", "M"); break;
		case 0xBF: opcode = makeOpcode("CMP", "A"); break;

		case 0xC0: opcode = {"RNZ", 1}; break;
		case 0xC1: opcode = makeOpcode("POP", "B"); break;
		case 0xC2: opcode = makeOpcode4b("JNZ", "", code[pc + 2], code[pc + 1]); break;
		case 0xC3: opcode = makeOpcode4b("JMP", "", code[pc + 2], code[pc + 1]); break;
		case 0xC4: opcode = makeOpcode4b("CNZ", "", code[pc + 2], code[pc + 1]); break;
		case 0xC5: opcode = makeOpcode("PUSH", "B"); break;
		case 0xC6: opcode = makeOpcode2b("ADI", "#", code[pc + 1]); break;
		case 0xC7: opcode = makeOpcode("RST", "0"); break;

		case 0xC8: opcode = {"RZ", 1}; break;
		case 0xC9: opcode = {"RET", 1}; break;
		case 0xCA: opcode = makeOpcode4b("JZ", "", code[pc + 2], code[pc + 1]); break;
		case 0xCB: opcode = opcodeNotFound(); break;
		case 0xCC: opcode = makeOpcode4b("CZ", "", code[pc + 2], code[pc + 1]); break;
		case 0xCD: opcode = makeOpcode4b("CALL", "", code[pc + 2], code[pc + 1]); break;
		case 0xCE: opcode = makeOpcode2b("ACI", "#", code[pc + 1]); break;
		case 0xCF: opcode = makeOpcode("RST", "1"); break;

		case 0xD0: opcode = {"RNC", 1}; break;
		case 0xD1: opcode = makeOpcode("POP", "D"); break;
		case 0xD2: opcode = makeOpcode4b("JNC", "", code[pc + 2], code[pc + 1]); break;
		case 0xD3: opcode = makeOpcode2b("OUT", "#", code[pc + 1]); break;
		case 0xD4: opcode = makeOpcode4b("CNC", "", code[pc + 2], code[pc + 1]); break;
		case 0xD5: opcode = makeOpcode("PUSH", "D"); break;
		case 0xD6: opcode = makeOpcode2b("SUI", "#", code[pc + 1]); break;
		case 0xD7: opcode = makeOpcode("RST", "2"); break;
		
		case 0xD8: opcode = {"RC", 1}; break;
		case 0xD9: opcode = opcodeNotFound(); break;
		case 0xDA: opcode = makeOpcode4b("JC", "", code[pc + 2], code[pc + 1]); break;
		case 0xDB: opcode = makeOpcode2b("IN", "#", code[pc + 1]); break;
		case 0xDC: opcode = makeOpcode4b("CC", "", code[pc + 2], code[pc + 1]); break;
		case 0xDD: opcode = opcodeNotFound(); break;
		case 0xDE: opcode = makeOpcode2b("SBI", "#", code[pc + 1]); break;
		case 0xDF: opcode = makeOpcode("RST", "3"); break;

		case 0xE0: opcode = {"RPO", 1}; break;
		case 0xE1: opcode = makeOpcode("POP", "H"); break;
		case 0xE2: opcode = makeOpcode4b("JPO", "", code[pc + 2], code[pc + 1]); break;
		case 0xE3: opcode = {"XTHL", 1}; break;
		case 0xE4: opcode = makeOpcode4b("CPO", "", code[pc + 2], code[pc + 1]); break;
		case 0xE5: opcode = makeOpcode("PUSH", "H"); break;
		case 0xE6: opcode = makeOpcode2b("ANI", "#", code[pc + 1]); break;
		case 0xE7: opcode = makeOpcode("RST", "4"); break;
		
		case 0xE8: opcode = {"RPE", 1}; break;
		case 0xE9: opcode = {"PCHL", 1}; break;
		case 0xEA: opcode = makeOpcode4b("JPE", "", code[pc + 2], code[pc + 1]); break;
		case 0xEB: opcode = {"XCHG", 1}; break;
		case 0xEC: opcode = makeOpcode4b("CPE", "", code[pc + 2], code[pc + 1]); break;
		case 0xED: opcode = opcodeNotFound(); break;
		case 0xEE: opcode = makeOpcode2b("XRI", "#", code[pc + 1]); break;
		case 0xEF: opcode = makeOpcode("RST", "5"); break;

		case 0xF0: opcode = {"RP", 1}; break;
		case 0xF1: opcode = makeOpcode("POP", "PSW"); break;
		case 0xF2: opcode = makeOpcode4b("JP", "", code[pc + 2], code[pc + 1]); break;
		case 0xF3: opcode = {"DI", 1}; break;
		case 0xF4: opcode = makeOpcode4b("CP", "", code[pc + 2], code[pc + 1]); break;
		case 0xF5: opcode = makeOpcode("PUSH", "PSW"); break;
		case 0xF6: opcode = makeOpcode2b("ORI", "#", code[pc + 1]); break;
		case 0xF7: opcode = makeOpcode("RST", "6"); break;
		
		case 0xF8: opcode = {"RM", 1}; break;
		case 0xF9: opcode = {"SPHL", 1}; break;
		case 0xFA: opcode = makeOpcode4b("JM", "", code[pc + 2], code[pc + 1]); break;
		case 0xFB: opcode = {"EI", 1}; break;
		case 0xFC: opcode = makeOpcode4b("CM", "", code[pc + 2], code[pc + 1]); break;
		case 0xFD: opcode = opcodeNotFound(); break;
		case 0xFE: opcode = makeOpcode2b("CPI", "#", code[pc + 1]); break;
		case 0xFF: opcode = makeOpcode("RST", "7"); break;

		default  : opcode = opcodeNotFound();
	}

	// quick and dirty debug print may fix it may not
	#if defined DEBUG || defined _DEBUG
		std::cout << std::hex << std::setw(5) << std::setfill('0') << pc << std::dec << " | ";
		for (int i=0; i < opcode.size; i++) {
			std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)code[pc + i] << " ";
		}
		for (int i=opcode.size; i < 3; i++) {
			std::cout << std::setw(3) << std::setfill(' ') << "";
		}
		std::cout << "| " << opcode.symbol << std::endl;
	#endif
	
	return opcode;
}