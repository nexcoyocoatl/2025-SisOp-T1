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

struct Program {
    size_t id;
    size_t pc;
    int acc;

    size_t deadline;
    int deadline_time_remaining;
    size_t next_deadline;
    size_t real_deadline;
    size_t processing_time;
    int processing_time_remaining;
    size_t arrival_time;
    int auto_user_input;

    size_t syscall_time;
    size_t syscall_count;

    int b_debug;
    int b_running;
    int b_finished;

    // Counters
    size_t variable_count; // Counter for number of variables in data
    size_t instruction_count; // "Virtual" memory for instructions
    size_t instructions_executed;

    // "Virtual" Memory
    struct Instruction instructions[512];
    struct Variable variables[32];
};

// Operation functions
void load( struct Program *program, struct Variable *op1 );

void store( struct Program *program, struct Variable *op1 );

void add( struct Program *program, int op1 );

void sub( struct Program *program, int op1 );

void mult( struct Program *program, int op1 );

void divide( struct Program *program, int op1 ); // There's already a "div" in stdlib.h

void brany( struct Program *program, int op1 );

void brpos( struct Program *program, int op1 );

void brzero( struct Program *program, int op1 );

void brneg( struct Program *program, int op1 );

void syscall( struct Program *program, int index );

// Reads one line of code instructions
void read_code( struct Program *program, char *instruction );

// Reads one line of data instructions
void read_data( struct Program *program, char *instruction);

// Reads line of instructions, one by one, calling the respective function for its type
void read_instructions( struct Program *program, FILE *fileptr );

// Executes one program instruction based on its program counter
int execute_instruction( struct Program *program );

int run_one_instruction( struct Program *program );

// Runs the program, executes and prints every instruction and then increments its PC
int run_program( struct Program *program );

// Calculates the program deadline based on its processing time and number of interrupts
int calculate_real_deadline( struct Program *program );

// Reads the received lines from a text file and calls the relevant functions to create instructions of a program
int program_setup( struct Program *program, FILE* fileptr, int debug, size_t arrival_time, size_t processing_time, size_t deadline );

#endif