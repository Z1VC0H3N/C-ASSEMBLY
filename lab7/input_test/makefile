all: my_echo

my_echo: my_echo.o
	gcc -g -m64 -Wall -o my_echo my_echo.o

my_echo.o: my_echo.c
	gcc -m64 -g -Wall -c -o my_echo.o my_echo.c
	
	
.PHONY: clean 

 #Clean the build directory 

clean: 
	rm -f *.o my_echo
