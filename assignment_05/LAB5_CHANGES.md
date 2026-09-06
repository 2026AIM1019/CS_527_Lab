# Lab 5 Changes

## Retained from Lab 4

- Four processor instances (`NP = 4`).
- Per-processor integer/vector registers, PC and flags.
- PID-to-processor mapping.
- Ready/running/waiting task states.
- 10-instruction time slices.
- Non-blocking shell.
- `Print xN` and `process.log`.
- Array Addition and FIR programs.
- Existing scalar regression tests.

## Replaced in Lab 5

The Lab 4 per-processor memory arrays:

```c
Instruction[NP][256]
Data[NP][4096]
```

are removed.

They are replaced by one shared physical memory:

```c
#define MEMSIZE 8192
#define PAGESIZE 512
unsigned char memory[MEMSIZE];
```

The OS/MMU maintains:

```c
unsigned char pageTable[NP][NUM_LOGICAL_PAGES];
unsigned char freePages[NUM_PHYSICAL_PAGES];
```

Frame 0 is permanently reserved.

## Address translation

Instruction fetch:

```text
page = logical_address / PAGESIZE
```

Data access:

```text
page = logical_address / PAGESIZE + 1024 / PAGESIZE
```

Physical address:

```text
frame * PAGESIZE + logical_address % PAGESIZE
```

The processor performs this translation before every instruction-byte fetch and before every data-memory read/write.

## Loading and finalising

`initialize()` allocates physical frames for the pages actually present in `program.byte` and `data.byte`, updates the process page table, and copies the bytes into physical memory.

`finalize()` reconstructs the task's logical data space from its page table, writes `data.byte`, then releases the task's frames.

If physical frames are temporarily unavailable, the Lab 4 OS keeps the task waiting and retries it after another process finishes.

## New verification

`tests/lab5_mmu` accesses addresses 508 and 512, crossing the 512-byte logical data-page boundary. This verifies that the processor does not assume logical pages are physically contiguous.
