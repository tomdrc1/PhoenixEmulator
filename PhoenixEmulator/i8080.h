#pragma once
typedef unsigned char byte;

typedef struct ConditionCodes {
	byte cy : 1;
	byte pad1 : 1;
	byte p : 1;
	byte pad2 : 1;
	byte ac : 1;
	byte pad3 : 1;
	byte z : 1;
	byte s : 1;
} ConditionCodes;

typedef struct State8080 {
	byte a;
	byte b;
	byte c;
	byte d;
	byte e;
	byte h;
	byte l;
	unsigned short sp;
	unsigned short pc;
	byte* memory;
	ConditionCodes cc;
	byte int_enable;
	unsigned int cycles;
} State8080;


static const byte OPCODES_CYCLES[] = {
	//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7,  4,  // 0
		4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7,  4,  // 1
		4,  10, 16, 5,  5,  5,  7,  4,  4,  10, 16, 5,  5,  5,  7,  4,  // 2
		4,  10, 13, 5,  10, 10, 10, 4,  4,  10, 13, 5,  5,  5,  7,  4,  // 3
		5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 4
		5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 5
		5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 6
		7,  7,  7,  7,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  7,  5,  // 7
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 8
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 9
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // A
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // B
		5,  10, 10, 10, 11, 11, 7,  11, 5,  10, 10, 10, 11, 11, 7,  11, // C
		5,  10, 10, 10, 11, 11, 7,  11, 5,  10, 10, 10, 11, 11, 7,  11, // D
		5,  10, 10, 18, 11, 11, 7,  11, 5,  5,  10, 5,  11, 11, 7,  11, // E
		5,  10, 10, 4,  11, 11, 7,  11, 5,  5,  10, 4,  11, 11, 7,  11  // F
};

byte pairtyCheck(byte num, byte size);
byte emulate8080Op(State8080* state);
void add(State8080* state, byte r);
void adc(State8080* state, byte r);
void sub(State8080* state, byte r);
void sbb(State8080* state, byte r);
void ana(State8080* state, byte r);
void xra(State8080* state, byte r);
void ora(State8080* state, byte r);
void cmp(State8080* state, byte r);
void push(State8080* state, byte high, byte low);
void pop(State8080* state, byte* high, byte* low);
void retConditional(State8080* state, byte Conditional);
void callConditional(State8080* state, byte Conditional, unsigned short adr);
void jmpConditional(State8080* state, byte Conditional, unsigned short adr);
void writeToMemory(State8080* state, unsigned short adr, byte value);
void generateInterrupt(State8080* state, byte interrupt_num);