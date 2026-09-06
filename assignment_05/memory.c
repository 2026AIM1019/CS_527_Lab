#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"

unsigned char memory[MEMSIZE];
unsigned char pageTable[NP][NUM_LOGICAL_PAGES];
unsigned char freePages[NUM_PHYSICAL_PAGES];

static int page_index(int isFetch, int address)
{
    if (address < 0) return -1;
    return isFetch
        ? address / PAGESIZE
        : address / PAGESIZE + INSTRUCTION_LOGICAL_SIZE / PAGESIZE;
}

void memory_system_init(void)
{
    memset(memory, 0, sizeof(memory));
    memset(pageTable, 0, sizeof(pageTable));
    memset(freePages, 0, sizeof(freePages));
    /* Frame 0 is permanently reserved. */
    freePages[0] = 1;
}

/*
 * Translate a logical address into a physical address.
 *
 * Fetches use logical instruction pages 0..1.
 * Data accesses use logical data pages beginning at 1024/PAGESIZE.
 */
int getPhysicallAddress(int proc_id, int isFetch, int address)
{
    int index;
    int page;
    int offset;

    if (proc_id < 0 || proc_id >= NP || address < 0) {
        fprintf(stderr, "MMU ERROR: invalid proc/address (%d, %d)\n",
                proc_id, address);
        return -1;
    }

    if (isFetch) {
        if (address >= INSTRUCTION_LOGICAL_SIZE) {
            fprintf(stderr, "MMU ERROR: instruction logical address %d out of range\n", address);
            return -1;
        }
    } else {
        if (address >= DATA_LOGICAL_SIZE) {
            fprintf(stderr, "MMU ERROR: data logical address %d out of range\n", address);
            return -1;
        }
    }

    index = page_index(isFetch, address);
    if (index < 0 || index >= NUM_LOGICAL_PAGES) {
        fprintf(stderr, "MMU ERROR: page-table index %d out of range\n", index);
        return -1;
    }

    page = pageTable[proc_id][index];
    if (page <= 0 || page >= NUM_PHYSICAL_PAGES || freePages[page] == 0) {
        fprintf(stderr, "MMU ERROR: unmapped logical page %d for PID slot %d\n",
                index, proc_id);
        return -1;
    }

    offset = address % PAGESIZE;
    return page * PAGESIZE + offset;
}

int getFreePage(void)
{
    /* Frame 0 is reserved and is never returned. */
    for (int i = 1; i < NUM_PHYSICAL_PAGES; ++i) {
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
    if (proc_id < 0 || proc_id >= NP) return;

    for (int i = 0; i < NUM_LOGICAL_PAGES; ++i) {
        int frame = pageTable[proc_id][i];
        if (frame > 0 && frame < NUM_PHYSICAL_PAGES) {
            freePages[frame] = 0;
        }
        pageTable[proc_id][i] = 0;
    }
}

/* Read a byte stream into successive physical frames for one logical region. */
static int load_region(int proc_id, const char *file_name,
                       int isFetch, int logical_base,
                       int max_bytes)
{
    FILE *fp = fopen(file_name, "r");
    int value;
    int logical_offset = 0;

    if (!fp) {
        fprintf(stderr, "OS: cannot open %s\n", file_name);
        return -1;
    }

    while (logical_offset < max_bytes && fscanf(fp, "%x", &value) == 1) {
        int logical_address = logical_base + logical_offset;
        int idx = page_index(isFetch, isFetch ? logical_offset : logical_offset);
        int frame;
        int physical;

        if (idx < 0 || idx >= NUM_LOGICAL_PAGES) {
            fclose(fp);
            return -1;
        }

        if (pageTable[proc_id][idx] == 0) {
            frame = getFreePage();
            if (frame < 0) {
                fclose(fp);
                return -2; /* out of physical memory */
            }
            pageTable[proc_id][idx] = (unsigned char)frame;
            memset(memory + frame * PAGESIZE, 0, PAGESIZE);
        }

        physical = getPhysicallAddress(proc_id, isFetch, isFetch ? logical_offset : logical_offset);
        if (physical < 0) {
            fclose(fp);
            return -1;
        }
        memory[physical] = (unsigned char)value;
        (void)logical_address;
        ++logical_offset;
    }

    fclose(fp);
    return logical_offset;
}

int initialize(int proc_id, const char *program_file, const char *data_file)
{
    int rc;

    if (proc_id < 0 || proc_id >= NP) return -1;

    /* A fresh task gets a fresh page table. */
    releaseProcessPages(proc_id);

    /*
     * Instruction bytes are mapped into logical instruction space.
     * Data bytes are mapped into logical data space (logical addresses
     * 0..4095 for the program's data references).
     */
    rc = load_region(proc_id, program_file, 1, 0, INSTRUCTION_LOGICAL_SIZE);
    if (rc < 0) {
        releaseProcessPages(proc_id);
        return rc;
    }

    rc = load_region(proc_id, data_file, 0, 0, DATA_LOGICAL_SIZE);
    if (rc < 0) {
        releaseProcessPages(proc_id);
        return rc;
    }

    return 0;
}

int finalize(int proc_id, const char *data_file)
{
    FILE *fp;

    if (proc_id < 0 || proc_id >= NP) return -1;

    fp = fopen(data_file, "w");
    if (!fp) {
        fprintf(stderr, "OS: cannot write data %s\n", data_file);
        return -1;
    }

    /*
     * Reconstruct the logical data address space from its page mappings.
     * The file contains exactly 4096 bytes, matching the Lab 2/3 format.
     */
    for (int logical = 0; logical < DATA_LOGICAL_SIZE; ++logical) {
        int idx = logical / PAGESIZE + INSTRUCTION_LOGICAL_SIZE / PAGESIZE;
        int offset = logical % PAGESIZE;
        unsigned char value = 0;

        /* Unmapped trailing logical pages are treated as zero-filled. */
        if (idx >= 0 && idx < NUM_LOGICAL_PAGES &&
            pageTable[proc_id][idx] != 0) {
            int physical = (int)pageTable[proc_id][idx] * PAGESIZE + offset;
            value = memory[physical];
        }

        fprintf(fp, "%02X%s", value, (logical % 4 == 3) ? "\n" : " ");
    }
    fclose(fp);

    releaseProcessPages(proc_id);
    return 0;
}

int32_t read_memory_32(int proc_id, int address)
{
    uint32_t v = 0;
    int p0, p1, p2, p3;

    p0 = getPhysicallAddress(proc_id, 0, address);
    p1 = getPhysicallAddress(proc_id, 0, address + 1);
    p2 = getPhysicallAddress(proc_id, 0, address + 2);
    p3 = getPhysicallAddress(proc_id, 0, address + 3);
    if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0) {
        fprintf(stderr, "Memory read error: proc=%d address=%d\n", proc_id, address);
        return 0;
    }

    v = (uint32_t)memory[p0]
      | ((uint32_t)memory[p1] << 8)
      | ((uint32_t)memory[p2] << 16)
      | ((uint32_t)memory[p3] << 24);
    return (int32_t)v;
}

void write_memory_32(int proc_id, int address, int32_t value)
{
    uint32_t v = (uint32_t)value;
    int p0, p1, p2, p3;

    p0 = getPhysicallAddress(proc_id, 0, address);
    p1 = getPhysicallAddress(proc_id, 0, address + 1);
    p2 = getPhysicallAddress(proc_id, 0, address + 2);
    p3 = getPhysicallAddress(proc_id, 0, address + 3);
    if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0) {
        fprintf(stderr, "Memory write error: proc=%d address=%d\n", proc_id, address);
        return;
    }

    memory[p0] = (unsigned char)(v & 0xff);
    memory[p1] = (unsigned char)((v >> 8) & 0xff);
    memory[p2] = (unsigned char)((v >> 16) & 0xff);
    memory[p3] = (unsigned char)((v >> 24) & 0xff);
}
