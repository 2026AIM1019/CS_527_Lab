CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
OBJ = main.o compiler.o memory.o processor.o os.o

executable: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o executable

main.o: main.c os.h
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

os.o: os.c os.h compiler.h memory.h processor.h
	$(CC) $(CFLAGS) -c os.c

clean:
	rm -f *.o executable process.log

test: executable
	python3 test_runner.py


demo: executable
	./run_multiprocess_demo.sh
