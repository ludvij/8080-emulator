#pragma once
#include <cstdint>
#include <vector>

namespace p8080 {

typedef std::vector<uint8_t> Memory;
typedef uint8_t reg_t;


constexpr uint8_t FLAG_Z  =  0b00000001;
constexpr uint8_t FLAG_S  =  0b00000010;
constexpr uint8_t FLAG_P  =  0b00000100;
constexpr uint8_t FLAG_CY =  0b00001000;
constexpr uint8_t FLAG_AC =  0b00010000;
constexpr uint8_t FLAG_ALL=	 0b00011111;

struct ConditionCodes8080 {
	uint8_t z : 1;
	uint8_t s : 1;
	uint8_t p : 1;
	uint8_t cy: 1;
	uint8_t ac: 1;
	uint8_t   : 3;
};

struct registers8080 {
	reg_t a;
	reg_t b;
	reg_t c;
	reg_t d;
	reg_t e;
	reg_t h;
	reg_t l;
};


class State8080 {
public:
	registers8080 r;
	uint16_t sp;
	uint16_t pc;
	Memory memory;
	ConditionCodes8080 cc;
	uint8_t int_enable;

public:
	int Emulate8080p();
private:
	void unimplementedInstruction();

	// arithmetic
	void inx(reg_t& r1, reg_t& r2);
	void inr(uint8_t& r);
	void dcr(reg_t& r);
	void dcx(reg_t& r1, reg_t& r2);
	void add(uint8_t other);
	void adc(uint8_t other);
	void sub(uint8_t other);
	void sbb(uint8_t other);
	void arithFlags(uint16_t res, uint8_t flags);

	// data
	void mov(uint8_t& r1, uint8_t r2);
	void stax(uint8_t r1, uint8_t r2);
	void lxi(uint8_t& r1, uint8_t& r2);
	void ldax(uint8_t r1, uint8_t r2);

	// logic
	void ana(uint8_t other);
	void ora(uint8_t other);
	void xra(uint8_t other);
	void cmp(uint8_t other);
	void dad(uint16_t rp);
	void logicFlags(reg_t reg, uint8_t flags);

	// branch
	void jmp(uint8_t hi, uint8_t lo);
	void jmp(bool cond);
	void call(bool cond);
	void ret(bool cond);
	void rst(uint16_t address);

	// stack
	void pop(uint8_t& r1, uint8_t& r2);
	void push(uint8_t r1, uint8_t r2);
	void popPSW();
	void pushPSW();
	void xthl();
	void xchg();
	
	// flag helpers
	int parity(int x, int size=8);
	void auxCarry(uint16_t a, uint16_t b);


	// get memory pos of HL
	uint8_t& getHL();

	uint8_t& getNextByte();
	uint16_t getNextAddress();

};

}