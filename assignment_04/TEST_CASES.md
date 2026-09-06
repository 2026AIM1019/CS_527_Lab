# CS527 Lab 4 Test Cases

## Build

```bash
make clean
make
```

## Automatic tests

```bash
make test
```

The suite contains:

1. `test_01` through `test_05` - Assignment 03 scalar regression tests.
2. `vector_array_add` - Lab 3 vector Array Addition.
3. `vector_fir` - Lab 3 vector FIR.
4. `lab4_print` - Lab 4 Print instruction and process log.
5. `lab4_multiprocess` - six long-running processes. The first four occupy processors 0-3; PID 5 and PID 6 enter the waiting queue and are dispatched after processors become free.

## Interactive shell

```bash
./executable
```

Then enter:

```text
$ tests/vector_array_add/program.txt tests/vector_array_add/data.byte
```

or:

```text
$ tests/vector_fir/program.txt tests/vector_fir/data.byte
```

Use a separate line for:

```text
$ exit
```

## Multiprocessing demonstration

For deterministic assignment of all four processors plus a waiting queue:

```bash
printf 'exit\n' | ./executable \
 tests/lab4_multiprocess/program1.txt tests/lab4_multiprocess/data1.byte \
 tests/lab4_multiprocess/program2.txt tests/lab4_multiprocess/data2.byte \
 tests/lab4_multiprocess/program3.txt tests/lab4_multiprocess/data3.byte \
 tests/lab4_multiprocess/program4.txt tests/lab4_multiprocess/data4.byte \
 tests/lab4_multiprocess/program5.txt tests/lab4_multiprocess/data5.byte \
 tests/lab4_multiprocess/program6.txt tests/lab4_multiprocess/data6.byte
```

Expected scheduler behavior:

```text
PID 1 -> processor 0
PID 2 -> processor 1
PID 3 -> processor 2
PID 4 -> processor 3
PID 5 -> WAITING queue
PID 6 -> WAITING queue
...
PID 5 -> processor 0/1/2/3 after a processor finishes
PID 6 -> another free processor
```
