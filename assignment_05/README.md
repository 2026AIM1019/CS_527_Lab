# CS527 Lab 5 - Logical Memory / Physical Memory with MMU

This project starts from the Assignment 03 / Lab 4 mini-computer and keeps the Lab 4 multiprocessing OS, scheduler, vector instructions, Print instruction and shell. Lab 5 replaces the per-processor logical memory arrays with one shared physical memory and per-process page tables.

## Lab 5 memory model

```text
#define MEMSIZE 8192
#define PAGESIZE 512

Physical memory = 8192 bytes = 16 frames
Frame 0 = reserved
Allocatable frames = 1..15

Logical instruction space = 1024 bytes = 2 pages
Logical data space        = 4096 bytes = 8 pages
Total logical pages/task  = 10
```

The MMU maintains:

```c
char pageTable[MAX_PROC][NUM_LOGICAL_PAGES];
char freePages[NUM_PHYSICAL_PAGES];
```

`pageTable[proc_id][page]` stores the physical frame number for that process.

Data references keep the program's logical addresses `0..4095`. For data accesses, the page-table index is:

```text
address / PAGESIZE + 1024 / PAGESIZE
```

Instruction fetches use:

```text
address / PAGESIZE
```

Physical address:

```text
physical_frame * PAGESIZE + address % PAGESIZE
```

## Build

```bash
make clean
make
```

## Automatic verification

```bash
make test
```

The test suite retains the Assignment 03 scalar/vector tests and Lab 4 tests, and adds `lab5_mmu`, which checks logical-to-physical translation across the 512-byte data-page boundary.

## Interactive Lab 4 shell

```bash
./executable
```

Then enter exactly two filenames:

```text
$ tests/vector_array_add/program.txt tests/vector_array_add/data.byte
```

or:

```text
$ tests/vector_fir/program.txt tests/vector_fir/data.byte
```

Type `exit` on its own line when no more tasks should be accepted.

## Multiprocessing demonstration

```bash
make demo
```

This retains the Lab 4 demonstration of four processors, time slices, process transitions, and the waiting queue. The demo uses one 512-byte logical data page per process so six processes can coexist in the 16-frame physical memory: frame 0 is reserved, and each process needs one instruction frame + one data frame.

## Lab 5 MMU behavior

When a processor fetches an instruction, the processor calls:

```c
getPhysicallAddress(proc_id, 1, PC)
```

before reading instruction bytes.

For scalar/vector data memory accesses, the processor calls:

```c
getPhysicallAddress(proc_id, 0, address)
```

before reading or writing physical memory.

`initialize()` now:
1. Finds the required instruction/data pages in the input files.
2. Allocates physical frames using `getFreePage()`.
3. Updates the process page table.
4. Copies the bytes into the allocated physical frames.

`finalize()`:
1. Reconstructs the logical 4096-byte data space through the page table.
2. Writes it back to the task's `data.byte`.
3. Releases all frames used by the task.

If physical frames are temporarily unavailable, the Lab 4 OS leaves the task in the waiting queue and retries it after another task finishes and releases its pages.

## Important

`Instruction[NP][256]` and `Data[NP][4096]` are no longer used as memory storage. The physical storage is:

```c
char memory[MEMSIZE];
```

The page tables perform the logical-to-physical translation.
