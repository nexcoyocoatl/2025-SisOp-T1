#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basic variables used for this abstraction of assembly code
int acc;
int pc;

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

enum Type
{
    IMMEDIATE = 0,
    VAR_POINTER
};

// (WIP) Instructions have an op code an an index for a variable (e.g. acc)
struct Instruction
{
    enum Operation operation;
    int type;
    union {
        struct Variable *var_pointer;
        int immediate;
    };
};

// Will contain variables and unnamed values to be used in memory as immediates (probably refactor later? maybe rename?)
struct Variable
{
    char name[256];
    int value;
};

// Counters
size_t variable_count; // Counter for number of variables in data
size_t instruction_count; // "Virtual" memory for instructions (maybe refactor?)

// "Virtual" Memory
struct Instruction instructions[512]; //
struct Variable variables[32];

// Struct that will create dictionary-like lists of string-value pairs
struct Conversion {
    enum Operation value;
    const char *str;
};

// List of strings and their op codes in enum values
struct Conversion operation_str2enum [] = {
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MULT, "MULT"},
    {DIV, "DIV"},
    {LOAD, "LOAD"},
    {STORE, "STORE"},
    {BRANY, "BRANY"},
    {BRPOS, "BRPOS"},
    {BRZERO, "BRZERO"},
    {BRNEG, "BRNEG"},
    {SYSCALL, "SYSCALL"}
};

// Function to find the op code from the list above
enum Operation str2enum(const char *s)
{
    for (size_t i = 0; i < sizeof(operation_str2enum) / sizeof(struct Conversion); i++)
    {
        if (strcmp(s, operation_str2enum[i].str) == 0)
        {
            return operation_str2enum[i].value;
        }
    }
    return OP_ERROR;
}

// Operation functions
void load( int op1 )
{
    acc = op1;
}

void store( int op1 )
{
    op1 = acc;
}

void add( int op1 )
{
    acc = acc + op1;
}

void sub( int op1 )
{
    acc = acc - op1;
}

void mult( int op1 )
{
    acc = acc * op1;
}

void divide( int op1 ) // There's already a "div" in stdlib.h
{
    acc = acc / op1;
}

void brany( int op1 )
{
    pc = op1;
}

void brpos( int op1 )
{
    if (acc > 0)
    {
        pc = op1;
    }
}

void brzero( int op1 )
{
    if (acc == 0)
    {
        pc = op1;
    }
}

void brneg( int op1 )
{
    if (acc < 0)
    {
        pc = op1;
    }
}

void syscall( int index )
{
    switch (index)
    {
        case 0:
            printf("Program halted\n");
            break;
        case 1:
            // printf("acc = ");
            printf("%d\n", acc);
            break;
        case 2:
            // printf("Type a value: ");
            scanf("%d", &acc);
            break;
        default:
            printf("Unknown system call\n");
    }
}

//
// AINDA NÃO FUNCIONA PRO PROG2.TXT
// PRECISA FAZER UM CASO PROS LABELS E BRANCHES
//

// Reads one line of code instructions
void read_code( char *instruction )
{
    char delim[] = "\n\r ";
    char *temp;
    char operation_string[128];
    char variable[128];
    int opcode;
    int value = 0;

    int b_immediate = 0;

    if ( (temp = strtok(instruction, delim)) != NULL )
    {        
        for (int i = 0; isalpha(temp[i]); i++)
        {
            temp[i] = toupper(temp[i]);
        }

        strcpy(operation_string, temp);
    }
    
    if ( (temp = strtok(NULL, delim)) != NULL )
    {
        while(isspace((unsigned char)*temp)) temp++;

        for (int i = 0; isalpha(temp[i]); i++)
        {
            temp[i] = toupper(temp[i]);
        }

        // Checks if the second part is another variable or an immediate value
        if (temp[0] == '#')
        {
            temp++;
            b_immediate = 1;
            value = atoi(temp);
        }
        else
        {
            strcpy(variable, temp);
        }
    }

    if ((opcode = str2enum(operation_string)) != OP_ERROR)
    {
        struct Variable temp_variable;
        struct Instruction temp_instruction;

        // Checks if the value is an immediate (branches and syscall parameters are considered immediate here) or a variable
        if (b_immediate || opcode > 5)
        {
            struct Instruction temp_instruction = {opcode, IMMEDIATE};
            temp_instruction.immediate = value;
            
            instructions[instruction_count] = temp_instruction;
            instruction_count++;
        }

        else
        {            
            struct Variable *variable_pointer = NULL;
            for (size_t i = 0; i < variable_count; i++) {
                if (strcmp(variable, variables[i].name) == 0)
                {
                    variable_pointer = &variables[i];
                    break;
                }
            }

            if (variable_pointer == NULL)
            {
                perror("variable not found\n");
                return;
            }

            struct Instruction temp_instruction;
            temp_instruction.operation = opcode;
            temp_instruction.type = VAR_POINTER;
            temp_instruction.var_pointer = variable_pointer;
            
            instructions[instruction_count] = temp_instruction;
            instruction_count++;
        }
    }
    else
    {
        perror("OPCODE not found!\n");
    }
}

// Reads one line of data instructions
void read_data( char *instruction)
{
    char delim[] = "\n\r ";
    char *temp;
    char variable_name[128];
    int value;

    if ( (temp = strtok(instruction, delim)) != NULL )
    {        
        for (int i = 0; isalpha(temp[i]); i++)
        {
            temp[i] = toupper(temp[i]);
        }

        strcpy(variable_name, temp);
    }
    
    if ( (temp = strtok(NULL, delim)) != NULL )
    {
        while(isspace((unsigned char)*temp)) temp++;
        value = atoi(temp);
    }

    struct Variable temp_variable;
    strcpy(temp_variable.name, variable_name);
    temp_variable.value = value;

    variables[variable_count] = temp_variable;
    variable_count++;
}

// Reads line of instructions, one by one, calling the respective function for its type
void read_instructions( FILE *fileptr )
{
    char s[256];
    char *instruction;
    char temp_code[512][256];
    size_t temp_code_i;

    int b_code = 0;
    int b_data = 0;

    // While there are still lines in the file
    while (fgets(s, 256, fileptr))
    {
        instruction = s;
    
        // Remove leading spaces
        while (isspace(instruction[0]) || instruction[0] == '\r' || instruction[0] == '\n' || instruction[0] == '\t')
        {
            instruction++;
        }

        // Ignore if it's only spaces
        if (*instruction == 0)
        {
            continue;
        }
        
        //
        // (WIP) ULTIMA LINHA DO PROG2 NÃO FUNCIONA AQUI
        //

        // Remove trailing spaces
        char *end = instruction + strlen(instruction) -1;
        while ( end > instruction && (isspace(instruction[0]) || instruction[0] == '\r' || instruction[0] == '\n' || instruction[0] == '\t'))
        {
            end--;
        }
        end[0] = '\0';

        if (strcmp( instruction, ".endcode") == 0)
        {
            b_code = 0;
            continue;
        }

        if (strcmp( instruction, ".enddata") == 0)
        {
            b_data = 0;
            continue;
        }

        if (b_code == 1)
        {
            strcpy(temp_code[temp_code_i], instruction);
            temp_code_i++;
            continue;
        }

        if (b_data == 1)
        {
            read_data( instruction );
            continue;
        }

        if ( b_code == 0 && strcmp(instruction, ".code") == 0 )
        {
            b_code = 1;
        }        
        else if ( b_data == 0 && strcmp(instruction, ".data") == 0 )
        {
            b_data = 1;
        }
    }

    // Always reads code instructions after data instructions
    for (size_t i = 0; i < temp_code_i; i++)
    {
        read_code( temp_code[i] );
    }
}

// Main only opens one file and stores one program for now
int main( int argc, char **argv )
{ 
    FILE* fileptr;

    if (argc == 2)
    {
        fileptr = fopen(argv[1], "r");
    }
    else
    {
        // Will be changed when there are other files to be loaded, for now it's just one
        printf("Usage: ./main [program_filename] (e.g. ./main prog1.txt) \n\n");
        char input[100] = "";
        printf("Type the name of the file: ");
        scanf("%s", input);
        printf("\n");
        fileptr = fopen(input, "r");
    }

    // Se não existe o arquivo, programa termina com erro
    if (fileptr == NULL)
    {
        printf("File not found.\nExiting program...\n");
        return 1;
    }

    read_instructions(fileptr);

    fclose(fileptr);

    // Test: Prints all variables and instructions on virtual memory
    printf("Data:\n");
    for (size_t i = 0; i < variable_count; i++)
    {
        printf(" %lu: %s = %d\n", i, variables[i].name, variables[i].value);
    }
    printf("Code Instructions:\n");
    for (size_t i = 0; i < instruction_count; i++)
    {
        char a[256];
        printf(" %lu: OPCODE %d: %s%s%d\n",
                        i,
                        instructions[i].operation,
                        instructions[i].type==IMMEDIATE? "" : instructions[i].var_pointer->name,
                        instructions[i].type==IMMEDIATE? "" : " = ",
                        instructions[i].type==IMMEDIATE? instructions[i].immediate : instructions[i].var_pointer->value
                    );
    }

    return 0;
}