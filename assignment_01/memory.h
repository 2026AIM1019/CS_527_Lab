#ifndef MEMORY_H
#define MEMORY_H

extern char Instruction[256];
extern char Data[256];

void initialize(char program_file[], char data_file[]);
void finalize(char data_file[]);

#endif
