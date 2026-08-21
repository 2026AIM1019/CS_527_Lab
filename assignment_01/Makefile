OBJ = main.o compiler.o memory.o processor.o

executable: $(OBJ)
	gcc $(OBJ) -o executable

main.o: main.c compiler.c memory.c processor.c
	gcc  -c main.c

compiler.o: compiler.c
	gcc -c compiler.c

memory.o: memory.c compiler.c
	gcc -c memory.c

processor.o: processor.c memory.c
	gcc -c processor.c

clean:
	rm -f *.o executable program.byte
