#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "program.h"
#include "scheduler.h"

#define BUFFER_SIZE 4096
#define PROGRAM_DEBUG 2 // SETS DEBUG MODE (1 = programs will print all instructions and some messages,
//                                           2 = programs will print all instructions, messages and variables)

// Struct to pass user values to the programs on their setup
struct User_input
{
    size_t arrival_time;
    size_t processing_time;
    size_t deadline;
};

// Arrays for storing programs and the user input for setup
struct Program programs[128];
struct User_input user_input[128];
size_t program_count;
size_t user_input_count;

// Sets debug mode
int program_debug;

FILE* fileptr;

// Reads user values from ./programs/uservalues.txt to be used on program setup
int read_user_values(char *filename);

// Reads program files into instructions and sets them up for the scheduler
int read_program_file(char *filename);

// Simple function to ask for typed user input if necessary (i.e. if not in uservalues.txt)
struct User_input ask_user_input(size_t p_count);

// Function to parse strings to uints (no negative numbers!)
int parse_uint(char *string, int *unsigned_int);

int main( int argc, char **argv )
{
    // SETS DEBUG MODE (will print all instructions)
    program_debug = PROGRAM_DEBUG;

    user_input_count = 0;
    program_count = 0;

    FILE *fileptr;
    DIR *directory;
    struct dirent *entry;

    directory = opendir("./programs");

    if (directory == NULL)
    {
        printf("Error opening directory\n");
        return 1;
    }

    // Checks for a file with user values for arrival_time, processing_time and deadline
    // Should be in ./programs/uservalues.txt
    // If there's not enough lines in the file, it will ask the user for the remaining values
    // It won't work if a line doesn't have at least 3 values (values beyond that per line will be discarded)
    while ((entry = readdir(directory)) != NULL)
    {
        if(strstr(entry->d_name, "uservalues.txt") )
        {
            char filepath[266];
            snprintf(filepath, 266, "programs/%s", entry->d_name);
            read_user_values(filepath);
        }
    }
    closedir(directory);

    directory = opendir("./programs");

    if (directory == NULL)
    {
        printf("Error opening directory\n");
        return 1;
    }

    while ((entry = readdir(directory)) != NULL)
    {
        char filepath[266];
        if(strstr(entry->d_name, "uservalues.txt") )
        {
            continue;
        }
        if(strstr(entry->d_name, ".txt") )
        {
            snprintf(filepath, 266, "programs/%s", entry->d_name);
            read_program_file( filepath );
        }
    }
    closedir(directory);

    // Sets up and executes scheduler
    scheduler_setup(programs, program_count);

    scheduler_execute_programs();

    return 0;
}

// Reads txt program files and sets them up to be run by the scheduler
int read_program_file(char* filepath)
{
    fileptr = fopen(filepath, "r");
    if (fileptr == NULL)
    {
        printf("File not found.\nExiting program...\n");
        return 1;
    }

    // If there is user values in the array to input into the program setup
    if (program_count < user_input_count)
    {
        program_setup(&programs[program_count], fileptr, program_debug,
            user_input[program_count].arrival_time,
            user_input[program_count].processing_time,
            user_input[program_count].deadline);
    }
    // If there aren't, it will ask the user for each of the 3
    else
    {
        struct User_input temp_user_input = ask_user_input(program_count);
        program_setup(&programs[program_count], fileptr, program_debug,
            temp_user_input.arrival_time,
            temp_user_input.processing_time,
            temp_user_input.deadline);
    }

    program_count++;
    fclose(fileptr);

    return 1;
}

// Reads user values from ./programs/uservalues.txt to be used on program setup
int read_user_values(char *filename)
{
    fileptr = fopen(filename, "r");
    if (fileptr == NULL)
    {
        printf("User values file not found.\n");
        return 0;
    }

    char s[256];
    char *delim = "\n\r,; ";
    char *temp;
    int value;
    size_t i;

    while (fgets(s, 256, fileptr))
    {
        if ( (temp = strtok(s, delim)) != NULL )
        {
            if( parse_uint(temp, &value))
            {
                user_input[user_input_count].arrival_time = (size_t)value;
            }
        }
        if ( (temp = strtok(NULL, delim)) != NULL )
        {
            if( parse_uint(temp, &value))
            {
                user_input[user_input_count].processing_time = (size_t)value;
            }
        }
        if ( (temp = strtok(NULL, delim)) != NULL )
        {
            if( parse_uint(temp, &value))
            {
                user_input[user_input_count].deadline = (size_t)value;
            }
        }
        user_input_count++;                
    }
    fclose(fileptr);

    return 1;
}

// Asks for user input on processing time, arrival time and automatic user input
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
    user_input.arrival_time = value;

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
    user_input.processing_time = value;

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
    user_input.deadline = value;

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