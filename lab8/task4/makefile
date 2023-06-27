all: counter

counter: counter.o
	gcc -g -m64 -fno-pie -Wall -o counter counter.o

counter.o: counter.c
	gcc -g -m64 -fno-pie -Wall -c counter.c -o counter.o

.PHONY: clean

clean:
	rm -f *.o counter
