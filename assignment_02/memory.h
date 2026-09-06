#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

extern char Instruction[256];
extern char Data[4096];

void initialize(const char *program_file, const char *data_file);

void finalize(const char *data_file);

int32_t read_memory_32(int address);

void write_memory_32(int address, int32_t value);

#endif
