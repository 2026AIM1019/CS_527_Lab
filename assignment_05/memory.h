#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define NP 4
#define MAX_PROC NP
#define MEMSIZE 8192
#define PAGESIZE 512
#define NUM_LOGICAL_PAGES (1024 / PAGESIZE + 4096 / PAGESIZE)
#define NUM_PHYSICAL_PAGES (MEMSIZE / PAGESIZE)
#define INSTRUCTION_LOGICAL_SIZE 1024
#define DATA_LOGICAL_SIZE 4096

extern unsigned char memory[MEMSIZE];
extern unsigned char pageTable[NP][NUM_LOGICAL_PAGES];
extern unsigned char freePages[NUM_PHYSICAL_PAGES];

/* Lab 5 MMU / memory management API. */
void memory_system_init(void);
int getPhysicallAddress(int proc_id, int isFetch, int address);
int getFreePage(void);
void releaseProcessPages(int proc_id);

int initialize(int proc_id, const char *program_file, const char *data_file);
int finalize(int proc_id, const char *data_file);

int32_t read_memory_32(int proc_id, int address);
void write_memory_32(int proc_id, int address, int32_t value);

#endif
