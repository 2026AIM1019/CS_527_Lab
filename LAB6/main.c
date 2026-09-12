#include <stdio.h>
#include "os.h"

int main(int argc, char **argv)
{
    if (argc > 1 && ((argc - 1) % 2 != 0)) {
        fprintf(stderr, "Usage: %s [program.txt data.byte] [program.txt data.byte] ...\n", argv[0]);
        return 1;
    }

    os_init();

    


    for (int i = 1; i < argc; i += 2) {
        if (loader(argv[i], argv[i + 1]) < 0) return 1;
    }

    scheduler();
    return 0;
}
