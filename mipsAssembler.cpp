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
 * 02/23/17	Max Austin & Alex Reid  	Began implementation, built scanLabels and assembleLine
 * 02/24/17	Max Austin & Alex Reid		85% of implmentation done, formatting/logic errors arose
 * 02/25/17	Max Austin & Alex Reid		Finished implmentation, bugs found and squashed!
 *
 * DESCRIPTION
 * A two-pass assembler that translates a subset of the MIPS instruction set to machine code. 
 * This assembler uses only the .text segment of MIPS instruction code.
 * At a high level, the functionality of our assembler can be divided as follows:
 * Pass 1: Reads the input file. The address of each label is recorded into the symbol table. 
 * Pass 2: Re-reads the input file and translates each instruction to machine code.
 * Comments are stripped here and the current address is incremented and displayed each line.
 * Instruction syntax and arguments are validated one line at a time, and then the functions 
 * to assemble the correct instruction type and to subsequently display it are called.
 *
 * Structs that contain the registers and opCodes for each instruction type are found in the included
 * mipsData.cpp file
 */

//-------------
// C++ Includes
//-------------
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include "mipsData.cpp"

//-----------------
// Using Statements
//-----------------
using namespace std;

//-----------------
// Data Stuctures
//-----------------
struct symbol{ 
	char label[10]; 
	int32_t address;
};

typedef struct symbol symbol;

//This vector stores each label and their respective addresses.
//This is used for branch and jump statements.
vector<symbol> symbolTable;

//Starting address
int32_t currAddress = 0x00400000;

/*********************************************************
* DESCRIPTION - scanLabels
* Does the first pass of the input data and creates the 
* symbol table.
*
* PARAMETERS
* string filename: name of the input file
*
**********************************************************/
void scanLabels(string filename);

/*********************************************************
* DESCRIPTION - assembleLine
* Performs the second pass of the input data and creates
* each instruction line by tokenizing each part of the 
* instruction lines.
*
* PARAMETERS
* string filename: name of the input file
*
**********************************************************/
void assembleLine(string filename);

/*********************************************************
* DESCRIPTION - makeRtype
* Creates the machine code for instructions that are in the
* R-format.
*
* PARAMETERS
* char* opCode: opCode instruction from input file
* char* r2: 	The rirst register read in from the input line
* char* r3:		The second register read in from the input line
* char* r4:		The final register read in from the input line
* string line:	The entire line from the input file
*
**********************************************************/
void makeRtype(char* opCode,char* r2,char* r3,char* r4,string line);

/*********************************************************
* DESCRIPTION - makeItype
* Creates the machine code for instructions that are in the
* I-format.
*
* PARAMETERS
* char* opCode: opCode instruction from input file
* char* r2: 	The rirst register read in from the input line
* char* r3:		The second register read in from the input line
* char* r4:		The final register read in from the input line
* string line:	The entire line from the input file
*
**********************************************************/
void makeItype(char* opCode,char* r2,char* r3,char* r4,string line);

/*********************************************************
* DESCRIPTION - makeJtype
* Creates the machine code for instructions that are in the
* J-format.
*
* PARAMETERS
* char* opCode: opCode instruction from input file
* char* r2: 	The address that will be jumped to
* string line:	The entire line from the input file
*
**********************************************************/
void makeJtype(char* opCode,char* r2,string line);

/*********************************************************
* DESCRIPTION - stripReg
* Strips the $ off of the registers
*
* PARAMETERS
* char* reg: 	register directly from the input file with
*				the $ still on it
*
* RETURNS
* Returns the register input but with the $ stripped off
*
**********************************************************/
char* stripReg(char* reg);

/*********************************************************
* DESCRIPTION - printHeader
* Prints the header of the output up to the start of the 
* mips instructions.
*
**********************************************************/
void printHeader();

/*********************************************************
* DESCRIPTION - displayInstruct
* Prints the mips instruction followed by its respective 
* address and machine code.
*
* PARAMETERS
* string line:		The full instruction line from the input file
* int32_t mCode:	The machine code calculated for the instruction
*
**********************************************************/
void displayInstruct(string line, int32_t mCode);

/*********************************************************
* DESCRIPTION - getLabelAddress
* Gets the address for a label from the symbol table.
*
* PARAMETERS
* char* label:		The label whose address need to be found
*					in the symbol table
*
* RETURNS
* The address that the label is located at.
*
**********************************************************/
int getLabelAddress(char* label);

int main(){
	string filename;
	
	cout << "Enter name of input file: ";
	cin >> filename;
	
	scanLabels(filename);
	printHeader();
	assembleLine(filename);
	
	return 0;
}//main()


void scanLabels(string filename){
	string line;
	ifstream inputfile(filename.c_str());
	
	if(inputfile.is_open()){
		//This strips out the .txt line
		getline(inputfile,line);
		while(getline(inputfile, line)){
			// If we see a label (defined as ending in a colon)
			// add it to the symbolTable
			if(strstr(line.c_str(), ":") != NULL ){
				symbol newSymbol;
				char temp[100];
				strcpy(temp, line.c_str());
				//Assigns label's name after stripping colon
				strcpy(newSymbol.label, strtok(temp, ":"));
				//then assigns the memory location/address of the label
				newSymbol.address = currAddress;
				//Push newly created symbol into symbolTable
				symbolTable.push_back(newSymbol);
			}//if
			currAddress += 4;
		}//while
		
		//Resets for 2nd pass when we read instructions
		currAddress = 0x00400000;
	}else{
		cout << "Unable to open file!" << endl;
	}//if
}//scanLabels()

void assembleLine(string filename){
	
	//These lines are to reset the input file so that a second
	//pass can be done.
	ifstream inputfile(filename);
	inputfile.clear();
	inputfile.seekg(0,ios::beg);
	
	char delim [] = " ,\t)(";
	char * opCode;
	char * r2;
	char * r3 = NULL;
	char * r4 = NULL;
	string line;
	
	if(inputfile.is_open()){
		//print out the start of the input i.e. .text
		getline(inputfile,line);
		cout << line << endl;
		
		while(getline(inputfile,line)){
			size_t pos = line.find(":");
			string newLine = line.substr(pos+1);
			pos = newLine.find("#");
			
			if(pos != string::npos)
				newLine = newLine.substr(0,pos);
			
			string endChar = newLine.substr(newLine.length()-1);
			
			if(endChar != " "){
				newLine.append("\t");
			}
			
			char temp[100];
			strcpy(temp, newLine.c_str());
			opCode = strtok(temp, delim);
			
			if(opCode != NULL)
				r2 = strtok(NULL, delim);
			if(r2 != NULL)
				r3 = strtok(NULL, delim);
			if(r3 != NULL)
				r4 = strtok(NULL, delim);
			
			if( strstr(opCode, "i") != NULL || 
				strstr(opCode, "w") != NULL ||
				strstr(opCode, "bne")!= NULL ||
				strstr(opCode, "beq")!= NULL){
				
				makeItype(opCode, r2, r3, r4, line);
			}
			else if(strstr(opCode, "j") != NULL &&
					strstr(opCode,"jr") == NULL){
				
				makeJtype(opCode, r2,line);
			}
			else
				makeRtype(opCode, r2, r3, r4,line);
			
			currAddress += 4;
		}
	}else
		cout << "Unable to open file!" << endl;
	inputfile.close();
}//assembleLine()

void makeRtype(char* opCode,char* r2,char* r3,char* r4,string line){
	int32_t mCode = 0;
	int opFunct, rd, rs, rt;
	
	if(strstr(opCode,"jr") != NULL){
		string endChar = line.substr(line.length()-1);
		if(endChar != " "){
			line.append("\t");
		}
		r3 = stripReg(r2);
		rd = 0x0;
		rt = 0x0;
	}
	else{
		r2 = stripReg(r2);
		r3 = stripReg(r3);
		r4 = stripReg(r4);
	}
	
	int indx = -1;
	while(rTypeInstruct[++indx].name != NULL){
		
		if(strcmp(opCode,rTypeInstruct[indx].name) == 0)
			opFunct = rTypeInstruct[indx].opcode;
		}//for - opCode instructions
		indx = -1;
	
	while(registerNum[++indx].name != NULL){
		
		if(strcmp(r2,registerNum[indx].name) == 0)
			rd = registerNum[indx].regNum;
		if(strcmp(r3,registerNum[indx].name) == 0){
			rs = registerNum[indx].regNum;
		}
		if(strcmp(r4,registerNum[indx].name) == 0){
			rt = registerNum[indx].regNum;
		}
	}
	
	mCode = (0<<26);
	mCode |= rs << 21;
	mCode |= rt << 16;
	mCode |= rd << 11;
	mCode |= (0<<6);
	mCode |= opFunct;
	
	displayInstruct(line, mCode);
}//makeRtype()

void makeItype(char* opCode,char* r2,char* r3,char* r4,string line){
	
	int32_t mCode = 0;
	int16_t immed = 0;
	int opFunct, rt, rs;
	if(strstr(opCode,"w") != NULL){
		char* temp = stripReg(r4);
		immed = atoi(r3);
		r2 = stripReg(r2);
		r3 = temp;
	}
	else if(strstr(opCode,"b") != NULL){
		char* temp = r2;
		r2 = stripReg(r3);
		r3 = stripReg(temp);
		int branchAddress = getLabelAddress(r4);
		int numInstruct = (branchAddress - currAddress - 4) / 4;
		immed = numInstruct;
	}
	else{	
		r2 = stripReg(r2);
		r3 = stripReg(r3);
		immed = atoi(r4);
	}
	
	int indx = -1;
	while(iTypeInstruct[++indx].name != NULL){
		if(strcmp(opCode,iTypeInstruct[indx].name) == 0)
			opFunct = iTypeInstruct[indx].opcode;
	}
	
	indx = -1;
	while(registerNum[++indx].name != NULL){
		if(strcmp(r2,registerNum[indx].name) == 0)
			rt = registerNum[indx].regNum;
		if(strcmp(r3,registerNum[indx].name) == 0)
			rs = registerNum[indx].regNum;
	}
	
	mCode = immed;
	mCode &=~ 0xFFFF0000;
	mCode |= (opFunct<<26);
	mCode |= rs << 21;
	mCode |= rt << 16;
	
	displayInstruct(line, mCode);
}//makeItype()

void makeJtype(char* opCode,char* r2,string line){
	
	string endChar = line.substr(line.length()-1);
	if(endChar != " "){
		line.append("\t");
	}
	int32_t mCode = 0;
	int opFunct;
	int indx = -1;
	
	while(jTypeInstruct[++indx].name != NULL){
		if(strcmp(opCode,jTypeInstruct[indx].name) == 0)
			opFunct = jTypeInstruct[indx].opcode;
	}//for - opCode instructions

	int jumpAdd = getLabelAddress(r2);	
	
	mCode = jumpAdd;
	mCode &=~ 0xFF000000;
	mCode |= opFunct << 26;
	
	displayInstruct(line, mCode);
}//makeJType();

char* stripReg(char* reg){
	char* cut = strtok(reg,"$");
	return cut;
}//stripReg()

void printHeader(){
	cout << endl << "Symbol Table:" << endl << endl;
	cout << "Label\tAddress(in hex)" << endl;
	
	for(int indx = 0; indx < symbolTable.size(); indx++){
		cout << symbolTable.at(indx).label << "\t";
		printf("%08x\n",symbolTable.at(indx).address);
	}
	cout << endl << "MIPS code\t\t\t\t\tAddress     Machine Lang." << endl;
}//printHeader()

void displayInstruct(string line, int32_t mCode){
	
	char temp[100];
	strcpy(temp,line.c_str());
	char* newLine;
	
	if(strstr(temp,"#") != NULL){
		newLine = strtok(temp,"#");
		cout << newLine;
	}
	else{
		cout << temp << "\t\t";
	}
	printf("%08x",currAddress);
	cout << "\t";
	printf("%08x\n",mCode);
}//displayInstruct()

int getLabelAddress(char* label){
	for(int indx = 0; indx < symbolTable.size(); indx++){
		if(strcmp(symbolTable[indx].label,label) == 0)
			return symbolTable[indx].address;
	}
	return -1;
}//getLabelAddress()
