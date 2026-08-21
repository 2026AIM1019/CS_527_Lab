#include <stdio.h>
#include "compiler.h"

void compile(char input_file[], char output_file[])
{
    FILE *input;
    FILE *output;

    input = fopen(input_file, "r");
    if (input == NULL)
    {
        printf("Error : Not able to open program.txt.\n");
        return;
    }
    // printf("Input File Readed\n");

    output = fopen(output_file, "w");
    if (output == NULL)
    {
        printf("Error : Not able to create program.byte.\n");
        fclose(input);
        return;
    }

    // printf("Output File Created\n");

    char line[100];

    while (fgets(line, sizeof(line), input))
    {
        int dest, src1, src2;

        char opcode;

        // printf("Compiling Line: %s", line);

        // Read
        if (sscanf(line, "Read x%d, %d", &dest, &src1) == 2)
        {
            fprintf(output, "5 %d %d 0\n", dest, src1);
            // printf("Read Instruction Compiled: %s", line);
            continue;
        }

        // Write
        if (sscanf(line, "Write x%d, %d", &dest, &src1) == 2)
        {
            fprintf(output, "6 %d %d 0\n", dest, src1);
            // printf("Write Instruction Compiled: %s", line);
            continue;
        }

        // Move
        if (sscanf(line, "x%d = %d", &dest, &src1) == 2)
        {
            fprintf(output, "7 %d %d 0\n", dest, src1);
            // printf("Move Instruction Compiled: %s", line);
            continue;
        }


        // Arithmetic operations
        if (sscanf(line, "x%d = x%d %c x%d", &dest, &src1, &opcode, &src2) == 4)
        {
            int op = 0;

            // printf("Compiling Arithmetic Instruction: %s", line);

            switch (opcode)
            {
            case '+':
                op = 1;
                break;
            case '-':
                op = 2;
                break;
            case '*':
                op = 3;
                break;
            case '/':
                op = 4;
                break;
            default:
                printf("Error : %c operator not supported in Line %s\n", opcode, line);
                fclose(input);
            }

            fprintf(output, "%d %d %d %d\n", op, dest, src1, src2);
            // printf("Arithmetic Instruction Compiled: %s", line);
        }
    }

        fprintf(output, "0 0 0 0\n");

        fclose(input);
        fclose(output);

        printf("Compilation successful. Output store to program.byte\n\n");
    
}
