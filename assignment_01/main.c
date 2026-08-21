#include <stdio.h>
#include <string.h>

// My Custom header files
#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main()
{
    int choice = 0;  // To Choose the Test Case

    char source_file[100];
    char program_file[100];
    char data_file[100];

    printf("Choose Test Program\n\n");

    printf("1. Sum of N Numbers\n");
    printf("2. Complex Multiplication\n");
    printf("3. Determinant of 3x3 Matrix\n");

    printf("\nEnter your choice : ");
    scanf("%d", &choice);
    
    sprintf(source_file, "tests/test_%02d/program.txt", choice);
    sprintf(program_file, "tests/test_%02d/program.byte", choice);
    sprintf(data_file, "tests/test_%02d/data.byte", choice);

    compile(source_file, program_file);

    initialize(program_file, data_file);

    reset();
    
    
    while (!end_of_simulation)
    {
        fetch();
        decode();
        execute();
    }

    printf("\nExecution Completed.\n");

    finalize(data_file);

    return 0;
}
