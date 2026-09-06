#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compiler.h"
#include "memory.h"
#include "processor.h"

// create a new filename in the same directory as another file.
static void make_sibling_path(const char *source, const char *name,
                              char *out, size_t out_size)
{
    const char *slash = strrchr(source, '/');
#ifdef _WIN32
    const char *bslash = strrchr(source, '\\');
    if (bslash && (!slash || bslash > slash)) slash = bslash;
#endif

    if (slash)
    {
        size_t n = (size_t)(slash - source);
        if (n + 1 + strlen(name) + 1 > out_size) {
            out[0] = '\0';
            return;
        }
        memcpy(out, source, n);
        out[n] = '/';
        strcpy(out + n + 1, name);
    }
    else
    {
        snprintf(out, out_size, "%s", name);
    }
}

int main(int argc, char **argv)
{
    char program_file[512];
    char data_file[512];

    if (argc < 2 || argc > 3)
    {
        printf("Usage:\n");
        printf("  %s program.txt [data.byte]\n", argv[0]);
        printf("  %s program.byte data.byte\n", argv[0]);
        return 1;
    }

    const char *input_program = argv[1];

    /* If source is supplied, compile it to program.byte in the same directory. */
    const char *dot = strrchr(input_program, '.');
    int is_source = dot && strcmp(dot, ".txt") == 0;

    if (is_source)
    {
        make_sibling_path(input_program, "program.byte",
                          program_file, sizeof(program_file));
        if (program_file[0] == '\0') {
            printf("Error: Program path is too long.\n");
            return 1;
        }

        if (argc == 3)
            snprintf(data_file, sizeof(data_file), "%s", argv[2]);
        else
            make_sibling_path(input_program, "data.byte",
                              data_file, sizeof(data_file));

        compile(input_program, program_file);
    }
    else
    {
        snprintf(program_file, sizeof(program_file), "%s", input_program);

        if (argc != 3)
        {
            printf("Error: Bytecode execution requires data.byte as second argument.\n");
            return 1;
        }

        snprintf(data_file, sizeof(data_file), "%s", argv[2]);
    }

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
