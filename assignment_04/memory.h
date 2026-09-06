#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define NP 4
#define INSTRUCTION_MEMORY_SIZE 256
#define DATA_MEMORY_SIZE 4096

extern unsigned char Instruction[NP][INSTRUCTION_MEMORY_SIZE];
extern unsigned char Data[NP][DATA_MEMORY_SIZE];

void initialize(int proc_id, const char *program_file, const char *data_file);
void finalize(int proc_id, const char *data_file);
int32_t read_memory_32(int proc_id, int address);
void write_memory_32(int proc_id, int address, int32_t value);

#endif
