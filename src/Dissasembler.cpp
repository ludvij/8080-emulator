#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include "Dissasembler.h"

#if defined DEBUG || defined _DEBUG
	#define LOG_TRACE_FMT(x, y) printf(x, y)
	#define LOG_TRACE(x) printf(x)
#else
	#define LOG_TRACE_FMT(x, y)
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
		std::snprintf(buffer.get(), size, format.c_str(), args ...);

		// convert char* to std string
		std::string res = std::string(buffer, buffer + size - 1);
		// free memory
		delete buffer;
		return res;
	}

	std::string makeOpcode(const char* opcode, const char* argPrefix) 
	{
		return format("%-7s%s", opcode, argPrefix);
	}

	std::string makeOpcodeD8(const char* opcode, const char* argPrefix, uint8_t data)
	{
		return format("%-7s%s$%02x", opcode, argPrefix, data);
	}

	std::string makeOpcodeD16(const char* opcode, const char* argPrefix, uint8_t dataHigh, uint8_t dataLow)
	{
		return format("%-7s%s$%02x%02x", opcode, argPrefix, dataHigh, dataLow);
	}


	std::string opcodeNotFound()
	{
	#ifdef THROW_ON_UNSUPPORTED_OPCODE
		throw std::runtime_error("unsupported opcode");
	#else
	#ifdef NOP_ON_UNSUPPORTED_OPCODE
		return "NOP";
	#else
		return "-";
	#endif//NOP_ON_UNSUPPORTED_OPCODE
	#endif//THROW_ON_UNSUPPORTED_OPCODE
	}
}


OpcodeData dissasemble::dissasemble(uint8_t* codeBuffer, int pc)
{
	uint8_t* code = &codeBuffer[pc];
	uint16_t opbytes = 1;
	LOG_TRACE("%04x ", pc);

	std::string strOpcode;

	switch(*code)
	{
		case 0x00: strOpcode = "NOP"; break;
		case 0x01: strOpcode = makeOpcodeD16("LXI","B, #", code[2], code[1]); opbytes=3; break;
		case 0x02: strOpcode = makeOpcode("STAX", "B"); break;
		case 0x03: strOpcode = makeOpcode("INX", "B"); break;
		case 0x04: strOpcode = makeOpcode("INR", "B"); break;
		case 0x05: strOpcode = makeOpcode("DCR", "B"); break;
		case 0x06: strOpcode = makeOpcodeD8("MVI","B, #0x", code[1]); opbytes=2; break;
		case 0x07: strOpcode = "RLC"; break;

		case 0x08: strOpcode = opcodeNotFound(); break;
		case 0x09: strOpcode = makeOpcode("DAD", "B"); break;
		case 0x0A: strOpcode = makeOpcode("LDAX", "B"); break;
		case 0x0B: strOpcode = makeOpcode("DCX", "B"); break;
		case 0x0C: strOpcode = makeOpcode("INR", "C"); break;
		case 0x0D: strOpcode = makeOpcode("DCR", "C"); break;
		case 0x0E: strOpcode = makeOpcodeD8("MVI", "C, #0x", code[1]); opbytes=2; break;
		case 0x0F: strOpcode = "RRC"; break;

		case 0x10: strOpcode = opcodeNotFound();
		case 0x11: strOpcode = makeOpcodeD16("LXI", "D, #", code[2], code[1]); opbytes=3; break;
		case 0x12: strOpcode = makeOpcode("STAX", "D"); break;
		case 0x13: strOpcode = makeOpcode("INX", "D"); break;
		case 0x14: strOpcode = makeOpcode("INR", "D"); break;
		case 0x15: strOpcode = makeOpcode("DCR", "D"); break;
		case 0x16: strOpcode = makeOpcodeD8("MVI", "D, #0x", code[1]); opbytes=2; break;
		case 0x17: strOpcode = "RAL"; break;

		case 0x18: strOpcode = opcodeNotFound(); break;
		case 0x19: strOpcode = makeOpcode("DAD", "D"); break;
		case 0x1A: strOpcode = makeOpcode("LDAX", "D"); break;
		case 0x1B: strOpcode = makeOpcode("DCX", "D"); break;
		case 0x1C: strOpcode = makeOpcode("INR", "E"); break;
		case 0x1D: strOpcode = makeOpcode("DCR", "E"); break;
		case 0x1E: strOpcode = makeOpcodeD8("MVI", "E, #0x", code[1]); opbytes=2; break;
		case 0x1F: strOpcode = "RAR"; break;

		case 0x20: strOpcode = opcodeNotFound(); break;
		case 0x21: strOpcode = makeOpcodeD16("LXI", "H, #", code[2], code[1]); opbytes=3; break;
		case 0x22: strOpcode = makeOpcodeD16("SHLD", "", code[2], code[1]); opbytes=3; break;
		case 0x23: strOpcode = makeOpcode("INX", "H"); break;
		case 0x24: strOpcode = makeOpcode("INR", "H"); break;
		case 0x25: strOpcode = makeOpcode("DCR", "H"); break;
		case 0x26: strOpcode = makeOpcodeD8("MVI", "H, #0x", code[1]); opbytes=2; break;
		case 0x27: strOpcode = "DAA"; break;

		case 0x28: strOpcode = opcodeNotFound(); break;
		case 0x29: strOpcode = makeOpcode("DAD", "H"); break;
		case 0x2A: strOpcode = makeOpcodeD16("LHLD", "", code[2], code[1]); opbytes=3; break;
		case 0x2B: strOpcode = makeOpcode("DCX", "H"); break;
		case 0x2C: strOpcode = makeOpcode("INR", "L"); break;
		case 0x2D: strOpcode = makeOpcode("DCR", "L"); break;
		case 0x2E: strOpcode = makeOpcodeD8("MVI", "L, #0x", code[1]); opbytes=2; break;
		case 0x2F: strOpcode = "CMA"; break;

		case 0x30: strOpcode = opcodeNotFound(); break;
		case 0x31: strOpcode = makeOpcodeD16("LXI","SP, #", code[2], code[1]); opbytes=3; break;
		case 0x32: strOpcode = makeOpcodeD16("STA", "", code[2], code[1]); opbytes=3; break;
		case 0x33: strOpcode = makeOpcode("INX", "SP"); break;
		case 0x34: strOpcode = makeOpcode("INR", "M"); break;
		case 0x35: strOpcode = makeOpcode("DCR", "M"); break;
		case 0x36: strOpcode = makeOpcodeD8("MVI","M, #0x", code[1]); opbytes=2; break;
		case 0x37: strOpcode = "STC"; break;

		case 0x38: strOpcode = opcodeNotFound(); break;
		case 0x39: strOpcode = makeOpcode("DAD", "SP"); break;
		case 0x3A: strOpcode = makeOpcodeD16("LDA", "", code[2], code[1]); opbytes=3; break;
		case 0x3B: strOpcode = makeOpcode("DCX", "SP"); break;
		case 0x3C: strOpcode = makeOpcode("INR", "A"); break;
		case 0x3D: strOpcode = makeOpcode("DCR", "A"); break;
		case 0x3E: strOpcode = makeOpcodeD8("MVI", "A, #0x", code[1]); opbytes=2; break;
		case 0x3F: strOpcode = "CMC"; break;

		case 0x40: strOpcode = makeOpcode("MOV", "B, B"); break;
		case 0x41: strOpcode = makeOpcode("MOV", "B, C"); break;
		case 0x42: strOpcode = makeOpcode("MOV", "B, D"); break;
		case 0x43: strOpcode = makeOpcode("MOV", "B, E"); break;
		case 0x44: strOpcode = makeOpcode("MOV", "B, H"); break;
		case 0x45: strOpcode = makeOpcode("MOV", "B, L"); break;
		case 0x46: strOpcode = makeOpcode("MOV", "B, M"); break;
		case 0x47: strOpcode = makeOpcode("MOV", "B, A"); break;

		case 0x48: strOpcode = makeOpcode("MOV", "C, B"); break;
		case 0x49: strOpcode = makeOpcode("MOV", "C, C"); break;
		case 0x4A: strOpcode = makeOpcode("MOV", "C, D"); break;
		case 0x4B: strOpcode = makeOpcode("MOV", "C, E"); break;
		case 0x4C: strOpcode = makeOpcode("MOV", "C, H"); break;
		case 0x4D: strOpcode = makeOpcode("MOV", "C, L"); break;
		case 0x4E: strOpcode = makeOpcode("MOV", "C, M"); break;
		case 0x4F: strOpcode = makeOpcode("MOV", "C, A"); break;

		case 0x50: strOpcode = makeOpcode("MOV", "D, B"); break;
		case 0x51: strOpcode = makeOpcode("MOV", "D, C"); break;
		case 0x52: strOpcode = makeOpcode("MOV", "D, D"); break;
		case 0x53: strOpcode = makeOpcode("MOV", "D, E"); break;
		case 0x54: strOpcode = makeOpcode("MOV", "D, H"); break;
		case 0x55: strOpcode = makeOpcode("MOV", "D, L"); break;
		case 0x56: strOpcode = makeOpcode("MOV", "D, M"); break;
		case 0x57: strOpcode = makeOpcode("MOV", "D, A"); break;
		
		case 0x58: strOpcode = makeOpcode("MOV", "E, B"); break;
		case 0x59: strOpcode = makeOpcode("MOV", "E, C"); break;
		case 0x5A: strOpcode = makeOpcode("MOV", "E, D"); break;
		case 0x5B: strOpcode = makeOpcode("MOV", "E, E"); break;
		case 0x5C: strOpcode = makeOpcode("MOV", "E, H"); break;
		case 0x5D: strOpcode = makeOpcode("MOV", "E, L"); break;
		case 0x5E: strOpcode = makeOpcode("MOV", "E, M"); break;
		case 0x5F: strOpcode = makeOpcode("MOV", "E, A"); break;

		case 0x60: strOpcode = makeOpcode("MOV", "H, B"); break;
		case 0x61: strOpcode = makeOpcode("MOV", "H, C"); break;
		case 0x62: strOpcode = makeOpcode("MOV", "H, D"); break;
		case 0x63: strOpcode = makeOpcode("MOV", "H, E"); break;
		case 0x64: strOpcode = makeOpcode("MOV", "H, H"); break;
		case 0x65: strOpcode = makeOpcode("MOV", "H, L"); break;
		case 0x66: strOpcode = makeOpcode("MOV", "H, M"); break;
		case 0x67: strOpcode = makeOpcode("MOV", "H, A"); break;
		
		case 0x68: strOpcode = makeOpcode("MOV", "L, B"); break;
		case 0x69: strOpcode = makeOpcode("MOV", "L, C"); break;
		case 0x6A: strOpcode = makeOpcode("MOV", "L, D"); break;
		case 0x6B: strOpcode = makeOpcode("MOV", "L, E"); break;
		case 0x6C: strOpcode = makeOpcode("MOV", "L, H"); break;
		case 0x6D: strOpcode = makeOpcode("MOV", "L, L"); break;
		case 0x6E: strOpcode = makeOpcode("MOV", "L, M"); break;
		case 0x6F: strOpcode = makeOpcode("MOV", "L, A"); break;

		case 0x70: strOpcode = makeOpcode("MOV", "M, B"); break;
		case 0x71: strOpcode = makeOpcode("MOV", "M, C"); break;
		case 0x72: strOpcode = makeOpcode("MOV", "M, D"); break;
		case 0x73: strOpcode = makeOpcode("MOV", "M, E"); break;
		case 0x74: strOpcode = makeOpcode("MOV", "M, H"); break;
		case 0x75: strOpcode = makeOpcode("MOV", "M, L"); break;
		case 0x76: strOpcode = "HLT"; break;
		case 0x77: strOpcode = makeOpcode("MOV", "M, A"); break;
		
		case 0x78: strOpcode = makeOpcode("MOV", "A, B"); break;
		case 0x79: strOpcode = makeOpcode("MOV", "A, C"); break;
		case 0x7A: strOpcode = makeOpcode("MOV", "A, D"); break;
		case 0x7B: strOpcode = makeOpcode("MOV", "A, E"); break;
		case 0x7C: strOpcode = makeOpcode("MOV", "A, H"); break;
		case 0x7D: strOpcode = makeOpcode("MOV", "A, L"); break;
		case 0x7E: strOpcode = makeOpcode("MOV", "A, M"); break;
		case 0x7F: strOpcode = makeOpcode("MOV", "A, A"); break;

		case 0x80: strOpcode = makeOpcode("ADD", "B"); break;
		case 0x81: strOpcode = makeOpcode("ADD", "C"); break;
		case 0x82: strOpcode = makeOpcode("ADD", "D"); break;
		case 0x83: strOpcode = makeOpcode("ADD", "E"); break;
		case 0x84: strOpcode = makeOpcode("ADD", "H"); break;
		case 0x85: strOpcode = makeOpcode("ADD", "L"); break;
		case 0x86: strOpcode = makeOpcode("ADD", "M"); break;
		case 0x87: strOpcode = makeOpcode("ADD", "A"); break;
		
		case 0x88: strOpcode = makeOpcode("ADC", "B"); break;
		case 0x89: strOpcode = makeOpcode("ADC", "C"); break;
		case 0x8A: strOpcode = makeOpcode("ADC", "D"); break;
		case 0x8B: strOpcode = makeOpcode("ADC", "E"); break;
		case 0x8C: strOpcode = makeOpcode("ADC", "H"); break;
		case 0x8D: strOpcode = makeOpcode("ADC", "L"); break;
		case 0x8E: strOpcode = makeOpcode("ADC", "M"); break;
		case 0x8F: strOpcode = makeOpcode("ADC", "A"); break;

		case 0x90: strOpcode = makeOpcode("SUB", "B"); break;
		case 0x91: strOpcode = makeOpcode("SUB", "C"); break;
		case 0x92: strOpcode = makeOpcode("SUB", "D"); break;
		case 0x93: strOpcode = makeOpcode("SUB", "E"); break;
		case 0x94: strOpcode = makeOpcode("SUB", "H"); break;
		case 0x95: strOpcode = makeOpcode("SUB", "L"); break;
		case 0x96: strOpcode = makeOpcode("SUB", "M"); break;
		case 0x97: strOpcode = makeOpcode("SUB", "A"); break;
		
		case 0x98: strOpcode = makeOpcode("SBB", "B"); break;
		case 0x99: strOpcode = makeOpcode("SBB", "C"); break;
		case 0x9A: strOpcode = makeOpcode("SBB", "D"); break;
		case 0x9B: strOpcode = makeOpcode("SBB", "E"); break;
		case 0x9C: strOpcode = makeOpcode("SBB", "H"); break;
		case 0x9D: strOpcode = makeOpcode("SBB", "L"); break;
		case 0x9E: strOpcode = makeOpcode("SBB", "M"); break;
		case 0x9F: strOpcode = makeOpcode("SBB", "A"); break;

		case 0xA0: strOpcode = makeOpcode("ANA", "B"); break;
		case 0xA1: strOpcode = makeOpcode("ANA", "C"); break;
		case 0xA2: strOpcode = makeOpcode("ANA", "D"); break;
		case 0xA3: strOpcode = makeOpcode("ANA", "E"); break;
		case 0xA4: strOpcode = makeOpcode("ANA", "H"); break;
		case 0xA5: strOpcode = makeOpcode("ANA", "L"); break;
		case 0xA6: strOpcode = makeOpcode("ANA", "M"); break;
		case 0xA7: strOpcode = makeOpcode("ANA", "A"); break;
		
		case 0xA8: strOpcode = makeOpcode("XRA", "B"); break;
		case 0xA9: strOpcode = makeOpcode("XRA", "C"); break;
		case 0xAA: strOpcode = makeOpcode("XRA", "D"); break;
		case 0xAB: strOpcode = makeOpcode("XRA", "E"); break;
		case 0xAC: strOpcode = makeOpcode("XRA", "H"); break;
		case 0xAD: strOpcode = makeOpcode("XRA", "L"); break;
		case 0xAE: strOpcode = makeOpcode("XRA", "M"); break;
		case 0xAF: strOpcode = makeOpcode("XRA", "A"); break;

		case 0xB0: strOpcode = makeOpcode("ORA", "B"); break;
		case 0xB1: strOpcode = makeOpcode("ORA", "C"); break;
		case 0xB2: strOpcode = makeOpcode("ORA", "D"); break;
		case 0xB3: strOpcode = makeOpcode("ORA", "E"); break;
		case 0xB4: strOpcode = makeOpcode("ORA", "H"); break;
		case 0xB5: strOpcode = makeOpcode("ORA", "L"); break;
		case 0xB6: strOpcode = makeOpcode("ORA", "M"); break;
		case 0xB7: strOpcode = makeOpcode("ORA", "A"); break;
		
		case 0xB8: strOpcode = makeOpcode("CMP", "B"); break;
		case 0xB9: strOpcode = makeOpcode("CMP", "C"); break;
		case 0xBA: strOpcode = makeOpcode("CMP", "D"); break;
		case 0xBB: strOpcode = makeOpcode("CMP", "E"); break;
		case 0xBC: strOpcode = makeOpcode("CMP", "H"); break;
		case 0xBD: strOpcode = makeOpcode("CMP", "L"); break;
		case 0xBE: strOpcode = makeOpcode("CMP", "M"); break;
		case 0xBF: strOpcode = makeOpcode("CMP", "A"); break;

		case 0xC0: strOpcode = "RNZ"; break;
		case 0xC1: strOpcode = makeOpcode("POP", "B"); break;
		case 0xC2: strOpcode = makeOpcodeD16("JNZ", "", code[2], code[1]); opbytes=3; break;
		case 0xC3: strOpcode = makeOpcodeD16("JMP", "", code[2], code[1]); opbytes=3; break;
		case 0xC4: strOpcode = makeOpcodeD16("CNZ", "", code[2], code[1]); opbytes=3; break;
		case 0xC5: strOpcode = makeOpcode("PUSH", "B"); break;
		case 0xC6: strOpcode = makeOpcodeD8("ADI", "#", code[1]); opbytes=2; break;
		case 0xC7: strOpcode = makeOpcode("RST", "0"); break;

		case 0xC8: strOpcode = "RZ"; break;
		case 0xC9: strOpcode = "RET"; break;
		case 0xCA: strOpcode = makeOpcodeD16("JZ", "", code[2], code[1]); opbytes=3; break;
		case 0xCB: strOpcode = opcodeNotFound(); break;
		case 0xCC: strOpcode = makeOpcodeD16("CZ", "", code[2], code[1]); opbytes=3; break;
		case 0xCD: strOpcode = makeOpcodeD16("CALL", "", code[2], code[1]); opbytes=3; break;
		case 0xCE: strOpcode = makeOpcodeD8("ACI", "#", code[1]); opbytes=2; break;
		case 0xCF: strOpcode = makeOpcode("RST", "1"); break;

		case 0xD0: strOpcode = "RNC"; break;
		case 0xD1: strOpcode = makeOpcode("POP", "D"); break;
		case 0xD2: strOpcode = makeOpcodeD16("JNC", "", code[2], code[1]); opbytes=3; break;
		case 0xD3: strOpcode = makeOpcodeD8("OUT", "#", code[1]); opbytes=2; break;
		case 0xD4: strOpcode = makeOpcodeD16("CNC", "", code[2], code[1]); opbytes=3; break;
		case 0xD5: strOpcode = makeOpcode("PUSH", "D"); break;
		case 0xD6: strOpcode = makeOpcodeD8("SUI", "#", code[1]); opbytes=2; break;
		case 0xD7: strOpcode = makeOpcode("RST", "2"); break;
		
		case 0xD8: strOpcode = "RC"; break;
		case 0xD9: strOpcode = opcodeNotFound(); break;
		case 0xDA: strOpcode = makeOpcodeD16("JC", "", code[2], code[1]); opbytes=3; break;
		case 0xDB: strOpcode = makeOpcodeD8("IN", "#", code[1]); opbytes=2; break;
		case 0xDC: strOpcode = makeOpcodeD16("CC", "", code[2], code[1]); opbytes=3; break;
		case 0xDD: strOpcode = opcodeNotFound(); break;
		case 0xDE: strOpcode = makeOpcodeD8("SBI", "#", code[1]); opbytes=2; break;
		case 0xDF: strOpcode = makeOpcode("RST", "3"); break;

		case 0xE0: strOpcode = "RPO"; break;
		case 0xE1: strOpcode = makeOpcode("POP", "H"); break;
		case 0xE2: strOpcode = makeOpcodeD16("JPO", "", code[2], code[1]); opbytes=3; break;
		case 0xE3: strOpcode = "XTHL"; break;
		case 0xE4: strOpcode = makeOpcodeD16("CPO", "", code[2], code[1]); opbytes=3; break;
		case 0xE5: strOpcode = makeOpcode("PUSH", "H"); break;
		case 0xE6: strOpcode = makeOpcodeD8("ANI", "#", code[1]); opbytes=2; break;
		case 0xE7: strOpcode = makeOpcode("RST", "4"); break;
		
		case 0xE8: strOpcode = "RPE"; break;
		case 0xE9: strOpcode = "PCHL"; break;
		case 0xEA: strOpcode = makeOpcodeD16("JPE", "", code[2], code[1]); opbytes=3; break;
		case 0xEB: strOpcode = "XCHG"; break;
		case 0xEC: strOpcode = makeOpcodeD16("CPE", "", code[2], code[1]); opbytes=3; break;
		case 0xED: strOpcode = opcodeNotFound(); break;
		case 0xEE: strOpcode = makeOpcodeD8("XRI", "#", code[1]); opbytes=2; break;
		case 0xEF: strOpcode = makeOpcode("RST", "5"); break;

		case 0xF0: strOpcode = "RP"; break;
		case 0xF1: strOpcode = makeOpcode("POP", "PSW"); break;
		case 0xF2: strOpcode = makeOpcodeD16("JP", "", code[2], code[1]); opbytes=3; break;
		case 0xF3: strOpcode = "DI"; break;
		case 0xF4: strOpcode = makeOpcodeD16("CP", "", code[2], code[1]); opbytes=3; break;
		case 0xF5: strOpcode = makeOpcode("PUSH", "PSW"); break;
		case 0xF6: strOpcode = makeOpcodeD8("ORI", "#", code[1]); opbytes=2; break;
		case 0xF7: strOpcode = makeOpcode("RST", "6"); break;
		
		case 0xF8: strOpcode = "RM"; break;
		case 0xF9: strOpcode = "SPHL"; break;
		case 0xFA: strOpcode = makeOpcodeD16("JM", "", code[2], code[1]); opbytes=3; break;
		case 0xFB: strOpcode = "EI"; break;
		case 0xFC: strOpcode = makeOpcodeD16("CM", "", code[2], code[1]); opbytes=3; break;
		case 0xFD: strOpcode = opcodeNotFound(); break;
		case 0xFE: strOpcode = makeOpcodeD8("CPI", "#", code[1]); opbytes=2; break;
		case 0xFF: strOpcode = makeOpcode("RST", "7"); break;

		default  : strOpcode = opcodeNotFound();
	}

	LOG_TRACE_FMT("%s\n", strOpcode);
	
	return {strOpcode, opbytes};
}