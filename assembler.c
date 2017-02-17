#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "tables.h"
#include "translate_utils.h"
#include "translate.h"
#include "assembler.h"

const int MAX_ARGS = 3;
const int BUFF_SIZE = 32;
const char* INGORE_CHARS = " \f\n\r\t\v,()";

//HELPER FUNCTIONS

//You should not be calling this yourself
static void raiseLabelError(uint32_t inputLine, const char* label) 
{
    write_to_log("Error - invalid label at line %d: %s\n", inputLine, label);
}

/*
   Call this function if more than MAX_ARGS are found while parsing arguments.
   INPUT_LINE is which line of the input file that the error occurred in. 
   Note that the first line is line 1 and that empty lines are included in the count.
   EXTRA_ARG should contain the first extra argument encountered.
*/
static void raiseExtraArgError(uint32_t inputLine, const char* extraArg) 
{
    write_to_log("Error - extra argument at line %d: %s\n", inputLine, extraArg);
}

/* 
   Call this function if writePassOne() or translateInst() 
   returns -1. 
 
   inputLine is which line of the input file that the error occurred in. 
*/
static void raiseInstError(uint32_t inputLine, const char* name, char** args, int numArgs) 
{
    
    write_to_log("Error - invalid instruction at line %d: ", inputLine);
    log_inst(name, args, numArgs);
}

// Comment stripper, skips the string at the first occurrence of the '#' character. 
static void skipComment(char* str) 
{
    char* commentStart = strchr(str, '#');
    if (commentStart) 
    {
        *commentStart = '\0';
    }
}

/* 
   Reads str and determines whether it is a (valid!) label (ends in ':'),
   then tries to add it to the symbol table.
   inputLine is which line of the input file we are currently processing.
   byteOffset is the offset of the NEXT instruction (if it exists). 
   Four scenarios can happen:
    1. str is not a label (does not end in ':'). Returns 0.
    2. str ends in ':', but is not a valid label. Returns -1.
    3. str ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    4. str ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int addLabel(uint32_t inputLine, char* str, uint32_t byteOffset,
													SymbolTable* symtbl) 
{
    size_t length = strlen(str);
    if (str[length - 1] == ':') 
    {
        str[length - 1] = '\0';
        if (isValidLabel(str)) 
        {
            if (addToTable(symtbl, str, byteOffset) == 0) 
            {
                return 1;
            } else {
                return -1;
            }
        } else {
            raiseLabelError(input_line, str);
            return -1;
        }
    } else {
        return 0;
    }
}



/* 
   First pass of the assembler. 
   This function reads each line, strips the comments, scans for labels,
   and passes instructions to writePassOne(). 
    1. Only one label may be present per line. It must be the first token present.
        Once you see a label, regardless of whether it is a valid label or invalid
        label, treat the NEXT token as the beginning of an instruction.
    2. If the first token is not a label, treat it as the name of an instruction.
    3. Everything after the instruction name should be treated as arguments to
        that instruction. If there are more than MAX_ARGS arguments, call
        raise_extra_arg_error() and pass in the first extra argument. Do not 
        write that instruction to the output file (eg. don't call write_pass_one())
    4. Only one instruction should be present per line. You do not need to do 
        anything extra to detect this - it should be handled by guideline 3. 
    5. A line containing only a label is valid. The address of the label should
        be the byte offset of the next instruction, regardless of whether there
        is a next instruction or not.
   If the function encounters an error it won't break, and will process the entire file
   and simply return -1. If no errors were encountered, it will return 0.
 */
 int passOne(FILE* input, FILE* output, SymbolTable* symtbl) 
 {
    uint32_t lineCounter = 1;
    int numValidInstructSoFar = 0;
    char buff[BUFF_SIZE];
    int boolean = 0;
    while (fgets(buff, sizeof(buff), input)) 
    {
        char* args[MAX_ARGS + 1];
        int numArgs = 0;
        skipComment(buff);
        int toWrite = 0;
        char* currToken = strtok(buff, IGNORE_CHARS);
        
        if(strcmp(buff, "") == 0)
        {
            line_counter += 1;
            continue;
        }
        
        if (currToken == NULL) {
            line_counter += 1;
            continue;
        }
        
        int retval = addLabel(lineCounter, currToken, numValidInstructSoFar * 4, symtbl);
        if(retval != 0) 
        {
            currToken = strtok(NULL, IGNORE_CHARS);
            if(strcmp(currToken, "") == 0)
            {
                lineCounter += 1;
                continue;
            }
        } else {
        
            char name[10];
            strcpy(name, currToken);
            // printf("%s\n", name);
            currToken = strtok(NULL, IGNORE_CHARS);
            while(currToken != NULL)
            {
                char* ptr = currToken;
                args[numArgs] = ptr;
                numArgs += 1;
                currToken = strtok(NULL, IGNORE_CHARS);
                if(numArgs > MAX_ARGS) 
                {
                    boolean = 1;
                    toWrite = 1;
                    raiseExtraArgError(lineCounter, currToken);
                    break;
                }
            }
            int returnVal = writePassOne(output, name, args, numArgs);
            if(!returnVal) 
            {
                boolean = 1;
            }
            if(returnVal && toWrite == 0) 
            {
                numValidInstructSoFar += returnVal;
            }
            lineCounter += 1;
        }
    }
    return boolean;
}



/* 
   Reads an intermediate file and translates it into machine code. 
   After passOne the file should:
   1. Contain no comments
   2. Contain no labels
   3. Contain at maximum one instruction per line
   4. All instructions have at maximum MAX_ARGS arguments
   5. The symbol table has been filled out already
   If an error is reached, the function keeps translating the rest of
   the document, and at the end, return -1. 
   Returns 0 if no errors were encountered. 
   Relocation table is generated here.
*/
int passTwo(FILE *input, FILE* output, SymbolTable* symtbl, SymbolTable* reltbl) 
{
    char buff[BUFF_SIZE];
    int boolean = 0;
    uint32_t line = 0;
    char* args[MAX_ARGS];
    while (fgets(buff, sizeof(buff), input)) 
    {
        char* currLine = strtok(buff, IGNORE_CHARS); //MAY NEED TO CHANGE THIS
        int numArgs = 0;
        char name[10];
        strcpy(name, currLine);
        currLine = strtok(NULL, IGNORE_CHARS);
        while(currLine != NULL) 
        {
            char* pt = currLine;
            args[numArgs] = pt;
            numArgs += 1;
            currLine = strtok(NULL, IGNORE_CHARS);
        }
        uint32_t branchOff = line * 4;
        int retval = 
        	translateInst(output, name, args, numArgs, branchOff, symtbl, reltbl);
        if (retval == -1) 
        {
            raiseInstError(line + 1, name, args, numArgs);
            boolean = 1;
        }
        line += 1;
    }
    if (boolean) 
    {
        return -1;
    }
    return 0;
}


/******************
 * Do Not Modify! *
 ******************/

static int openFiles(FILE** input, FILE** output, const char* input_name, 
    const char* output_name) 
{    
	//read
    *input = fopen(input_name, "r");
    if (!*input) 
    {
        write_to_log("Error: unable to open input file: %s\n", input_name);
        return -1;
    }
    //write
    *output = fopen(output_name, "w");
    if (!*output) 
    {
        write_to_log("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0;
}

static void closeFiles(FILE* input, FILE* output) 
{
    fclose(input);
    fclose(output);
}


/* 
   Runs the two-pass assembler. 
   Most of the actual work is done in passOne() and passTwo().
*/
int assemble(const char* inName, const char* tempName, const char* outName) 
{
    FILE *src, *dst;
    int err = 0;
    SymbolTable* symtbl = createTable(SYMTBL_UNIQUE_NAME);
    SymbolTable* reltbl = createTable(SYMTBL_NON_UNIQUE);

    if (inName) {
        printf("Running pass one: %s -> %s\n", inName, tempName);
        if (openFiles(&src, &dst, inName, tempName) != 0)
        {
            freeTable(symtbl);
            freeTable(reltbl);
            exit(1);
        }

        if (pass_one(src, dst, symtbl) != 0) 
        {
            err = 1;
        }
        closeFiles(src, dst);
    }

    if (out_name) {
        printf("Running pass two: %s -> %s\n", tmp_name, out_name);
        if (open_files(&src, &dst, tmp_name, out_name) != 0) {
            free_table(symtbl);
            free_table(reltbl);
            exit(1);
        }

        fprintf(dst, ".text\n");
        if (pass_two(src, dst, symtbl, reltbl) != 0) {
            err = 1;
        }
        
        fprintf(dst, "\n.symbol\n");
        write_table(symtbl, dst);

        fprintf(dst, "\n.relocation\n");
        write_table(reltbl, dst);

        close_files(src, dst);
    }
    
    free_table(symtbl);
    free_table(reltbl);
    return err;
}







