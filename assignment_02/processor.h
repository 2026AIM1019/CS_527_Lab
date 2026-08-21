
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>

extern int32_t Register[256];

extern int PC;

extern int opcode;
extern int dest;
extern int src1;
extern int src2;

extern int Z;
extern int N;
extern int C;
extern int V;

extern int end_of_simulation;

void reset();
void fetch();
void decode();
void execute();

#endif
