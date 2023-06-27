all: hexeditplus

hexeditplus: hexeditplus.o
	gcc -g -m64 -Wall -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	gcc -g -m64 -Wall -c hexeditplus.c -o hexeditplus.o

.PHONY: clean

clean:
	rm -f *.o hexeditplus
