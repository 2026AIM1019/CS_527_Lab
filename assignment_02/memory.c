#include <stdio.h>
#include <stdint.h>

#include "memory.h"

char Instruction[256];
char Data[4096];


/*
 * ============================================================
 * INITIALIZE MEMORY
 * ============================================================
 */

void initialize(const char *program_file, const char *data_file)
{
    FILE *fp;

    int value;
    int index = 0;


    /*
     * --------------------------------------------------------
     * Load Instruction Memory
     * --------------------------------------------------------
     */

    fp = fopen(program_file, "r");

    if (fp == NULL)
    {
        printf("Error opening %s\n", program_file);
        return;
    }

    while (fscanf(fp, "%x", &value) == 1 && index < 256)
    {
        Instruction[index++] = (char)value;
    }

    fclose(fp);


    /*
     * --------------------------------------------------------
     * Load Data Memory
     * --------------------------------------------------------
     */

    fp = fopen(data_file, "r");

    if (fp == NULL)
    {
        printf("Error opening %s\n", data_file);
        return;
    }

    index = 0;

    while (fscanf(fp, "%x", &value) == 1 && index < 4096)
    {
        Data[index++] = (char)value;
    }

    fclose(fp);


    printf("Memory Initialized Successfully.\n");
}


/*
 * ============================================================
 * READ 32-BIT VALUE FROM MEMORY
 * ============================================================
 *
 * Memory is byte addressable.
 *
 * Therefore a 32-bit integer occupies:
 *
 * Data[address]
 * Data[address + 1]
 * Data[address + 2]
 * Data[address + 3]
 *
 * We use little-endian representation.
 * ============================================================
 */

int32_t read_memory_32(int address)
{
    uint32_t value;


    /*
     * Check that all four bytes are inside memory.
     */

    if (address < 0 || address + 3 >= 4096)
    {
        printf(
            "Memory Read Error: Invalid address %d\n",
            address
        );

        return 0;
    }


    value =
        ((uint32_t)(unsigned char)Data[address]) |
        ((uint32_t)(unsigned char)Data[address + 1] << 8) |
        ((uint32_t)(unsigned char)Data[address + 2] << 16) |
        ((uint32_t)(unsigned char)Data[address + 3] << 24);


    return (int32_t)value;
}


/*
 * ============================================================
 * WRITE 32-BIT VALUE TO MEMORY
 * ============================================================
 *
 * A 32-bit value is split into four bytes.
 * ============================================================
 */

void write_memory_32(int address, int32_t value)
{
    uint32_t uvalue;


    /*
     * Check that all four bytes are inside memory.
     */

    if (address < 0 || address + 3 >= 4096)
    {
        printf(
            "Memory Write Error: Invalid address %d\n",
            address
        );

        return;
    }


    uvalue = (uint32_t)value;


    Data[address] =
        (char)(uvalue & 0xFF);

    Data[address + 1] =
        (char)((uvalue >> 8) & 0xFF);

    Data[address + 2] =
        (char)((uvalue >> 16) & 0xFF);

    Data[address + 3] =
        (char)((uvalue >> 24) & 0xFF);
}


/*
 * ============================================================
 * FINALIZE MEMORY
 * ============================================================
 *
 * Save all 4096 bytes back into data.byte.
 * ============================================================
 */

void finalize()
{
    FILE *fp;


    fp = fopen("data.byte", "w");

    if (fp == NULL)
    {
        printf("Error writing data.byte\n");
        return;
    }


    for (int i = 0; i < 4096; i++)
    {
        fprintf(
            fp,
            "%02X\n",
            (unsigned char)Data[i]
        );
    }


    fclose(fp);

    printf("Data Memory Saved.\n");
}

