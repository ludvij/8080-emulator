#include "8080.h"
#include <iostream>
#include <stdexcept>

namespace p8080 {

void State8080::unimplementedInstruction() {
	pc--;
	std::cerr << "ERROR: Unimplemented instruction" << std::endl;
	throw std::runtime_error("Unimplemented instruction");
}


void State8080::lxi(uint8_t& r1, uint8_t& r2)
{
	r2 = getNextByte();
	r1 = getNextByte();
}

void State8080::stax(uint8_t r1, uint8_t r2)
{
	memory[(r1 << 8) | r2] = r.a;
}

void State8080::ldax(uint8_t r1, uint8_t r2)
{
	memory[(r1 << 8) | r2] = r.a;
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
	arithFlags(r, FLAG_ALL ^ FLAG_CY);
}
void State8080::inr(reg_t& r)
{
	// aux carry
	cc.ac = 0;
	// ALL FLAGS MINUS CARRY
	arithFlags(r, FLAG_ALL ^ FLAG_CY);
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
void State8080::ana(uint8_t other)
{
	r.a = r.a & other;
	logicFlags(r.a, FLAG_ALL);
}
void State8080::ora(uint8_t other)
{
	r.a = r.a | other;
	logicFlags(r.a, FLAG_ALL);
}
void State8080::xra(uint8_t other)
{
	r.a = r.a ^ other;
	logicFlags(r.a, FLAG_ALL);
}
void State8080::cmp(uint8_t other)
{
	uint8_t x = r.a - other;
	logicFlags(x, FLAG_ALL ^ FLAG_CY);
	cc.cy = r.a < other;
	cc.ac = 0;
}

void State8080::jmp(uint8_t hi, uint8_t lo)
{
	pc = static_cast<uint16_t>((hi << 8) | lo);
}
void State8080::jmp(bool cond)
{
	uint8_t lo = getNextByte();
	uint8_t hi = getNextByte();
	if (cond)
		jmp(hi, lo);
}

void State8080::rst(uint16_t address)
{
	uint16_t ret = pc + 2;
	uint8_t lo = static_cast<uint8_t>(ret & 0xff);
	uint8_t hi = static_cast<uint8_t>(ret >> 8);
	push(hi, lo);
	pc = address;
}
void State8080::call(bool cond)
{
	uint16_t addr = getNextAddress();
	if (cond) {	// if it doesn't happen pc + 2
		uint8_t lo = static_cast<uint8_t>(pc & 0xff);
		uint8_t hi = static_cast<uint8_t>(pc >> 8);
		push(hi, lo);
		pc = addr;
	}
}

void State8080::ret(bool cond)
{
	if (cond) {
		pc = memory[sp] | (memory[sp+1] << 8);
		sp += 2;
	}
}

void State8080::pop(uint8_t& r1, uint8_t& r2)
{
	r2 = memory[sp];
	r1 = memory[sp + 1];
	sp += 2;
}

void State8080::push(uint8_t r1, uint8_t r2)
{
	memory[sp - 1] = r2;
	memory[sp - 2] = r1;
	sp -= 2;
}

void State8080::popPSW()
{
	r.a = memory[sp+1];
	uint8_t psw = memory[sp];
	cc.z  = (0x01 == (psw & 0x01));
	cc.s  = (0x02 == (psw & 0x02));
	cc.p  = (0x04 == (psw & 0x04));
	cc.cy = (0x08 == (psw & 0x08));	// check this in tutorial repo
	cc.ac = (0x10 == (psw & 0x10));
	sp += 2;
}

void State8080::pushPSW()
{
	memory[sp-1] = r.a;
	uint8_t psw = (cc.z | cc.s << 1 | cc.p << 2 | cc.cy << 3 | cc.ac << 4 );
	memory[sp-2] = psw;
	sp = sp - 2;
}

void State8080::xthl()
{
	uint8_t exchange = r.l;
	r.l = memory[sp];
	memory[sp] = exchange;

	exchange = r.h;
	r.h = memory[sp + 1];
	memory[sp + 1] = exchange;

	sp++;
}

void State8080::xchg()
{
	// (H) <-> (D)
	// (L) <-> (E)
	uint8_t exchange = r.h;
	r.h = r.d;
	r.d = exchange;

	exchange = r.l;
	r.l = r.e;
	r.e = exchange;
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
 	// data book says these are zeroed
	if(flags & FLAG_CY) cc.cy = cc.ac = 0;
}

uint8_t& State8080::getHL()
{
	uint16_t offset = (r.h << 8) | r.l;
	return memory[offset];
}

uint8_t& State8080::getNextByte()
{
	return memory[++pc];
}

uint16_t State8080::getNextAddress()
{
	uint8_t lo = getNextByte();
	uint8_t hi = getNextByte();
	return (hi << 8) | lo;
}



int State8080::Emulate8080p() {

	switch(memory[pc]) {
		case 0x00: break; // NOP
		case 0x01: lxi(r.b, r.c); // LXI B, WORD
		case 0x02: stax(r.b, r.c); break; //STAX B
		case 0x03: inx(r.b, r.c); break; // INX B
		case 0x04: inr(r.b); break; // INR B
		case 0x05: dcr(r.b); break; // DCR B
		case 0x06: mov(r.b, getNextByte()); break; // MVI B, BYTE
		case 0x07: // RLC
			uint8_t x = r.a;
			r.a = ((x & 0x80) >> 7) | (x << 1);
			cc.cy = 1 == (x & 0x80);
			break;
		case 0x08: break; // -
		case 0x09: dad((r.b << 8) | r.c); break; // DAD B
		case 0x0A: ldax(r.b, r.c); break; // LDAX B
		case 0x0B: dcx(r.b, r.c); break; // DCX B
		case 0x0C: inr(r.c); break; // INR C
		case 0x0D: dcr(r.c); break; // DCR C
		case 0x0E: mov(r.c, getNextByte()); break; // MVI C, BYTE
		case 0x0F: // RRC
			uint8_t x = r.a;
			r.a = ((x & 1) << 7) | (x >> 1);
			cc.cy = (1 == (x & 1));
			break;
		case 0x10: break; // -
		case 0x11: lxi(r.d, r.e); break; // LXI D, WORD
		case 0x12: stax(r.d, r.e); break; // STAX D
		case 0x13: inx(r.d, r.e); break; // INX D
		case 0x14: inr(r.d); break; // INR E
		case 0x15: dcr(r.d); break; // DCR D
		case 0x16: mov(r.d, getNextByte()); break; // MVI D, BYTE
		case 0x17: // RAL
			uint8_t x = r.a;
			r.a = (x << 1) | cc.cy ;
			cc.cy = (1 == (x&0x80));
			break;

		case 0x18: break; // -
		case 0x19: dad((r.d << 8) | r.e); break; // DAD D
		case 0x1A: ldax(r.d, r.e); break; // LDAX D
		case 0x1B: dcx(r.d, r.e); break; // DCX D
		case 0x1C: inr(r.e); break; // INR E
		case 0x1D: dcr(r.e); break; // DCR E
		case 0x1E: mov(r.e, getNextByte()); break; // MVI E
		case 0x1F: //RAR
			uint8_t x = r.a;
			r.a = (cc.cy << 7) | (x >> 1);
			cc.cy = (1 == (x&1));
			break;

		case 0x20: break; // -
		case 0x21: lxi(r.h, r.l); break; // LXI H, WORD
		case 0x22: // SHLD H
			uint16_t addr = getNextAddress();
			memory[addr] = r.l;
			memory[addr + 1] = r.h;
			break;
		case 0x23: inx(r.h, r.l); break; // INX H
		case 0x24: inr(r.h); break; // INR H
		case 0x25: dcr(r.h); break; // DCR H
		case 0x26: mov(r.h, getNextByte()); break; // MVI H, BYTE
			break;
		case 0x27: unimplementedInstruction(); break; // DAA

		case 0x28: break; // -
		case 0x29: dad((r.h << 8) | r.l); break; // DAD H
		case 0x2A: // LHLD
			uint16_t address = getNextAddress();
			r.l = memory[address];
			r.h = memory[address + 1];
			break;
		case 0x2B: dcx(r.h, r.l); break; // DCX H
		case 0x2C: inr(r.l); break; // INR L
		case 0x2D: dcr(r.l); break; // DCR L
		case 0x2E: mov(r.l, getNextByte()); break; // MVI L, BYTE
		case 0x2F: r.a = ~r.a; break; // CMA

		case 0x30: break; // -
		case 0x31: sp = getNextAddress(); break; // LXI SP, WORD
		case 0x32: ; break;
		case 0x33: sp++; break; // INX SP
		case 0x34: inr(getHL()); break; // INR M
		case 0x36: mov(getHL(), getNextByte()); break; // MVI M, BYTE
		case 0x35: dcr(getHL()); break; // DCR M
		case 0x37: cc.cy = 1; break; // STC

		case 0x38: break; // -
		case 0x39: dad(sp); break; // DAD SP, quick and dirty, should work
		case 0x3A: r.a = memory[getNextAddress()]; break; // LDA adr
		case 0x3B: sp--; break; // DCX SP
		case 0x3C: inr(r.a); break; // INR A
		case 0x3D: dcr(r.b); break; // DCR A
		case 0x3E: mov(r.a, getNextByte()); break; // MVI A, BYTE
		case 0x3F: cc.cy = ~cc.cy; break; // CMC

		case 0x40: mov(r.b, r.b); break; // MOV B,B
		case 0x41: mov(r.b, r.c); break; // MOV B,C
		case 0x42: mov(r.b, r.d); break; // MOV B,D
		case 0x43: mov(r.b, r.e); break; // MOV B,E
		case 0x44: mov(r.b, r.h); break; // MOV B,H
		case 0x45: mov(r.b, r.l); break; // MOV B,L
		case 0x46: mov(r.b, getHL()); break;// MOV B,HL
		case 0x47: mov(r.b, r.a); break; // MOV B,A

		case 0x48: mov(r.c, r.b); break; // MOV C,B
		case 0x49: mov(r.c, r.c); break; // MOV C,C
		case 0x4A: mov(r.c, r.d); break; // MOV C,D
		case 0x4B: mov(r.c, r.e); break; // MOV C,E
		case 0x4C: mov(r.c, r.h); break; // MOV C,H
		case 0x4D: mov(r.c, r.l); break; // MOV C,L
		case 0x4E: mov(r.c, getHL()); break;// MOV C,HL
		case 0x4F: mov(r.c, r.a); break; // MOV C,A

		case 0x50: mov(r.d, r.b); break; // MOV D,B
		case 0x51: mov(r.d, r.c); break; // MOV D,C
		case 0x52: mov(r.d, r.d); break; // MOV D,D
		case 0x53: mov(r.d, r.e); break; // MOV D,E
		case 0x54: mov(r.d, r.h); break; // MOV D,H
		case 0x55: mov(r.d, r.l); break; // MOV D,L
		case 0x56: mov(r.d, getHL()); break;// MOV D,HL
		case 0x57: mov(r.d, r.a); break; // MOV D,A

		case 0x58: mov(r.e, r.b); break; // MOV E,B
		case 0x59: mov(r.e, r.c); break; // MOV E,C
		case 0x5A: mov(r.e, r.d); break; // MOV E,D
		case 0x5B: mov(r.e, r.e); break; // MOV E,E
		case 0x5C: mov(r.e, r.h); break; // MOV E,H
		case 0x5D: mov(r.e, r.l); break; // MOV E,L
		case 0x5E: mov(r.e, getHL()); break;// MOV E,HL
		case 0x5F: mov(r.e, r.a); break; // MOV E,A

		case 0x60: mov(r.h, r.b); break; // MOV H,B
		case 0x61: mov(r.h, r.c); break; // MOV H,C
		case 0x62: mov(r.h, r.d); break; // MOV H,D
		case 0x63: mov(r.h, r.e); break; // MOV H,E
		case 0x64: mov(r.h, r.h); break; // MOV H,H
		case 0x65: mov(r.h, r.l); break; // MOV H,L
		case 0x66: mov(r.h, getHL()); break;// MOV H,HL
		case 0x67: mov(r.h, r.a); break; // MOV H,A

		case 0x68: mov(r.l, r.b); break; // MOV L,B
		case 0x69: mov(r.l, r.c); break; // MOV L,C
		case 0x6A: mov(r.l, r.d); break; // MOV L,D
		case 0x6B: mov(r.l, r.e); break; // MOV L,E
		case 0x6C: mov(r.l, r.h); break; // MOV L,H
		case 0x6D: mov(r.l, r.l); break; // MOV L,L
		case 0x6E: mov(r.l, getHL()); break;// MOV L,HL
		case 0x6F: mov(r.l, r.a); break; // MOV L,A

		case 0x70: mov(getHL(), r.b); break; // MOV M,B
		case 0x71: mov(getHL(), r.c); break; // MOV M,C
		case 0x72: mov(getHL(), r.d); break; // MOV M,D
		case 0x73: mov(getHL(), r.e); break; // MOV M,E
		case 0x74: mov(getHL(), r.h); break; // MOV M,H
		case 0x75: mov(getHL(), r.l); break; // MOV M,L
		case 0x76: std::exit(0); break; // HLT, lol
		case 0x77: mov(getHL(), r.a); break; // MOV C,A

		case 0x78: mov(r.a, r.b); break; // MOV A,B
		case 0x79: mov(r.a, r.c); break; // MOV A,C
		case 0x7A: mov(r.a, r.d); break; // MOV A,D
		case 0x7B: mov(r.a, r.e); break; // MOV A,E
		case 0x7C: mov(r.a, r.h); break; // MOV A,H
		case 0x7D: mov(r.a, r.l); break; // MOV A,L
		case 0x7E: mov(r.a, getHL()); break; // MOV A,HL
		case 0x7F: mov(r.a, r.a); break;// MOV A,A

		case 0x80: add(r.b); break; // ADD B
		case 0x81: add(r.c); break; // ADD C
		case 0x82: add(r.d); break; // ADD D
		case 0x83: add(r.e); break; // ADD E
		case 0x84: add(r.h); break; // ADD H
		case 0x85: add(r.l); break; // ADD L
		case 0x86: add(getHL()); break; // ADD M
		case 0x87: add(r.a); break; // ADD A

		case 0x88: adc(r.b); break; // ADC B
		case 0x89: adc(r.c); break; // ADC C
		case 0x8A: adc(r.d); break; // ADC D
		case 0x8B: adc(r.e); break; // ADC E
		case 0x8C: adc(r.h); break; // ADC H
		case 0x8D: adc(r.l); break; // ADC L
		case 0x8E: adc(getHL()); break; // ADC M
		case 0x8F: adc(r.a); break; // ADC A

		case 0x90: sub(r.b); break; // SUB B
		case 0x91: sub(r.c); break; // SUB C
		case 0x92: sub(r.d); break; // SUB D
		case 0x93: sub(r.e); break; // SUB E
		case 0x94: sub(r.h); break; // SUB H
		case 0x95: sub(r.l); break; // SUB L
		case 0x96: sub(getHL()); break; // SUB M
		case 0x97: sub(r.a); break; // SUB A

		case 0x98: sbb(r.b); break; // SBB B
		case 0x99: sbb(r.c); break; // SBB C
		case 0x9A: sbb(r.d); break; // SBB D
		case 0x9B: sbb(r.e); break; // SBB E
		case 0x9C: sbb(r.h); break; // SBB H
		case 0x9D: sbb(r.l); break; // SBB L
		case 0x9E: sbb(getHL()); break; // SBB M
		case 0x9F: sbb(r.a); break; // SBB A

		case 0xA0: ana(r.b); break; // ANA B
		case 0xA1: ana(r.c); break; // ANA C
		case 0xA2: ana(r.d); break; // ANA D
		case 0xA3: ana(r.e); break; // ANA E
		case 0xA4: ana(r.l); break; // ANA L
		case 0xA5: ana(r.h); break; // ANA H
		case 0xA6: ana(getHL()); break; // ANA M
		case 0xA7: ana(r.a); break; // ANA A

		case 0xA8: xra(r.b); break; // XRA B
		case 0xA9: xra(r.c); break; // XRA C
		case 0xAA: xra(r.d); break; // XRA D
		case 0xAB: xra(r.e); break; // XRA E
		case 0xAC: xra(r.l); break; // XRA L
		case 0xAD: xra(r.h); break; // XRA H
		case 0xAE: xra(getHL()); break; // XRA M
		case 0xAF: xra(r.a); break; // XRA A

		case 0xB0: ora(r.b); break; // ORA B
		case 0xB1: ora(r.c); break; // ORA C
		case 0xB2: ora(r.d); break; // ORA D
		case 0xB3: ora(r.e); break; // ORA E
		case 0xB4: ora(r.l); break; // ORA L
		case 0xB5: ora(r.h); break; // ORA H
		case 0xB6: ora(getHL()); break; // ORA M
		case 0xB7: ora(r.a); break; // ORA A

		case 0xB8: cmp(r.b); break; // CMP B
		case 0xB9: cmp(r.c); break; // CMP C
		case 0xBA: cmp(r.d); break; // CMP D
		case 0xBB: cmp(r.e); break; // CMP E
		case 0xBC: cmp(r.h); break; // CMP H
		case 0xBD: cmp(r.l); break; // CMP L
		case 0xBE: cmp(getHL()); break; // CMP M
		case 0xBF: cmp(r.a); break; // CMP A

		case 0xC0: ret(cc.z == 0); break;  						// RNZ
		case 0xC1: pop(r.b, r.c); break;						// POP B
		case 0xC2: jmp(cc.z == 0); break;						// JNZ address
		case 0xC3: jmp(true); break;							// JMP address
		case 0xC4: call(cc.z == 0); break;  					// CNZ address
		case 0xC5: push(r.b, r.c); break;						// PUSH B
		case 0xC6: add(getNextByte()); break;					// ADI byte
		case 0xC7: rst(0x0); break;								// RST 0

		case 0xC8: ret(cc.z != 0); break;						// RZ
		case 0xC9: ret(true); break;							// RET
		case 0xCA: jmp(cc.z != 0); break;	  					// JZ address
		case 0xCB: break;										//
		case 0xCC: call(cc.z != 0); break;  					// CZ address
		case 0xCD: call(true); break;							// CALL address
		case 0xCE: adc(getNextByte()); break;					// ACY byte
		case 0xCF: rst(0x8); break;								// RST 1

		case 0xD0: ret(cc.cy == 0); break;						// RNC
		case 0xD1: pop(r.d, r.e); break;						// POP D
		case 0xD2: jmp(cc.cy == 0); break;						// JNC address
		case 0xD3: /* NOT YET IMPLEMENTED */; pc++; break;		// OUT
		case 0xD4: call(cc.cy == 0); break; 					// CNC address
		case 0xD5: push(r.d, r.e); break;						// PUSH D
		case 0xD6: sub(getNextByte()); break;					// SUI byte
		case 0xD7: rst(0x10); break;							// RST 2

		case 0xD8: ret(cc.cy != 0); break;						// RC
		case 0xD9: break;										// -
		case 0xDA: jmp(cc.cy != 0); break;						// JC address
		case 0xDB: /* NOT YET IMPLEMENTED */; pc++; break;		// IN
		case 0xDC: call(cc.cy != 0); break; 					// CC address
		case 0xDD: break;										// -
		case 0xDE: sbb(getNextByte()); break;					// SBI byte
		case 0xDF: rst(0x18); break;							// RST 3

		case 0xE0: ret(cc.p == 0); break;						// RPO
		case 0xE1: pop(r.h, r.l); break;						// POP H
		case 0xE2: jmp(cc.p == 0); break;						// JPO address (parity odd, not set)
		case 0xE3: xthl(); break;								// XTHL
		case 0xE4: call(cc.p == 0); break;						// CPO address
		case 0xE5: push(r.h, r.l); break;						// PUSH H
		case 0xE6: ana(getNextByte()); break;					// ANI byte
		case 0xE7: rst(0x20); break;							// RST 4

		case 0xE8: ret(cc.p != 0); break;						// RPE
		case 0xE9: jmp(r.h, r.l); break;						// PCHL
		case 0xEA: jmp(cc.p != 0); break;					  	// JPE address (parity even, set)
		case 0xEB: xchg(); break;								// XCHG
		case 0xEC: call(cc.p != 0); break;  					// CPE address
		case 0xED: break;										// -
		case 0xEE: xra(getNextByte()); break;					// XRI byte
		case 0xEF: rst(0x28); break;							// RST 5

		case 0xF0: ret(cc.s == 0); break;						// RP
		case 0xF1: popPSW(); break;								// POP PSW
		case 0xF2: jmp(cc.s == 0); break;						// JP address (plus)
		case 0xF3: int_enable = 0; break;						// DI
		case 0xF4: call(cc.s == 0); break;						// CP address
		case 0xF5: pushPSW(); break;							// PUSH PSW
		case 0xF6: ora(getNextByte()); break;					// ORI byte
		case 0xF7: rst(0x30); break;							// RST 6

		case 0xF8: ret(cc.p != 0); break;						// RM
		case 0xF9: sp = (r.h << 8) | r.l; break;				// SPHL
		case 0xFA: jmp(cc.s != 0); break; 						// JM address (minus)
		case 0xFB: int_enable = 1; break;						// EI
		case 0xFC: call(cc.s != 0); break;						// CM address
		case 0xFD: break;										// -
		case 0xFE: cmp(getNextByte()); break;					// CPI byte
		case 0xFF: rst(0x38); break;							// RST 7
	}
	pc++;
}

}