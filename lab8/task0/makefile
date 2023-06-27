all: hexeditplus

hexeditplus: hexeditplus.o
	gcc -g -m32 -Wall -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	gcc -g -m32 -Wall -c hexeditplus.c -o hexeditplus.o

.PHONY: clean

clean:
	rm -f *.o hexeditplus
