#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <stdio.h>
#include <stdlib.h>

// Operations have their own Op Code
enum Operation
{
    OP_ERROR = -1,
    ADD = 0,
    SUB,
    MULT,
    DIV,
    LOAD,
    STORE,
    BRANY,
    BRPOS,
    BRZERO,
    BRNEG,
    SYSCALL,
};

enum Operation str2enum(const char *s);

enum Type
{
    IMMEDIATE = 0,
    VAR_POINTER
};

// Instructions have an op code an an index for a variable
struct Instruction
{
    enum Operation operation;
    int type;
    union {
        int immediate;
        struct Variable *var_pointer;
    };
};

// Will contain variables and unnamed values to be used in memory as immediates (probably refactor later? maybe rename?)
struct Variable
{
    char name[256];
    int value;
};

struct Label
{
    char name[256];
    int line_num;
};

// Temporary struct to iterate through branches and find the label line numbers
struct Branch
{
    size_t instruction_num;
    char label_name[256];
};

// Struct that will create dictionary-like lists of string-value pairs
struct Conversion {
    enum Operation value;
    const char *str;
};

// Operation functions
void load( int op1 );

void store( int *op1 );

void add( int op1 );

void sub( int op1 );

void mult( int op1 );

void divide( int op1 ); // There's already a "div" in stdlib.h

void brany( int op1 );

void brpos( int op1 );

void brzero( int op1 );

void brneg( int op1 );

void syscall( int index );

// Reads one line of code instructions
void read_code( char *instruction );

// Reads one line of data instructions
void read_data( char *instruction);

// Reads line of instructions, one by one, calling the respective function for its type
void read_instructions( FILE *fileptr );

int execute_instruction( struct Instruction *instruction );

int run_program();

// Main only opens one file and stores one program for now
int program_setup( FILE* fileptr );

#endif