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
using namespace std;

// data structures
struct label {
    char name[100];
    int32_t location;
};

// Global variables
label label_list[100];
int labelindex = 0;

int32_t instructions[1000];
int instr_index = 0;

//int32_t datasection[1000];
//int datasectionindex = 0;

// functions
void scanLabels(char *filename);
//int countDataSection(char *filename);

void assembleLine(char *line);

void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct);
void makeI_type(int op, int rs, int rt, int addr);
void makeJ_type(int op, int addr);

int regToInt(char *reg);
int labelToIntAddr(char *label);
int immToInt(char *immediate);
char *int2bin(int a, char *buffer, int buf_size);

/************************ MAIN ************************/


int main(int argc, const char * argv[]) 
{    
    if (argv[1] == NULL) {
        printf("Please type in input file name.\n");
        return 0;
    }
     
    char filename[20];
    strcpy(filename, argv[1]);
    
    //strcpy(filename, "example_mod.s");
    //strcpy(filename, "example2_mod.s");
    //strcpy(filename, "example3.s");
    //strcpy(filename, "example4.s");
    
    //int dataSectionSize = countDataSection(filename);
    scanLabels(filename);
    
    string line;
    ifstream inputfile(filename);
    if (inputfile.is_open())
    {
        while (getline(inputfile, line) )
        {
            if(strstr(line.c_str(),".text") != NULL)
			{
                break;
            }
        }
        
        while (getline(inputfile, line) )
        {
            if (strstr(line.c_str(), ":") != NULL) 
			{
                continue;
            } else {
                char codeline[100];
                strcpy(codeline, line.c_str());
                assembleLine(codeline);
            }
        }
        inputfile.close();
    }
    else cout << "Unable to open file\n";
    
    
    /* Writing section */
    /*
    ofstream outputfile("output.o", ios::out | ios::binary);
    outputfile.put(instr_index*4); // text section size
    outputfile.put(dataSectionSize*4); // data section size
    int i;
    for (i=0; i<instr_index; i++) {
        outputfile.put(instructions[i]);
    }
    for (i=0; i<datasectionindex; i++) {
        outputfile.put(datasection[i]);
    }
    outputfile.close();
    */
    
    FILE f = *fopen("output.o", "w");
    char binary[40];
    int2bin(instr_index*4, binary, 32);
    fprintf(&f, "%s", binary);
    
    int i;
    for (i=0; i<instr_index; i++) 
	{
        int2bin(instructions[i], binary, 32);
        fprintf(&f, "%s", binary);
    }
    
    //fprintf(&f, "\n");
    fclose(&f);
    
    return 0;
}


void scanLabels(char *filename) 
{
    int datasizecnt = 0;
    int instruction_count = 0;
    
    string line;
    ifstream inputfile(filename);
    if (inputfile.is_open())
    {      
        // Labels inside .text section (function labels)
        while (getline(inputfile, line)) {
            if (strstr(line.c_str(), ":") != NULL) 
			{
                label newLabel;
                char temp[100];
                strcpy(temp, line.c_str());
                strcpy(newLabel.name , strtok(temp,":"));
                newLabel.location = 0x400000 + instruction_count*4;
                label_list[labelindex++]=newLabel;
            } else {
                char temp[100];
                strcpy(temp, line.c_str());
                
                char * one;
                char * two;
                char * three = NULL;
                char key[] = " ,\t";
                one = strtok(temp, key);
                if(one != NULL)
                    two = strtok(NULL,key);
                if(two != NULL)
                    three = strtok(NULL,key);
                
                if (strcmp(one, "la") == 0){
                    int location = labelToIntAddr(three);
                    if ((location & 65535) == 0) {          // if lower 16bit address is 0x0000
                        instruction_count++;                // lui instruction
                    } else {
                        instruction_count += 2;             // lui instruction + ori instruction
                    }
                } else {
                    instruction_count++;
                }
            }
        }
        
        
        inputfile.close();
    }
    else cout << "Unable to open file\n";
}

/* Assembling Function */
void assembleLine(char *line){

    char key[] = " ,\t";
    char * one;
    char * two;
    char * three;
    char * four = NULL;
    
    one = strtok(line,key);
    
    if(one != NULL)
        two = strtok(NULL,key);
    if(two != NULL)
        three = strtok(NULL,key);
    if(three != NULL)
        four = strtok(NULL,key);
    
    
    if (strcmp(one, "addi") == 0) {
        // I-type
        // addi $t,$s,C
        // $t = $s + C (unsigned)
        // R[rt] = R[rs] + SignExtImm
        makeI_type(8, regToInt(three), regToInt(two), immToInt(four));
        
    } else if (strcmp(one, "add") == 0) {
        // R-type
        // add $d,$s,$t
        // $d = $s + $t
        // R[rd] = R[rs] + R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 33);
        
        
    } else if (strcmp(one, "sub") == 0) {
        // R-type
        // subu $d,$s,$t
        // $d = $s - $t
        // R[rd] = R[rs] - R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 35);
		
    }else if (strcmp(one, "beq") == 0) {
        // I-type
        // beq $s,$t,C
        // if ($s == $t) go to PC+4+4*C
        // if(R[rs]==R[rt]) PC=PC+4+BranchAddr
        int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
        makeI_type(4, regToInt(two), regToInt(three), (reladdr/4));          // relative address
        
    } else if (strcmp(one, "bne") == 0) {
        // I-type
        // bne $s,$t,C
        // if ($s != $t) go to PC+4+4*C
        // if(R[rs]!=R[rt]) PC=PC+4+BranchAddr
        int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
        makeI_type(5, regToInt(two), regToInt(three), (reladdr/4));          // relative address
        
    } else if (strcmp(one, "jal") == 0) {
        // J-type
        // jal C
        // $31 = PC + 4; PC = PC+4[31:28] . C*4
        // R[31]=PC+8;PC=JumpAddr
        makeJ_type(3, (labelToIntAddr(two)>>2));                             // absolute address
        
    } else if (strcmp(one, "jr") == 0) {
        // R-type
        // jr $s
        // goto address $s
        // PC=R[rs]
        makeR_type(0, regToInt(two), 0, 0, 0, 8);
        
    } else if (strcmp(one, "j") == 0) {
        // J-type
        // j C
        // PC = PC+4[31:28] . C*4
        // PC=JumpAddr
        makeJ_type(2, (labelToIntAddr(two)>>2));                             // absolute address
        
    } else if (strcmp(one, "lw") == 0) {
        // I-type
        // lw $t,C($s)
        // $t = Memory[$s + C]
        // R[rt] = M[R[rs]+SignExtImm]
        char * offset = strtok(three,"()");
        char * rs = strtok(NULL,"()");
        makeI_type(35, regToInt(rs), regToInt(two), immToInt(offset));
        
    } else if (strcmp(one, "slt") == 0) {
        // R-type
        // sltu $d,$s,$t
        // R[rd] = (R[rs] < R[rt]) ? 1 : 0
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 43);
        
        
    } else if (strcmp(one, "sw") == 0) {
        // I-type
        // sw $t,C($s)
        // Memory[$s + C] = $t
        // M[R[rs]+SignExtImm] = R[rt]
        char * offset = strtok(three,"()");
        char * rs = strtok(NULL,"()");
        makeI_type(43, regToInt(rs), regToInt(two), immToInt(offset));
        
    }
}

// MIPS Fields
/*
 
 < R-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |   rd(5bits)  | shamt(5bits) |   fucnt(6bits)  |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 
 
 < I-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |         constant or address (16bits)          |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 
 < J-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |                            address (30bits)                                 |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 
*/


void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= (rs << 21);
    ans |= (rt << 16);
    ans |= (rd << 11);
    ans |= (shamt << 6);
    ans |= funct;
    instructions[instr_index++] = ans;
}

void makeI_type(int op, int rs, int rt, int addr) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= (rs << 21);
    ans |= (rt << 16);
    ans |= (addr & 65535);
    instructions[instr_index++] = ans;
}

void makeJ_type(int op, int addr) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= addr;
    instructions[instr_index++] = ans;
}


// Conversion functions
int regToInt(char *reg){
    //turns registers to integers
    char * cut = strtok(reg,"$");
    return atoi(cut);
}

int labelToIntAddr(char *label) {
    int i;
    for (i=0; i<labelindex; i++) {
        if (strcmp(label_list[i].name, label) == 0) {
            return label_list[i].location;
        }
    }
    return -1;
}

int immToInt(char *immediate){
    // 2 cases - hexadecimal and decimal.
    if (strstr(immediate,"0x") != NULL) {
        return (int)strtol(immediate, NULL, 0);
    } else {
        return atoi(immediate);
    }
}

char *int2bin(int a, char *buffer, int buf_size) {
    buffer += (buf_size - 1);
    for (int i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';
        a >>= 1;
    }
    return buffer;
}

