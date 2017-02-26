

/* PROJECT: MIPS Assembler for CSCI361
 * 
 * FILE: mipsAssembler.cpp
 *
 * EXECUTION ENVIRONMENTS
 * Montana Tech Katie server
 *
 * HISTORY
 * Date			Author			Description
 * ====			======			===========
 * 02/23/17	Max Austin & Alex Reid  	Implemented mipsData class
 * 02/24/17	Max Austin & Alex Reid		Changed values to hexadecimal, easier
 *
 * DESCRIPTION
 * Contains structs for all of the constant machine code 
 * values associated to components used in MIPS.
 */

#include <stdio.h>


//Struct array that stores each number associated  to each register

struct Reg{

	const char* name;
	int regNum;
};

typedef struct Reg Reg;

Reg registerNum[] = 
	{
		{ "zero", 0x00},
		{ "a0", 0x4},
		{ "a1", 0x5},
		{ "a2", 0x6},
		{ "a3", 0x7},
		{ "t0", 0x8},
		{ "t1", 0x9},
		{ "t2", 0xA},
		{ "t3", 0xB},
		{ "t4", 0xC},
		{ "t5", 0xD},
		{ "t6", 0xE},
		{ "t7", 0xF},
		{ "s0", 0x10},
		{ "s1", 0x11},
		{ "s2", 0x12},
		{ "s3", 0x13},
		{ "s4", 0x14},
		{ "s5", 0x15},
		{ "s6", 0x16},
		{ "s7", 0x17},
		{ NULL, 0 } 
		
	};

//Struct array that contains the R-format instruction opcode functions	

struct rFormat{

	const char* name; 
	int opcode;
}; 

typedef struct rFormat rFormat;

rFormat rTypeInstruct[] = 
	{
		{"add",0x20},
		{"jr",0x08},
		{"slt",0x2A},
		{"sub",0x22},
		{ NULL, 0}
	};
	
//Struct array that contains the I-format instruction opcode functions

struct iFormat{

	const char* name; 
	int opcode;
}; 

typedef struct iFormat iFormat;

iFormat iTypeInstruct[] =
	{
		{"addi",0x8},
		{"beq",0x4},
		{"bne",0x5},
		{"lw",0x23},
		{"slti",0xA},
		{"sw",0x2B},		
		{ NULL, 0 } 
	};

//Stryct array that contains the J-format instruction opcode functions

struct jFormat{

	const char* name;
	int opcode;
}; 

typedef struct jFormat jFormat;

jFormat jTypeInstruct[] =
	{
		{"j",0x2},
		{"jal",0x3},
		{ NULL, 0 }
	};	
	
