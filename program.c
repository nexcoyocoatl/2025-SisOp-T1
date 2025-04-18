#include <ctype.h>
#include <string.h>
#include "program.h"

// Basic variables used for this abstraction of assembly code
int acc;
size_t pc;

// Counters
size_t variable_count; // Counter for number of variables in data
size_t instruction_count; // "Virtual" memory for instructions (maybe refactor?)
size_t label_count;
size_t branch_count;

// "Virtual" Memory
struct Instruction instructions[512];
struct Variable variables[32];
struct Label labels[256];
struct Branch branches[128];

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

void store( int *op1 )
{
    *op1 = acc;
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
    pc = op1-1;
}

void brpos( int op1 )
{
    if (acc > 0)
    {
        pc = op1-1;
    }
}

void brzero( int op1 )
{
    if (acc == 0)
    {
        pc = op1-1;
    }
}

void brneg( int op1 )
{
    if (acc < 0)
    {
        pc = op1-1;
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
            scanf("%d", &acc); // (WIP) Mudar pra fgets e fazer trim?
            break;
        default:
            printf("Unknown system call\n");
    }
}

// Reads one line of code instructions
void read_code( char *instruction )
{
    char delim[] = "\n\r ";
    char *temp;
    char operation_string[128];
    char variable[128];
    int opcode = -1;
    int value = -1;

    int b_immediate = 0;
    int b_label = 0;

    if (instruction[strlen(instruction) - 1] == ':')
    {
        b_label = 1;
    }

    if ( (temp = strtok(instruction, delim)) != NULL )
    {

        for (int i = 0; temp[i] != '\0'; i++)
        {
            temp[i] = toupper(temp[i]);
        }
        
        if (b_label)
        {
            temp[strlen(temp) - 1] = '\0';
            struct Label temp_label;
            strcpy(temp_label.name, temp);
            temp_label.line_num = instruction_count;
            // (WIP)
            printf("LABEL NAME: %s, LABEL LINE:%d\n", temp_label.name, temp_label.line_num);
            labels[label_count] = temp_label;
            label_count++;
            return;
        }

        strcpy(operation_string, temp);
    }
    
    if ( (temp = strtok(NULL, delim)) != NULL )
    {
        while(isspace((unsigned char)*temp)) temp++;

        for (int i = 0; temp[i] != '\0'; i++)
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
        struct Instruction temp_instruction;
        temp_instruction.operation = opcode;

        // Checks if the value is an immediate (syscall parameters are considered immediate here) or a variable
        if (b_immediate)
        {
            temp_instruction.type = IMMEDIATE;
            temp_instruction.immediate = value;
        }

        else if (opcode == 10)
        {
            temp_instruction.type = IMMEDIATE;
            temp_instruction.immediate = atoi(variable);
        }

        // Is Branch
        else if (opcode > 5)
        {
            // Finds label num in a second iteration
            branches[branch_count].instruction_num = instruction_count;
            strcpy(branches[branch_count].label_name, variable);
            branch_count++;

            temp_instruction.type = IMMEDIATE;
        }

        else
        {            
            struct Variable *variable_pointer = NULL;
            for (size_t i = 0; i < variable_count; i++)
            {
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

            temp_instruction.type = VAR_POINTER;
            temp_instruction.var_pointer = variable_pointer;
        }

        instructions[instruction_count] = temp_instruction;
        instruction_count++;
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
    int value = -1;

    if ( (temp = strtok(instruction, delim)) != NULL )
    {        
        for (int i = 0; temp[i] != '\0'; i++)
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
    char *instruction = NULL;
    char temp_code[512][256];
    size_t temp_code_i = 0;

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
        if (*instruction == '\0')
        {
            continue;
        }

        // Remove trailing spaces
        char *end = instruction + strlen(instruction) - 1;
        while ( end > instruction && (isspace(end[0]) || end[0] == '\r' || end[0] == '\n' || end[0] == '\t') )
        {
            end--;
        }
        end[1] = '\0';

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

    // Finds line number of branches through labels
    for (size_t i = 0; i < branch_count; i++)
    {
        int temp_label_num = -1;
        for (size_t j = 0; j < label_count; j++)
        {
            if (strcmp(branches[i].label_name, labels[j].name) == 0)
            {
                temp_label_num = labels[j].line_num;
            }
        }

        if (temp_label_num == -1)
        {
            perror("label not found\n");
            return;
        }

        instructions[branches[i].instruction_num].immediate = temp_label_num;
    }
}

int execute_instruction( struct Instruction *instruction )
{
    switch (instruction->operation)
    {        
        case (ADD):
        {
            if ( instruction->type == IMMEDIATE )
            {
                add(instruction->immediate);
                break;
            }

            add( instruction->var_pointer->value );

            break;
        }
        case (SUB):
        {
            if ( instruction->type == IMMEDIATE )
            {
                sub(instruction->immediate);
                break;
            }

            sub( instruction->var_pointer->value );

            break;
        }
        case (MULT):
        {
            if ( instruction->type == IMMEDIATE )
            {
                mult(instruction->immediate);
                break;
            }

            mult( instruction->var_pointer->value );

            break;
        }
        case (DIV):
        {
            if ( instruction->type == IMMEDIATE )
            {
                divide(instruction->immediate);
                break;
            }

            divide( instruction->var_pointer->value );

            break;
        }
        case (LOAD):
        {
            load( instruction->var_pointer->value );

            break;
        }
        case (STORE):
        {
            store( &(instruction->var_pointer->value) );

            break;
        }
        case (BRANY):
        {
            brany(instruction->immediate);

            break;
        }
        case (BRPOS):
        {
            brpos(instruction->immediate);

            break;
        }
        case (BRZERO):
        {
            brzero(instruction->immediate);

            break;
        }
        case (BRNEG):
        {
            brneg(instruction->immediate);

            break;
        }
        case (SYSCALL):
        {
            syscall(instruction->immediate);

            break;
        }
        case (OP_ERROR):
        {
            perror("OPCODE not found");
        }
        default:
        {
            break;
        }
    }

    return 0;
}

int run_program()
{
    size_t i = 0;

    while ( !(instructions[pc].operation == 10 && instructions[pc].immediate == 0) )
    {
        if (instructions[pc].type == IMMEDIATE)
        {
            printf("%lu: instruction %lu: OPCODE %d-%d", i, pc, instructions[pc].operation, instructions[pc].immediate);    
        }
        else
        {
            printf("%lu: instruction %lu: OPCODE %d - %s", i, pc, instructions[pc].operation, instructions[pc].var_pointer->name);
        }

        if (instructions[pc].operation > 5)
        {
            printf("\n");
        }

        execute_instruction(&instructions[pc]);
        
        if (instructions[pc].operation <= 5)
        {
            printf(" -> ");
            for (size_t j = 0; j < variable_count; j++)
            {
                printf("%s = %d", variables[j].name, variables[j].value);

                if (j != variable_count-1)
                {
                    printf(", ");
                }
            }
            printf("\n");
        }

        pc++;
        i++;
    }

    acc = 0;
    pc = 0;
    
    return 0;
}

int program_setup( FILE* fileptr )
{
    // Counters
    variable_count = 0; // Counter for number of variables in data
    instruction_count = 0; // "Virtual" memory for instructions (maybe refactor?)
    label_count = 0;
    branch_count = 0;

    acc = 0;
    pc = 0;

    read_instructions(fileptr);

    // Test: Prints all variables and instructions on virtual memory
    printf("Data:\n");
    for (size_t i = 0; i < variable_count; i++)
    {
        printf(" %lu: %s = %d\n", i, variables[i].name, variables[i].value);
    }
    printf("Code Instructions:\n");
    for (size_t i = 0; i < instruction_count; i++)
    {
        printf(" %lu: OPCODE %d: %s%s%d\n",
                        i,
                        instructions[i].operation,
                        instructions[i].type==IMMEDIATE? "" : instructions[i].var_pointer->name,
                        instructions[i].type==IMMEDIATE? "" : " = ",
                        instructions[i].type==IMMEDIATE? instructions[i].immediate : instructions[i].var_pointer->value
                    );
    }
    printf("Labels:\n");
    for (size_t i = 0; i < label_count; i++)
    {
        printf(" %lu: Label %s - line %d\n", i, labels[i].name, labels[i].line_num);
    }

    printf("Branches:\n");
    for (size_t i = 0; i < branch_count; i++)
    {
        printf(" %lu: Branch %s - instruction %lu\n", i, branches[i].label_name, branches[i].instruction_num);
    }

    return 0;
}