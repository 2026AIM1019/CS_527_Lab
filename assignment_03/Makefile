CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = main.o compiler.o memory.o processor.o

executable: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o executable

main.o: main.c compiler.h memory.h processor.h
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

clean:
	rm -f *.o executable program.byte


test: executable
	python3 test_runner.py
