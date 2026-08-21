#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <program.txt>\n", argv[0]);
        return 1;
    }

    char program_file[512];
    char data_file[512];
    char directory[256];

    const char *last_slash = strrchr(argv[1], '/');

    if (last_slash != NULL)
    {
        size_t length = (size_t)(last_slash - argv[1]);

        if (length >= sizeof(directory))
        {
            printf("Error: Program path is too long.\n");
            return 1;
        }

        memcpy(directory, argv[1], length);
        directory[length] = '\0';

        snprintf(program_file, sizeof(program_file), "%s/program.byte", directory);
        snprintf(data_file, sizeof(data_file), "%s/data.byte", directory);
    }
    else
    {
        snprintf(program_file, sizeof(program_file), "program.byte");
        snprintf(data_file, sizeof(data_file), "data.byte");
    }

    compile(argv[1], program_file);
    initialize(program_file, data_file);
    reset();

    while (!end_of_simulation)
    {
        fetch();
        decode();
        execute();
    }

    finalize(data_file);

    return 0;
}
