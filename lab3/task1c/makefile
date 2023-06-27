all : hexaPrint

hexaPrint: hexaPrint.o
	gcc -g -m32 -Wall -o hexaPrint hexaPrint.o
hexaPrint.o:
	gcc -m32 -g -Wall -c -o hexaPrint.o hexaPrint.c
.PHONY:clean

clean:
	rm -f *.o hexaPrint
