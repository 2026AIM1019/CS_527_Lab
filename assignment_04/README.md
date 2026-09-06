# CS527 Lab 4 - Mini Computer with Multiprocessing

This project starts from Assignment 03 and adds the Lab 4 operating-system layer while retaining the scalar and vector processor functionality.

## Lab 4 additions

- `NP = 4` independent processor instances.
- Per-processor integer registers: `Register[NP][256]`.
- Per-processor vector registers: `VectorRegister[NP][32][8]`.
- Per-processor instruction/data memories.
- Per-processor PC, flags and end-of-simulation state.
- PID-to-processor mapping.
- Ready/running and waiting queues.
- Round-robin scheduling with a 10-instruction time slice.
- 10 microsecond sleep after every processor time slice.
- Non-blocking shell input using `select()`/`read()`.
- Loader for source `.txt` programs and `.byte` programs.
- Waiting queue when all four processors are busy.
- `Print xN` instruction and `process.log` output.
- Assignment 03 scalar/vector tests, Array Add and FIR retained.

## Build

```bash
make clean
make
```

## Automatic verification

Run:

```bash
make test
```

The test runner checks:

- build
- five Assignment 03 scalar regression tests
- vector Array Addition
- vector FIR
- Lab 4 Print/logging
- deterministic four-processor + waiting-queue multiprocessing

## Interactive shell

Start the OS shell:

```bash
./executable
```

You will see:

```text
$ 
```

Enter **two filenames on one line**, separated by a space:

```text
$ tests/vector_array_add/program.txt tests/vector_array_add/data.byte
```

Press Enter and wait for the OS status messages. Then you may submit another task.

For FIR:

```text
$ tests/vector_fir/program.txt tests/vector_fir/data.byte
```

When you do not want to submit any more tasks, type:

```text
exit
```

The shell stops accepting new tasks, but already-loaded/waiting tasks continue until completion.

### Important shell rule

Do not append `exit` directly to a filename. This is wrong:

```text
$ tests/vector_array_add/program.txt tests/vector_array_add/data.byteexit
```

Use a separate line:

```text
$ tests/vector_array_add/program.txt tests/vector_array_add/data.byte
$ exit
```

The shell rejects commands that do not contain exactly two filenames, which prevents accidental malformed data-file paths.

## Deterministic multiprocessing demonstration

The easiest way to visibly demonstrate all four processors and the waiting queue is to start six long-running test tasks as command-line arguments:

```bash
./executable \
 tests/lab4_multiprocess/program1.txt tests/lab4_multiprocess/data1.byte \
 tests/lab4_multiprocess/program2.txt tests/lab4_multiprocess/data2.byte \
 tests/lab4_multiprocess/program3.txt tests/lab4_multiprocess/data3.byte \
 tests/lab4_multiprocess/program4.txt tests/lab4_multiprocess/data4.byte \
 tests/lab4_multiprocess/program5.txt tests/lab4_multiprocess/data5.byte \
 tests/lab4_multiprocess/program6.txt tests/lab4_multiprocess/data6.byte
```

If you want the command to terminate automatically after all tasks finish, pipe `exit` into the shell:

```bash
printf 'exit\n' | ./executable \
 tests/lab4_multiprocess/program1.txt tests/lab4_multiprocess/data1.byte \
 tests/lab4_multiprocess/program2.txt tests/lab4_multiprocess/data2.byte \
 tests/lab4_multiprocess/program3.txt tests/lab4_multiprocess/data3.byte \
 tests/lab4_multiprocess/program4.txt tests/lab4_multiprocess/data4.byte \
 tests/lab4_multiprocess/program5.txt tests/lab4_multiprocess/data5.byte \
 tests/lab4_multiprocess/program6.txt tests/lab4_multiprocess/data6.byte
```

You should see the first four tasks assigned to processors 0-3 and tasks 5 and 6 placed in the WAITING queue. Later, they are dispatched to processors as those processors finish.

## Print/logging

A source instruction:

```text
x1 = 42
Print x1
```

generates:

```text
08 00 00 01
```

and produces a line in `process.log` containing the PID, register number and hexadecimal value.

Check it with:

```bash
cat process.log
```

## Output data

When a process finishes, its processor's data memory is written back to the data file supplied to that process. For Array Add and FIR, use `make test` to verify the mathematical output automatically.

## One-command multiprocessing demo

After `make`, run:

```bash
./run_multiprocess_demo.sh
```

This deterministically loads six long-running tasks before scheduling starts. The expected visible sequence includes:

```text
[OS] Loaded PID 1 on processor 0
[OS] Loaded PID 2 on processor 1
[OS] Loaded PID 3 on processor 2
[OS] Loaded PID 4 on processor 3
[OS] All 4 processors busy; PID 5 placed in WAITING queue
[OS] All 4 processors busy; PID 6 placed in WAITING queue
...
[OS] Dispatched PID 5 from WAITING queue ...
[OS] Dispatched PID 6 from WAITING queue ...
```

The final `process.log` contains one `Print` record for each PID, proving that each task kept its own register state.

## Lab 4 scheduler demonstration

`make test` runs the automated regression suite and intentionally keeps its output concise.

To visibly demonstrate the Lab 4 OS scheduler, run:

```bash
make demo
```

The demo creates six short-running processes. Since `NP = 4`, PIDs 1-4 are assigned to processors 0-3 and PIDs 5-6 enter the waiting queue. With `OS_DEMO=1`, the OS prints scheduler rounds, processor assignments, 10-instruction time slices, READY/WAITING queues, state transitions, dispatches, and process completion. It also prints the resulting `process.log`.
