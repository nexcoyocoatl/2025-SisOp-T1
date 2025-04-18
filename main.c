#include <stdio.h>
#include "program.h"

int main( int argc, char **argv )
{ 
    FILE* fileptr = NULL;

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

    program_setup(fileptr);

    fclose(fileptr);

    run_program();

    return 0;
}