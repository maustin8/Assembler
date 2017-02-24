

/*
 * The structs below map a character to an integer.
 * They are used in order to map a specific instruciton/register to its binary format in ASCII
 */

// Struct that stores registers and their respective binary reference
struct { const char *name; char *address;} registerMap[] 
= 	{
		{ "zero", "00000" },
		{ "a0", "00100" },
		{ "a1", "00101" },
		{ "a2", "00110" },
		{ "a3", "00111" },
		{ "t0", "01000" },
		{ "t1", "01001" },
		{ "t2", "01010" },
		{ "t3", "01011" },
		{ "t4", "01100" },
		{ "t5", "01101" },
		{ "t6", "01110" },
		{ "t7", "01111" },
		{ "t8", "11000" },
		{ "t9", "11001" },
		{ "s0", "10000" },
		{ "s1", "10001" },
		{ "s2", "10010" },
		{ "s3", "10011" },
		{ "s4", "10100" },
		{ "s5", "10101" },
		{ "s6", "10110" },
		{ "s7", "10111" },
		{ NULL, 0 } 
	};

//INSTRUCTIONS

// Struct for R-Type instructions mapping for the 'function' field in the instruction
struct { const char *name; char *function;} rMap[]
=	{
		{ "add", "100000" },
		{ "sub", "100001" },
		{ "slt", "101010" },
		{ "jr",  "001000" },
		{ NULL, 0 } 
	};
	
// Struct for I-Type instructions
struct { const char *name; char *address;} iMap[] 
=	{
		{ "lw",   "100011" },
		{ "sw",   "101011" },
		{ "beq",  "000100" },
		{ "bne",  "000101" },
		{ "slti", "001010" },
		{ "addi", "001000" },
		{ NULL, 0 } 
	};
	
// Struct for J-Type instructions
struct { const char *name; char *address;} jMap[] 
=	{
		{ "j", "000010" },
		{ "jal", "000011" },
		{ NULL, 0 } 
	};		
