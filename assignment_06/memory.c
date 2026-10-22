#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "memory.h"
#include "processor.h"

unsigned char memory[MEMSIZE];
unsigned char freePages[NUM_PHYSICAL_PAGES];

static int page_index(int isFetch, int address)
{
    if (address < 0)
        return -1;
    if (isFetch)
        return address / PAGESIZE;
    return address / PAGESIZE + INSTRUCTION_LOGICAL_SIZE / PAGESIZE;
}

void memory_system_init(void)
{
    int i;
    memset(memory, 0, sizeof(memory));
    memset(freePages, 0, sizeof(freePages));
    freePages[0] = 1;
    for (i = 0; i < NP; ++i)
        PTBR[i] = -1;
}

static int page_entry(int proc_id, int index)
{
    int address;
    if (proc_id < 0 || proc_id >= NP || index < 0 || index >= NUM_LOGICAL_PAGES)
        return -1;
    if (PTBR[proc_id] < 0 || PTBR[proc_id] + index >= PAGESIZE)
        return -1;
    address = PTBR[proc_id] + index;
    return memory[address];
}

static void set_page_entry(int proc_id, int index, int frame)
{
    if (proc_id < 0 || proc_id >= NP || index < 0 || index >= NUM_LOGICAL_PAGES)
        return;
    memory[PTBR[proc_id] + index] = (unsigned char)frame;
}

int getPhysicallAddress(int proc_id, int isFetch, int address)
{
    int index;
    int frame;

    if (proc_id < 0 || proc_id >= NP || address < 0 || PTBR[proc_id] < 0) {
        fprintf(stderr, "MMU ERROR: invalid process or address\n");
        return -1;
    }

    if (isFetch) {
        if (address >= INSTRUCTION_LOGICAL_SIZE)
            return -1;
    } else if (address >= DATA_LOGICAL_SIZE) {
        return -1;
    }

    index = page_index(isFetch, address);
    frame = page_entry(proc_id, index);
    if (frame <= 0 || frame >= NUM_PHYSICAL_PAGES || freePages[frame] == 0) {
        fprintf(stderr, "MMU ERROR: unmapped logical page %d\n", index);
        return -1;
    }

    return frame * PAGESIZE + address % PAGESIZE;
}

int getFreePage(void)
{
    int i;
    for (i = 1; i < NUM_PHYSICAL_PAGES; ++i) {
        if (freePages[i] == 0) {
            freePages[i] = 1;
            return i;
        }
    }
    fprintf(stderr, "ERROR: No free physical frame available\n");
    return -1;
}

void releaseProcessPages(int proc_id)
{
    int i;
    int frame;

    if (proc_id < 0 || proc_id >= NP)
        return;
    if (PTBR[proc_id] < 0)
        return;

    for (i = 0; i < NUM_LOGICAL_PAGES; ++i) {
        frame = page_entry(proc_id, i);
        if (frame > 0 && frame < NUM_PHYSICAL_PAGES)
            freePages[frame] = 0;
        memory[PTBR[proc_id] + i] = 0;
    }
    PTBR[proc_id] = -1;
}

static int load_region(int proc_id, const char *file_name, int isFetch, int max_bytes)
{
    FILE *fp;
    int value;
    int logical_offset = 0;

    fp = fopen(file_name, "r");
    if (!fp)
        return -1;

    while (logical_offset < max_bytes && fscanf(fp, "%x", &value) == 1) {
        int index = page_index(isFetch, logical_offset);
        int frame;
        int physical;

        if (index < 0 || index >= NUM_LOGICAL_PAGES) {
            fclose(fp);
            return -1;
        }

        frame = page_entry(proc_id, index);
        if (frame == 0) {
            frame = getFreePage();
            if (frame < 0) {
                fclose(fp);
                return -2;
            }
            set_page_entry(proc_id, index, frame);
            memset(memory + frame * PAGESIZE, 0, PAGESIZE);
        }

        physical = frame * PAGESIZE + logical_offset % PAGESIZE;
        memory[physical] = (unsigned char)(value & 0xff);
        ++logical_offset;
    }

    fclose(fp);
    return logical_offset;
}

int initialize(int proc_id, const char *program_file, const char *data_file)
{
    int rc;

    if (proc_id < 0 || proc_id >= NP || !program_file || !data_file)
        return -1;

    releaseProcessPages(proc_id);
    PTBR[proc_id] = proc_id * NUM_LOGICAL_PAGES;
    memset(memory + PTBR[proc_id], 0, NUM_LOGICAL_PAGES);

    rc = load_region(proc_id, program_file, 1, INSTRUCTION_LOGICAL_SIZE);
    if (rc < 0) {
        releaseProcessPages(proc_id);
        return rc;
    }

    rc = load_region(proc_id, data_file, 0, DATA_LOGICAL_SIZE);
    if (rc < 0) {
        releaseProcessPages(proc_id);
        return rc;
    }

    return 0;
}

int finalize(int proc_id, const char *data_file)
{
    FILE *fp;
    int logical;

    if (proc_id < 0 || proc_id >= NP || !data_file || PTBR[proc_id] < 0)
        return -1;

    fp = fopen(data_file, "w");
    if (!fp)
        return -1;

    for (logical = 0; logical < DATA_LOGICAL_SIZE; ++logical) {
        int index = page_index(0, logical);
        int frame = page_entry(proc_id, index);
        unsigned char value = 0;
        if (frame > 0 && frame < NUM_PHYSICAL_PAGES)
            value = memory[frame * PAGESIZE + logical % PAGESIZE];
        fprintf(fp, "%02X%s", value, logical % 4 == 3 ? "\n" : " ");
    }

    fclose(fp);
    releaseProcessPages(proc_id);
    return 0;
}

int32_t read_memory_32(int proc_id, int address)
{
    uint32_t value = 0;
    int p0 = getPhysicallAddress(proc_id, 0, address);
    int p1 = getPhysicallAddress(proc_id, 0, address + 1);
    int p2 = getPhysicallAddress(proc_id, 0, address + 2);
    int p3 = getPhysicallAddress(proc_id, 0, address + 3);

    if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0)
        return 0;

    value = (uint32_t)memory[p0]
          | ((uint32_t)memory[p1] << 8)
          | ((uint32_t)memory[p2] << 16)
          | ((uint32_t)memory[p3] << 24);
    return (int32_t)value;
}

void write_memory_32(int proc_id, int address, int32_t value)
{
    uint32_t v = (uint32_t)value;
    int p0 = getPhysicallAddress(proc_id, 0, address);
    int p1 = getPhysicallAddress(proc_id, 0, address + 1);
    int p2 = getPhysicallAddress(proc_id, 0, address + 2);
    int p3 = getPhysicallAddress(proc_id, 0, address + 3);

    if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0)
        return;

    memory[p0] = (unsigned char)(v & 0xff);
    memory[p1] = (unsigned char)((v >> 8) & 0xff);
    memory[p2] = (unsigned char)((v >> 16) & 0xff);
    memory[p3] = (unsigned char)((v >> 24) & 0xff);
}
