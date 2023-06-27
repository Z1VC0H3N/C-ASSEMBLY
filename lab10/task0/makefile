all: task0

task0: task0.o
	gcc -g -m32 -Wall -o task0 task0.o

task0.o: task0.c
	gcc -g -m32 -Wall -c task0.c -o task0.o

.PHONY: clean

clean:
	rm -f *.o task0
