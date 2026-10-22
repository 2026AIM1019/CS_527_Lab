#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdio.h>
#include "memory.h"

extern int32_t Register[NP][256];
extern int32_t VectorRegister[NP][32][8];
extern int PC[NP];
extern int opcode, dest, src1, src2;
extern int Z[NP], N[NP], C[NP], V[NP];
extern int process_pid[NP];
extern int end_of_simulation[NP];
extern int PTBR[NP];
extern int proc_id;
extern FILE *fd_log;

void reset(int id);
void fetch(int id);
void decode(void);
void execute(int id);
void process_instructions(int id, int instruction_count);
void set_processor_pid(int id, int pid);
int load_instruction_map(int id, const char *map_file);
int write_execution_frequency(int id, const char *frequency_file);

#endif
