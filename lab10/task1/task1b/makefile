all: task1b

task1b: task1b.o
	gcc -g -m64 -Wall -o task1b task1b.o

task1b.o: task1b.c
	gcc -g -m64 -Wall -c task1b.c -o task1b.o

.PHONY: clean

clean:
	rm -f *.o task1b
