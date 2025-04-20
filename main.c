#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "program.h"
#include "scheduler.h"

#define BUFFER_SIZE 4096

struct Program programs[128];
size_t program_count;

struct User_input
{
    size_t processing_time;
    size_t auto_user_input;
    size_t arrival_time;
};

FILE* fileptr;

int read_file(char* filename);

struct User_input ask_user_input(size_t p_count);

int parse_uint(char *string, int *unsigned_int);

int main( int argc, char **argv )
{ 
    DIR *directory;
    struct dirent *entry;

    // TALVEZ NÃO FUNCIONE NO WINDOWS
    directory = opendir("./programs");

    if (directory == NULL)
    {
        printf("Error opening directory\n");
        return 1;
    }

    while ((entry = readdir(directory)) != NULL)
    {
        char filepath[266];
        if(strstr(entry->d_name, ".txt") )
        {
            snprintf(filepath, 266, "programs/%s", entry->d_name);
            read_file( filepath );
        }
    }

    scheduler_setup();
    scheduler_auto(programs, program_count);
    scheduler_print_creating();
    scheduler_execute_programs();

    return 0;
}

int read_file(char* filepath)
{
    fileptr = fopen(filepath, "r");
    if (fileptr == NULL)
    {
        printf("File not found.\nExiting program...\n");
        return 1;
    }

    //(WIP)
    // program_setup(&programs[program_count], fileptr, 3, 0, 3); // ASSIM FUNCIONA

    switch (program_count)
    {
        case 0:
            program_setup(&programs[program_count], fileptr, 3, 0, 3);
            break;
        case 1:
            program_setup(&programs[program_count], fileptr, 3, 0, 9);
            break;
        case 2:
            program_setup(&programs[program_count], fileptr, 3, 0, 18);
            break;
    }

    // struct User_input user_input = ask_user_input(program_count); // ASSIM NÃO
    // program_setup(&programs[program_count], fileptr, user_input.processing_time, user_input.arrival_time, user_input.auto_user_input);
    


    // if (program_count == 0)
    // {
    //     program_setup(&programs[program_count], fileptr, 3, 5)
    // }




    program_count++;
    fclose(fileptr);

    return 0;
}

// Asks for user input on processing time, arrival time and automatic user input

// (WIP) parece que valores altos estão dando problema, imagino que a implementacao do
// processing time esteja errada lá no scheduler
struct User_input ask_user_input(size_t p_count)
{
    struct User_input user_input;

    int b_parse = 0;
    int value = 0;
    do
    {
        printf("Type the processing time for PID %lu: ", p_count);

        char buffer[BUFFER_SIZE];

        fgets(buffer, BUFFER_SIZE, stdin);

        b_parse = parse_uint(buffer, &value);
    }
    while (!b_parse);
    user_input.processing_time = value;

    b_parse = 0;
    value = 0;
    do
    {
        printf("Type the arrival time for PID %lu: ", p_count);

        char buffer[BUFFER_SIZE];

        fgets(buffer, BUFFER_SIZE, stdin);

        b_parse = parse_uint(buffer, &value);
    }
    while (!b_parse);
    user_input.arrival_time = value;

    b_parse = 0;
    value = 0;
    do
    {
        printf("Type the automatic user input for PID %lu: ", p_count);

        char buffer[BUFFER_SIZE];

        fgets(buffer, BUFFER_SIZE, stdin);

        b_parse = parse_uint(buffer, &value);
    }
    while (!b_parse);
    user_input.auto_user_input = value;

    return user_input;
}

// Parses unsigned integers from user input
int parse_uint(char *string, int *unsigned_int)
{
    int i = 0;

    while(isspace(string[i]))
    {
        i++;
    }

    int length = strlen(string);    
    if (length == i) { return 0; }

    char integer_buffer[BUFFER_SIZE];
    int integer_chars = 0;

    // if signed or if alpha
    if (!isdigit(string[i]))
    {
        return 0;
    }
    
    while (i < length && !(isspace(string[i])))
    {
        if (isspace(string[i])) { return 0; }

        integer_buffer[integer_chars] = string[i];
        integer_chars++;
        i++;
    }

    integer_buffer[integer_chars] = '\0';

    while(isspace(string[i])) { i++; }

    if (string[i] != '\0') { return 0; }

    *unsigned_int = atoi(integer_buffer);

    return 1;
}