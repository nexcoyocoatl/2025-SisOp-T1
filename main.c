#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "program.h"
#include "scheduler.h"

struct Program programs[128];
size_t program_count;

FILE* fileptr;

int read_file(char* filename);

int main( int argc, char **argv )
{ 
    DIR *directory;
    struct dirent *entry;

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

    // run_program(&programs[5]);

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

    program_setup(&programs[program_count], fileptr, 3, 3); //(WIP) MUDAR
    program_count++;

    fclose(fileptr);

    return 0;
}