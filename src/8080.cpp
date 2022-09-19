#include "8080.h"
#include <iostream>
#include <stdexcept>

namespace p8080 {

void State8080::unimplementedInstruction() {
	pc--;
	std::cerr << "ERROR: Unimplemented instruction" << std::endl;
	throw std::runtime_error("Unimplemented instruction");
}

void State8080::inx(reg_t& r1, reg_t& r2)
{
	// could add registers, increment, and then sepparate again, or could not, or could not and just do this
	if(r1 == 0xff && r2 == 0xff) {
		r1 = 0;
		r2 = 0;
	} else if(r2 == 0xff) {
		r1++;
		r2 = 0;
	} else {
		r2++;
	}
}
void State8080::inr(reg_t& r)
{
	// aux carry
	cc.ac = (r & 0xf) == 0xf;
	r++;
	// ALL FLAGS MINUS CARRY
	arithFlags(r, ~(FLAG_ALL & FLAG_CY));
}
void State8080::inr(reg_t& r)
{
	// aux carry
	cc.ac = 0;
	// ALL FLAGS MINUS CARRY
	arithFlags(r, ~(FLAG_ALL & FLAG_CY));
}
void State8080::dcx(reg_t& r1, reg_t& r2)
{
	// could add registers, decrement, and then sepparate again, or could not, or could not and just do this
	if(r1 == 0x00 && r2 == 0x00) {
		r1 = 0xff;
		r2 = 0xff;
	} else if((r2 == 0xff) && (r1 > 0)) {
		r1--;
		r2 = 0xff;
	} else {
		r2--;
	}
}

void State8080::dad(uint16_t rp)
{
	// TODO: make this simpler
	uint32_t hl_32 = (r.h << 8) | r.l;
	hl_32 = hl_32 + static_cast<uint32_t>(rp);
	cc.cy = (hl_32 > 0xffff);
	
	uint16_t hl = (hl_32 & 0xffff);
	r.h = static_cast<reg_t>(hl >> 8);
	r.l = static_cast<reg_t>(hl & 0xff);
}

void State8080::mov(uint8_t& r1, uint8_t r2) { r1 = r2;}
void State8080::add(uint8_t other) 
{
	//A = A + other
	uint16_t res = static_cast<uint16_t>(r.a) + static_cast<uint16_t>(other);
	arithFlags(res, FLAG_ALL);
	auxCarry(r.a, other);

	r.a = static_cast<reg_t>(res & 0xff);
}
void State8080::adc(uint8_t other) 
{
	//A = A + other + cy; 
	uint16_t res = static_cast<uint16_t>(r.a) + static_cast<uint16_t>(other) + static_cast<uint16_t>(cc.cy);
	arithFlags(res, FLAG_ALL);
	auxCarry(r.a, other);

	r.a = static_cast<reg_t>(res & 0xff);
}
void State8080::sub(uint8_t other) 
{
	//A = A - other
	uint16_t res = static_cast<uint16_t>(r.a) - static_cast<uint16_t>(other);
	arithFlags(res, FLAG_ALL);
	// I think the aux carry is always 0 in this case
	// auxCarry(r.a, other); 
	cc.ac = 0;

	r.a = static_cast<reg_t>(res & 0xff);
}
void State8080::sbb(uint8_t other) 
{
	//A = A - other
	uint16_t res = static_cast<uint16_t>(r.a) - static_cast<uint16_t>(other) - static_cast<uint16_t>(cc.cy);
	arithFlags(res, FLAG_ALL);
	// I think the aux carry is always 0 in this case
	// auxCarry(r.a, other); 
	cc.ac = 0;

	r.a = static_cast<reg_t>(res & 0xff);
}
void State8080::ana(reg_t r1, reg_t r2) { r1 = r1 & r2; }
void State8080::ora(reg_t r1, reg_t r2) { r1 = r1 | r2; }
void State8080::xra(reg_t r1, reg_t r2) { r1 = r1 ^ r2; }
void State8080::cmp(reg_t r1, reg_t r2) { r1 = r1 == r2;}

void State8080::jmp(uint8_t hi, uint8_t lo)	{ pc = (hi << 8) | lo; 					}
void State8080::jmp()						{ jmp(memory[pc + 2], memory[pc + 1]);	}

void State8080::rst(uint16_t address) 
{
	uint16_t ret = pc + 2;
	memory[sp - 1] = (ret >> 8) | 0xff;
	memory[sp - 2] = (ret & 0xff);
	sp -= 2;
	pc = address; 
}
void State8080::call()
{
	uint16_t ret = pc + 2;
	memory[sp - 1] = (ret >> 8) | 0xff;
	memory[sp - 2] = (ret & 0xff);
	sp -= 2;
	pc = (memory[pc + 2] << 8) | memory[pc + 1];
}

void State8080::ret()
{
	pc = memory[sp] | (memory[sp+1] << 8);
	sp += 2;
}

// AC flag, carry is set from bit 3 to bit 4, 
// so when an operation with bits under 0x10 trigger the 0x10 bit
void State8080::auxCarry(uint16_t a, uint16_t b)
{
	uint8_t low_A = a & 0xf;
	uint8_t low_B = b & 0xf;

	cc.ac = (low_A + low_B) > 0xf;
}

int State8080::parity(int x, int size)
{
	int p = 0;
	x = (x & ((1 << size) - 1));

	for(int i = 0; i < size; i++) {
		if(x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
};

void State8080::arithFlags(uint16_t res, uint8_t flags)
{
	if(flags & FLAG_Z)  cc.z = ((res & 0xff) == 0);
	if(flags & FLAG_S)  cc.s = ((res & 0x80) != 0);
	if(flags & FLAG_P)  cc.p = parity(res & 0xff);
	if(flags & FLAG_CY) cc.cy = (res > 0xff);
}
void State8080::logicFlags(reg_t reg, uint8_t flags)
{
	if(flags & FLAG_Z)  cc.z = (reg == 0);
	if(flags & FLAG_S)  cc.s = ((reg & 0x80) != 0);
	if(flags & FLAG_P)  cc.p = parity(reg & 0xff);
	if(flags & FLAG_CY) cc.cy = cc.ac = 0;
}

uint8_t& State8080::getHL()
{
	uint16_t offset = (r.h << 8) | r.l;
	return memory[offset];
}

int State8080::Emulate8080p() {

	switch(memory[pc]) {
		case 0x00: break; 		// NOP
		case 0x01: 				// LXI	B,word
			r.c = memory[pc + 1];
			r.b = memory[pc + 2];
			pc += 2;
			break;
		case 0x02: unimplementedInstruction(); break;
		case 0x03: inx(r.b, r.c); break;	// INX B
		case 0x04: inr(r.b); break;			// INR B
		case 0x05: dcr(r.b); break;			// DCR B
		case 0x06: unimplementedInstruction(); break;
		case 0x07: unimplementedInstruction(); break;
		
		case 0x08: break; // -
		case 0x09: dad((r.b << 8) | r.c); break;	// DAD B
		case 0x0A: unimplementedInstruction(); break;
		case 0x0B: dcx(r.b, r.c); break;	// DCX B
		case 0x0C: inr(r.c); break;			// INR C
		case 0x0D: dcr(r.c); break;			// DCR C
		case 0x0E: unimplementedInstruction(); break;
		case 0x0F: unimplementedInstruction(); break;

		case 0x10: break; 					// -
		case 0x11: unimplementedInstruction(); break;
		case 0x12: unimplementedInstruction(); break;
		case 0x13: inx(r.d, r.e); break;	// INX D
		case 0x14: inr(r.d); break;			// INR E
		case 0x15: dcr(r.d); break;			// DCR D
		case 0x16: unimplementedInstruction(); break;
		case 0x17: unimplementedInstruction(); break;

		case 0x18: break; 					// -
		case 0x19: dad((r.d << 8) | r.e); break;	// DAD D
		case 0x1A: unimplementedInstruction(); break;
		case 0x1B: dcx(r.d, r.e); break;	// DCX D
		case 0x1C: inr(r.e); break;			// INR E
		case 0x1D: dcr(r.e); break;			// DCR E
		case 0x1E: unimplementedInstruction(); break;
		case 0x1F: unimplementedInstruction(); break;

		case 0x20: break; 					// -
		case 0x21: unimplementedInstruction(); break;
		case 0x22: unimplementedInstruction(); break;
		case 0x23: inx(r.h, r.l); break;	// INX H
		case 0x24: inr(r.h); break;			// INR H
		case 0x25: dcr(r.h); break;			// DCR H
		case 0x26: unimplementedInstruction(); break;
		case 0x27: unimplementedInstruction(); break;

		case 0x28: break; 					// -
		case 0x29: dad((r.h << 8) | r.l); break;	// DAD H
		case 0x2A: unimplementedInstruction(); break;
		case 0x2B: dcx(r.h, r.l); break;	// DCX H
		case 0x2C: inr(r.l); break;			// INR L
		case 0x2D: dcr(r.l); break;			// DCR L
		case 0x2E: unimplementedInstruction(); break;
		case 0x2F: unimplementedInstruction(); break;

		case 0x30: break; 					// -
		case 0x31: unimplementedInstruction(); break;
		case 0x32: unimplementedInstruction(); break;
		case 0x33: sp++; break;				// INX SP
		case 0x34: inr(getHL()); break;		// INR M
		case 0x35: dcr(getHL()); break;		// DCR M
		case 0x36: unimplementedInstruction(); break;
		case 0x37: unimplementedInstruction(); break;

		case 0x38: break; 					// -
		case 0x39: dad(sp); break;			// DAD SP, quick and dirty, should work
		case 0x3A: unimplementedInstruction(); break;
		case 0x3B: sp--; break;				// DCX SP
		case 0x3C: inr(r.a); break;			// INR A
		case 0x3D: dcr(r.b); break;			// DCR A
		case 0x3E: unimplementedInstruction(); break;
		case 0x3F: unimplementedInstruction(); break;

		case 0x40: mov(r.b, r.b); break;	// MOV B,B	
		case 0x41: mov(r.b, r.c); break;	// MOV B,C
		case 0x42: mov(r.b, r.d); break;	// MOV B,D
		case 0x43: mov(r.b, r.e); break;	// MOV B,E
		case 0x44: mov(r.b, r.h); break;	// MOV B,H
		case 0x45: mov(r.b, r.l); break;	// MOV B,L
		case 0x46: mov(r.b, getHL()); break;// MOV B,HL
		case 0x47: mov(r.b, r.a); break;	// MOV B,A

		case 0x48: mov(r.c, r.b); break;	// MOV C,B	
		case 0x49: mov(r.c, r.c); break;	// MOV C,C
		case 0x4A: mov(r.c, r.d); break;	// MOV C,D
		case 0x4B: mov(r.c, r.e); break;	// MOV C,E
		case 0x4C: mov(r.c, r.h); break;	// MOV C,H
		case 0x4D: mov(r.c, r.l); break;	// MOV C,L
		case 0x4E: mov(r.c, getHL()); break;// MOV C,HL
		case 0x4F: mov(r.c, r.a); break;	// MOV C,A
		
		case 0x50: mov(r.d, r.b); break;	// MOV D,B	
		case 0x51: mov(r.d, r.c); break;	// MOV D,C
		case 0x52: mov(r.d, r.d); break;	// MOV D,D
		case 0x53: mov(r.d, r.e); break;	// MOV D,E
		case 0x54: mov(r.d, r.h); break;	// MOV D,H
		case 0x55: mov(r.d, r.l); break;	// MOV D,L
		case 0x56: mov(r.d, getHL()); break;// MOV D,HL
		case 0x57: mov(r.d, r.a); break;	// MOV D,A

		case 0x58: mov(r.e, r.b); break;	// MOV E,B	
		case 0x59: mov(r.e, r.c); break;	// MOV E,C
		case 0x5A: mov(r.e, r.d); break;	// MOV E,D
		case 0x5B: mov(r.e, r.e); break;	// MOV E,E
		case 0x5C: mov(r.e, r.h); break;	// MOV E,H
		case 0x5D: mov(r.e, r.l); break;	// MOV E,L
		case 0x5E: mov(r.e, getHL()); break;// MOV E,HL
		case 0x5F: mov(r.e, r.a); break;	// MOV E,A

		case 0x60: mov(r.h, r.b); break;	// MOV H,B	
		case 0x61: mov(r.h, r.c); break;	// MOV H,C
		case 0x62: mov(r.h, r.d); break;	// MOV H,D
		case 0x63: mov(r.h, r.e); break;	// MOV H,E
		case 0x64: mov(r.h, r.h); break;	// MOV H,H
		case 0x65: mov(r.h, r.l); break;	// MOV H,L
		case 0x66: mov(r.h, getHL()); break;// MOV H,HL
		case 0x67: mov(r.h, r.a); break;	// MOV H,A

		case 0x68: mov(r.l, r.b); break;	// MOV L,B	
		case 0x69: mov(r.l, r.c); break;	// MOV L,C
		case 0x6A: mov(r.l, r.d); break;	// MOV L,D
		case 0x6B: mov(r.l, r.e); break;	// MOV L,E
		case 0x6C: mov(r.l, r.h); break;	// MOV L,H
		case 0x6D: mov(r.l, r.l); break;	// MOV L,L
		case 0x6E: mov(r.l, getHL()); break;// MOV L,HL
		case 0x6F: mov(r.l, r.a); break;	// MOV L,A

		case 0x70: mov(getHL(), r.b); break;	// MOV HL,B	
		case 0x71: mov(getHL(), r.c); break;	// MOV HL,C
		case 0x72: mov(getHL(), r.d); break;	// MOV HL,D
		case 0x73: mov(getHL(), r.e); break;	// MOV HL,E
		case 0x74: mov(getHL(), r.h); break;	// MOV HL,H
		case 0x75: mov(getHL(), r.l); break;	// MOV HL,L
		case 0x76: unimplementedInstruction(); break; //HLT
		case 0x77: mov(getHL(), r.a); break;	// MOV C,A

		case 0x78: mov(r.a, r.b); break;	// MOV A,B	
		case 0x79: mov(r.a, r.c); break;	// MOV A,C
		case 0x7A: mov(r.a, r.d); break;	// MOV A,D
		case 0x7B: mov(r.a, r.e); break;	// MOV A,E
		case 0x7C: mov(r.a, r.h); break;	// MOV A,H
		case 0x7D: mov(r.a, r.l); break;	// MOV A,L
		case 0x7E: mov(r.a, getHL()); break;// MOV A,HL
		case 0x7F: mov(r.a, r.a); break;	// MOV A,A

		case 0x80: add(r.b); break;			// ADD B
		case 0x81: add(r.c); break;			// ADD C
		case 0x82: add(r.d); break;			// ADD D
		case 0x83: add(r.e); break;			// ADD E
		case 0x84: add(r.h); break;			// ADD H
		case 0x85: add(r.l); break;			// ADD L
		case 0x86: add(getHL()); break;		// ADD HL
		case 0x87: add(r.a); break;			// ADD A

		case 0x88: adc(r.b); break;			// ADC B		
		case 0x89: adc(r.c); break;			// ADC C
		case 0x8A: adc(r.d); break;			// ADC D
		case 0x8B: adc(r.e); break;			// ADC E
		case 0x8C: adc(r.h); break;			// ADC H
		case 0x8D: adc(r.l); break;			// ADC L
		case 0x8E: adc(getHL()); break;		// ADC HL
		case 0x8F: adc(r.a); break;			// ADC A

		case 0x90: sub(r.b); break;			// SUB B		
		case 0x91: sub(r.c); break;			// SUB C
		case 0x92: sub(r.d); break;			// SUB D
		case 0x93: sub(r.e); break;			// SUB E
		case 0x94: sub(r.h); break;			// SUB H
		case 0x95: sub(r.l); break;			// SUB L
		case 0x96: sub(getHL()); break;		// SUB HL
		case 0x97: sub(r.a); break;			// SUB A

		case 0x98: sbb(r.b); break;			// SBB B		
		case 0x99: sbb(r.c); break;			// SBB C
		case 0x9A: sbb(r.d); break;			// SBB D
		case 0x9B: sbb(r.e); break;			// SBB E
		case 0x9C: sbb(r.h); break;			// SBB H
		case 0x9D: sbb(r.l); break;			// SBB L
		case 0x9E: sbb(getHL()); break;		// SBB HL
		case 0x9F: sbb(r.a); break;			// SBB A

		case 0xA0: unimplementedInstruction(); break;
		case 0xA1: unimplementedInstruction(); break;
		case 0xA2: unimplementedInstruction(); break;
		case 0xA3: unimplementedInstruction(); break;
		case 0xA4: unimplementedInstruction(); break;
		case 0xA5: unimplementedInstruction(); break;
		case 0xA6: unimplementedInstruction(); break;
		case 0xA7: unimplementedInstruction(); break;

		case 0xA8: unimplementedInstruction(); break;
		case 0xA9: unimplementedInstruction(); break;
		case 0xAA: unimplementedInstruction(); break;
		case 0xAB: unimplementedInstruction(); break;
		case 0xAC: unimplementedInstruction(); break;
		case 0xAD: unimplementedInstruction(); break;
		case 0xAE: unimplementedInstruction(); break;
		case 0xAF: unimplementedInstruction(); break;

		case 0xB0: unimplementedInstruction(); break;
		case 0xB1: unimplementedInstruction(); break;
		case 0xB2: unimplementedInstruction(); break;
		case 0xB3: unimplementedInstruction(); break;
		case 0xB4: unimplementedInstruction(); break;
		case 0xB5: unimplementedInstruction(); break;
		case 0xB6: unimplementedInstruction(); break;
		case 0xB7: unimplementedInstruction(); break;

		case 0xB8: unimplementedInstruction(); break;
		case 0xB9: unimplementedInstruction(); break;
		case 0xBA: unimplementedInstruction(); break;
		case 0xBB: unimplementedInstruction(); break;
		case 0xBC: unimplementedInstruction(); break;
		case 0xBD: unimplementedInstruction(); break;
		case 0xBE: unimplementedInstruction(); break;
		case 0xBF: unimplementedInstruction(); break;

		case 0xC0: if(cc.z == 0) ret(); else pc += 2; break;	// RNZ 
		case 0xC1: unimplementedInstruction(); break;
		case 0xC2: if(cc.z == 0) jmp(); else pc += 2; break;	// JNZ address
		case 0xC3: jmp(); break;								// JMP address
		case 0xC4: if(cc.z == 0) call(); else pc+=2; break;		// CNZ address
		case 0xC5: unimplementedInstruction(); break;
		case 0xC6: add(memory[pc + 1]);	break;					// ADI byte
		case 0xC7: rst(0x0); break;								// RST 0

		case 0xC8: if(cc.z != 0) ret(); else pc += 2; break;	// RZ
		case 0xC9: ret(); break;								// RET
		case 0xCA: if(cc.z != 0) jmp(); else pc += 2; break;	// JZ address
		case 0xCB: unimplementedInstruction(); break;
		case 0xCC: if(cc.z != 0) call(); else pc += 2; break;	// CZ address
		case 0xCD: call(); break;								// CALL address
		case 0xCE: adc(memory[pc + 1]); break; 					// ACY byte
		case 0xCF: rst(0x8); break;								// RST 1

		case 0xD0: if(cc.cy == 0) ret(); else pc += 2; break;	// RNC
		case 0xD1: unimplementedInstruction(); break;
		case 0xD2: if(cc.cy == 0) jmp(); else pc += 2; break;	// JNC address
		case 0xD3: unimplementedInstruction(); break;
		case 0xD4: if(cc.cy == 0) call(); else pc += 2; break;	// CNC address
		case 0xD5: unimplementedInstruction(); break;
		case 0xD6: sub(memory[pc + 1]); break;					// SUI byte
		case 0xD7: rst(0x10); break;							// RST 2

		case 0xD8: if(cc.cy != 0) ret(); else pc += 2; break;	// RC
		case 0xD9: unimplementedInstruction(); break;
		case 0xDA: if(cc.cy != 0) jmp(); else pc += 2; break;	// JC address
		case 0xDB: unimplementedInstruction(); break;
		case 0xDC: if(cc.cy != 0) call(); else pc += 2; break;	// CC address
		case 0xDD: unimplementedInstruction(); break;
		case 0xDE: sbb(memory[pc + 1]); break; 					// SBI byte
		case 0xDF: rst(0x18); break;							// RST 3
		
		case 0xE0: if(cc.p == 0) ret(); else pc += 2; break;	// RPO
		case 0xE1: unimplementedInstruction(); break;
		case 0xE2: if(cc.p == 0) jmp(); else pc += 2; break;	// JPO address (parity odd, not set)
		case 0xE3: unimplementedInstruction(); break;
		case 0xE4: if(cc.p == 0) call(); else pc += 2; break;	// CPO address
		case 0xE5: unimplementedInstruction(); break;
		case 0xE6: unimplementedInstruction(); break;
		case 0xE7: rst(0x20); break;							// RST 4
		
		case 0xE8: if(cc.p != 0) ret(); else pc += 2; break;	// RPE
		case 0xE9: jmp(r.h, r.l); break;						// PCHL
		case 0xEA: if(cc.p != 0) jmp(); else pc += 2; break;	// JPE address (parity even, set)
		case 0xEB: unimplementedInstruction(); break;
		case 0xEC: if(cc.p != 0) call(); else pc += 2; break;	// CPE address
		case 0xED: unimplementedInstruction(); break;
		case 0xEE: unimplementedInstruction(); break;
		case 0xEF: rst(0x28); break;							// RST 5
		
		case 0xF0: if(cc.s == 0) ret(); else pc += 2; break;	// RP
		case 0xF1: unimplementedInstruction(); break;
		case 0xF2: if(cc.s == 0) jmp(); else pc += 2; break;	// JP address (plus)
		case 0xF3: unimplementedInstruction(); break;
		case 0xF4: if(cc.s == 0) call(); else pc += 2; break;	// CP address
		case 0xF5: unimplementedInstruction(); break;
		case 0xF6: unimplementedInstruction(); break;
		case 0xF7: rst(0x30); break;							// RST 6
		
		case 0xF8: if(cc.p != 0) ret(); else pc += 2; break;	// RM
		case 0xF9: unimplementedInstruction(); break;
		case 0xFA: if(cc.s != 0) jmp(); else pc += 2; break;	// JM address (minus)
		case 0xFB: unimplementedInstruction(); break;
		case 0xFC: if(cc.s != 0) call(); else pc += 2; break;	// CM address
		case 0xFD: unimplementedInstruction(); break;
		case 0xFE: unimplementedInstruction(); break;
		case 0xFF: rst(0x38); break;							// RST 7
	}

}

}