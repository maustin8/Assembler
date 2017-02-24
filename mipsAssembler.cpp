//
//  main.cpp
//  MIPS Assembler
//
//

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>

//Holds structs
#include "fileParser.cpp"

//Namespaces
using namespace std;

//Data Structures
struct symbol{ const char *label; int address;};

vector<symbol> symbolTable;
vector<int32_t> instructions;
int32_t currAddress = 0x00400000;


void scanLabels(char* filename)
{
	string line;
	ifstream inputfile(filename);
	
	if(inputfile.is_open())
	{
		while(getline(inputfile, line))
		{
			// if we see a label (defined as ending in a colon)
			// add it to the symbolTable
			if(strstr(line.c_str(), ":") != NULL )
			{
				symbol newSymbol;
				char temp[100];
				strcpy(temp, line.c_str());
				
				//Assigns label's name after stripping colon
				strcpy(newSymbol.label, strtok(temp, ":"))
				//then assigns the memory location/address of the label
				newSymbol.address = currAddress;
				
				//Push newly created symbol into symbolTable
				symbolTable.push_back(newSymbol);	
			}
			
			currAddress += 4;
		}//while
		inputfile.close();
		
		//Resets for 2nd pass when we read instructions
		currAddress = 0x00400000;
	}else{
		cout << "Unable to open file! \n";
	}//if
	
}//scanLabels()

void assembleLine(char *line)
{
	char key [] = ",\t";
	char * opCode;
	char * r2;
	char * r3 = NULL;
	char * r4 = NULL;
	
	opCode = strtok(line, key);
	
	if(opCode != NULL)
		r2 = strtok(NULL, key);
	if(r2 != NULL)
		r3 = strtok(NULL, key);
	if(r3 != NULL)
		r4 = strtok(NULL, key);	
	
	if( strstr(opCode, "i") != NULL || 
		strstr(opCode, "w") != NULL ||
		strstr(opCode, "b")	!= NULL )
	{
		makeItype(opCode, r2, r3, r4, line);
	}
	else if(strstr(opCode, "j") != NULL &&
			strstr(opCode, "jr") == NULL )
	{
		makeJtype(opCode, r2);
	}
	else
		makeRtype(opCode, r2, r3, r4);
		
		
}//assembleLine()

void makeRtype(char* opCode, r2, r3, r4, line)
{
	int32_t mCode = 0;
	r2 = stripReg(r2);
	r3 = stripReg(r3);
	r4 = stripReg(r4);
	
	int opFunct, rd, rs, rt;
	
	for(int i = 0; i < rTypeInstruct.length; i++)
	{
		if(strcmp(opCode,rTypeInstruct[i].name) == 0)
			opFunct = rTypeInstruct[i].opcode;
		
	}//for - opCode instructions
	for(i = 0; i < registerNum.length; i++)
	{
		if(strcmp(r2,registerNum[i].name) == 0)
			rd = registerNum[i].address;
		if(strcmp(r3,registerNum[i].name) == 0)
			rs = registerNum[i].address;
		if(strcmp(r4,registerNum[i].name) == 0)
			rt = registerNum[i].address;
	}//for - register numbers
	
	mCode = 0 << 26;
	mCode |= rs << 21;
	mCode |= rt << 16;
	mCode |= rd << 11;
	mCode |= mCode << 6;
	mCode |= opFunct;
	
	displayInstruct(line, mCode);
}//makeRtype()

void makeItype(char* opCode, r2, r3, r4, line)
{
	int32_t mCode = 0;
	r2 = stripReg(r2);
	r3 = stripReg(r3);
	
	int opFunct, rd, rs;
	
	int16_t immed = atoi(r4);
	
	for(int i = 0; i < iTypeInstruct.length; i++)
	{
		if(strcmp(opCode,iTypeInstruct[i].name) == 0)
			opFunct = iTypeInstruct[i].opcode;
		
	}//for - opCode instructions
	for(i = 0; i < registerNum.length; i++)
	{
		if(strcmp(r2,registerNum[i].name) == 0)
			rt = registerNum[i].address;
		if(strcmp(r3,registerNum[i].name) == 0)
			rs = registerNum[i].address;
	}//for - register numbers
	
	
	mCode = opFunct << 26;
	mCode |= rs << 21;
	mCode |= rt << 16;
	mCode |=  immed;
	
	displayInstruct(line, mCode);
}//makeItype()



char* stripReg(char* reg)
{
	char* cut = strtok(reg,"$");
	return cut;
}

void printTable()
{

}

void displayInstruct(char* line, int32_t mCode)
{
	cout << line << "\t" << currAddress << hex << mCode << endl;
}

void main()
{

	scanLabels();




}