#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "memory.h"

unsigned char Instruction[NP][INSTRUCTION_MEMORY_SIZE];
unsigned char Data[NP][DATA_MEMORY_SIZE];

void initialize(int proc_id, const char *program_file, const char *data_file)
{
    FILE *fp;
    int value, index;

    if (proc_id < 0 || proc_id >= NP) return;

    memset(Instruction[proc_id], 0, INSTRUCTION_MEMORY_SIZE);
    memset(Data[proc_id], 0, DATA_MEMORY_SIZE);

    fp = fopen(program_file, "r");
    if (!fp) { fprintf(stderr, "OS: cannot open program %s\n", program_file); return; }
    index = 0;
    while (index < INSTRUCTION_MEMORY_SIZE && fscanf(fp, "%x", &value) == 1)
        Instruction[proc_id][index++] = (unsigned char)value;
    fclose(fp);

    fp = fopen(data_file, "r");
    if (!fp) { fprintf(stderr, "OS: cannot open data %s\n", data_file); return; }
    index = 0;
    while (index < DATA_MEMORY_SIZE && fscanf(fp, "%x", &value) == 1)
        Data[proc_id][index++] = (unsigned char)value;
    fclose(fp);
}

int32_t read_memory_32(int proc_id, int address)
{
    uint32_t v;
    if (proc_id < 0 || proc_id >= NP || address < 0 || address + 3 >= DATA_MEMORY_SIZE) {
        fprintf(stderr, "Memory read error: proc=%d address=%d\n", proc_id, address);
        return 0;
    }
    v = (uint32_t)Data[proc_id][address]
      | ((uint32_t)Data[proc_id][address + 1] << 8)
      | ((uint32_t)Data[proc_id][address + 2] << 16)
      | ((uint32_t)Data[proc_id][address + 3] << 24);
    return (int32_t)v;
}

void write_memory_32(int proc_id, int address, int32_t value)
{
    uint32_t v = (uint32_t)value;
    if (proc_id < 0 || proc_id >= NP || address < 0 || address + 3 >= DATA_MEMORY_SIZE) {
        fprintf(stderr, "Memory write error: proc=%d address=%d\n", proc_id, address);
        return;
    }
    Data[proc_id][address]     = (unsigned char)(v & 0xff);
    Data[proc_id][address + 1] = (unsigned char)((v >> 8) & 0xff);
    Data[proc_id][address + 2] = (unsigned char)((v >> 16) & 0xff);
    Data[proc_id][address + 3] = (unsigned char)((v >> 24) & 0xff);
}

void finalize(int proc_id, const char *data_file)
{
    FILE *fp;
    int i;
    if (proc_id < 0 || proc_id >= NP) return;
    fp = fopen(data_file, "w");
    if (!fp) { fprintf(stderr, "OS: cannot write data %s\n", data_file); return; }
    for (i = 0; i < DATA_MEMORY_SIZE; ++i)
        fprintf(fp, "%02X%s", Data[proc_id][i], (i % 4 == 3) ? "\n" : " ");
    fclose(fp);
}
