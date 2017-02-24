/* File: fileParser.c
*  Contains structs for all of the constant machine code values
*  associated to components used in MIPS.
*/



//Struct array that stores each number associated  to each register
struct {const char* name, int address} registerNum[] = 
	{
		{ "zero", 00000},
		{ "a0", 00100 },
		{ "a1", 00101 },
		{ "a2", 00110 },
		{ "a3", 00111 },
		{ "t0", 01000 },
		{ "t1", 01001 },
		{ "t2", 01010 },
		{ "t3", 01011 },
		{ "t4", 01100 },
		{ "t5", 01101 },
		{ "t6", 01110 },
		{ "t7", 01111 },
		{ "s0", 10000 },
		{ "s1", 10001 },
		{ "s2", 10010 },
		{ "s3", 10011 },
		{ "s4", 10100 },
		{ "s5", 10101 },
		{ "s6", 10110 },
		{ "s7", 10111 },
		{ "t8", 11000 },
		{ "t9", 11001 },
		{ NULL, 0 } 
		
	};

//Struct array that contains the R-format instruction opcode functions	
struct {const char* name, int opcode} rTypeInstruct[] = 
	{
		{"add",100000},
		{"jr",001000},
		{"slt",101010},
		{"sub",100010},
		{ NULL, 0}
	};
	
//Struct array that contains the I-format instruction opcode functions
struct {const char* name, int opcode} iTypeInstruct[] =
	{
		{"addi",001000},
		{"beq",000100},
		{"bne",000101},
		{"lw",100011},
		{"slti",001010},
		{"sw",101011},		
		{ NULL, 0 } 
	};

//Stryct array that contains the J-format instruction opcode functions
struct {const char* name, int opcode} jTypeInstruct[] =
	{
		{"j",000010},
		{"jal",000011},
		{ NULL, 0 }
	};	
	