#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "program.h"

// Global variables for temporary storage
int program_id_count = 0;
int branch_count;
int label_count;
struct Branch *branches;
struct Label *labels;

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
void load( struct Program *program, struct Variable *op1 )
{
    program->acc = op1->value;
}

void store( struct Program *program, struct Variable *op1 )
{
    op1->value = program->acc;
}

void add( struct Program *program, int op1 )
{
    program->acc = program->acc + op1;
}

void sub( struct Program *program, int op1 )
{
    program->acc = program->acc - op1;
}

void mult( struct Program *program, int op1 )
{
    program->acc = program->acc * op1;
}

void divide( struct Program *program, int op1 ) // There's already a "div" in stdlib.h
{
    program->acc = program->acc / op1;
}

void brany( struct Program *program, int op1 )
{
    program->pc = op1-1;
}

void brpos( struct Program *program, int op1 )
{
    if (program->acc > 0)
    {
        program->pc = op1-1;
    }
}

void brzero( struct Program *program, int op1 )
{
    if (program->acc == 0)
    {
        program->pc = op1-1;
    }
}

void brneg( struct Program *program, int op1 )
{
    if (program->acc < 0)
    {
        program->pc = op1-1;
    }
}

void syscall( struct Program *program, int index )
{
    switch (index)
    {
        case 0:
            break;
        case 1:
            printf("PID %lu: prints %d\n", program->id, program->acc);
            break;
        case 2:
            printf("PID %lu: user types %d\n", program->id, program->auto_user_input);
            program->acc = program->auto_user_input;
            break;
        default:
            printf("Unknown system call, halting program %lu\n--------------------------------------\n", program->id);
    }
}

// Reads one line of code instructions
void read_code( struct Program *program, char *instruction )
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
            temp_label.line_num = program->instruction_count;
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

        // Checks if the value is an immediate (syscall parameters are considered immediate) or a variable
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
            branches[branch_count].instruction_num = program->instruction_count;
            strcpy(branches[branch_count].label_name, variable);
            branch_count++;

            temp_instruction.type = IMMEDIATE;
        }

        else
        {            
            struct Variable *variable_pointer = NULL;
            for (size_t i = 0; i < program->variable_count; i++)
            {
                if (strcmp(variable, program->variables[i].name) == 0)
                {
                    variable_pointer = &(program->variables[i]); /// PROBLEM?!
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

        program->instructions[program->instruction_count] = temp_instruction;
        program->instruction_count++;
    }
    else
    {
        perror("OPCODE not found!\n");
    }
}

// Reads one line of data instructions
void read_data( struct Program *program, char *instruction)
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

    program->variables[program->variable_count] = temp_variable;
    program->variable_count++;
}

// Reads line of instructions, one by one, calling the respective function for its type
void read_instructions( struct Program *program, FILE *fileptr )
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
            read_data( program, instruction );
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
        read_code( program, temp_code[i] );
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

        program->instructions[branches[i].instruction_num].immediate = temp_label_num;
    }
}

// Execute one program instruction based on its program counter
int execute_instruction( struct Program *program )
{
    struct Instruction *instruction = &program->instructions[program->pc];

    switch (instruction->operation)
    {        
        case (ADD):
        {
            if ( instruction->type == IMMEDIATE )
            {
                add(program, instruction->immediate);
                break;
            }

            add( program, instruction->var_pointer->value );

            break;
        }
        case (SUB):
        {
            if ( instruction->type == IMMEDIATE )
            {
                sub(program, instruction->immediate);
                break;
            }

            sub( program, instruction->var_pointer->value );

            break;
        }
        case (MULT):
        {
            if ( instruction->type == IMMEDIATE )
            {
                mult(program, instruction->immediate);
                break;
            }

            mult(program, instruction->var_pointer->value );

            break;
        }
        case (DIV):
        {
            if ( instruction->type == IMMEDIATE )
            {
                divide(program, instruction->immediate);
                break;
            }

            divide(program, instruction->var_pointer->value );

            break;
        }
        case (LOAD):
        {
            load( program, instruction->var_pointer );

            break;
        }
        case (STORE):
        {
            store( program, instruction->var_pointer );

            break;
        }
        case (BRANY):
        {
            brany(program, instruction->immediate);

            break;
        }
        case (BRPOS):
        {
            brpos(program, instruction->immediate);

            break;
        }
        case (BRZERO):
        {
            brzero(program, instruction->immediate);

            break;
        }
        case (BRNEG):
        {
            brneg(program, instruction->immediate);

            break;
        }
        case (SYSCALL):
        {
            syscall(program, instruction->immediate);

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

    return 1;
}

// Runs the program, executes and prints every instruction and then increments its PC
int run_program( struct Program *program )
{
    program->b_running = 1;
    int b_program_stop = 0;
    size_t i = 0;

    printf("instruction count = %lu\n", program->instruction_count);
    printf("variable count = %lu\n", program->variable_count);

    // Program stops after every syscall, and will have to be given permission by the scheduler to run again
    while ( program->b_running )
    {
        size_t program_pc = program->pc;
        struct Instruction *program_instruction = &program->instructions[program_pc];

        if (program_instruction->type == IMMEDIATE)
        {
            printf("%lu: instruction %lu: OPCODE %d-%d", i, program_pc, program_instruction->operation, program_instruction->immediate);    
        }
        else
        {
            printf("%lu: instruction %lu: OPCODE %d - %s", i, program_pc, program_instruction->operation, program_instruction->var_pointer->name);
        }

        if (program->instructions[program->pc].operation > 5)
        {
            printf("\n");
        }

        execute_instruction(program);
        
        if (program_instruction->operation <= 5)
        {
            printf(" -> ");
            for (size_t j = 0; j < program->variable_count; j++)
            {
                printf("%s = %d", program->variables[j].name, program->variables[j].value);

                if (j != program->variable_count-1)
                {
                    printf(", ");
                }
            }
            printf("\n");
        }

        // if ( program_instruction->operation == 10 && program_instruction->immediate == 0 )
        // {
        //     program->b_running = 0;
        //     b_program_end = 1;
        //     break;
        // }
        if ( program_instruction->operation == 10 ) // Interrupt, will be resumed if it has an early deadline
        {
            program->b_running = 0;
            b_program_stop = 1;

            if (program_instruction->immediate == 0)
            {
                program->b_finished = 1;
            }
        }

        program->pc++;
        i++;
    }

    for (size_t j = 0; j < program->variable_count; j++)
    {
        printf("%s = %d", program->variables[j].name, program->variables[j].value);

        if (j != program->variable_count-1)
        {
            printf(", ");
        }
    }
    printf("\n");

    if (!program->b_running)
    {
        printf("Program %lu halted at instruction %lu/%lu\n--------------------------------------\n",
            program->id, program->pc, program->instruction_count);
    }

    if (program->b_finished == 1)
    {
        program->pc = 0;
        program->acc = 0;
    }
    
    return 1;
}

int calculate_deadline( struct Program *program )
{
    program->pc = 0;
    program->acc = 0;
    size_t syscall_count = 1;

    while(1)
    {
        execute_instruction(program);
        if ( program->instructions[program->pc].operation == 10 )
        {
            if (program->instructions[program->pc].immediate == 0)
            {
                break;
            }
            syscall_count++;
        }
        program->pc++;
    }

    program->pc = 0;
    program->acc = 0;

    program->deadline = syscall_count * program->processing_time;
    printf("PID %lu: syscall count = %lu, deadline = %lu\n", program->id, syscall_count, program->deadline);

    return 1;
}

int program_setup( struct Program *program, FILE* fileptr, size_t processing_time, int auto_user_input )
{
    program->id = program_id_count;
    branches = malloc( sizeof(struct Branch) * 256 );
    labels = malloc(sizeof(struct Label) * 256);

    // Counters
    program->variable_count = 0; // Counter for number of variables in data
    program->instruction_count = 0;
    label_count = 0;
    branch_count = 0;

    program->b_running = 0;
    program->b_finished = 0;
    program->acc = 0;
    program->pc = 0;
    program->processing_time = processing_time;
    program->auto_user_input = auto_user_input;

    read_instructions(program, fileptr);

    // Test: Prints all variables and instructions on virtual memory
    // printf("Data:\n");
    // for (size_t i = 0; i < program->variable_count; i++)
    // {
    //     printf(" %lu: %s = %d\n", i, program->variables[i].name, program->variables[i].value);
    // }
    // printf("Code Instructions:\n");
    // for (size_t i = 0; i < program->instruction_count; i++)
    // {
    //     printf(" %lu: OPCODE %d: %s%s%d\n",
    //                     i,
    //                     program->instructions[i].operation,
    //                     program->instructions[i].type==IMMEDIATE? "" : program->instructions[i].var_pointer->name,
    //                     program->instructions[i].type==IMMEDIATE? "" : " = ",
    //                     program->instructions[i].type==IMMEDIATE? program->instructions[i].immediate : program->instructions[i].var_pointer->value
    //                 );
    // }
    // printf("Labels:\n");
    // for (size_t i = 0; i < label_count; i++)
    // {
    //     printf(" %lu: Label %s - line %d\n", i, labels[i].name, labels[i].line_num);
    // }
    //
    // printf("Branches:\n");
    // for (size_t i = 0; i < branch_count; i++)
    // {
    //     printf(" %lu: Branch %s - instruction %lu\n", i, branches[i].label_name, branches[i].instruction_num);
    // }

    free(branches);
    free(labels);

    calculate_deadline(program);

    program_id_count++;

    return 1;
}