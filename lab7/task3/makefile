 #format is target-name: target dependencies 

 

#{-tab-}actions 

 # All Targets 

all: main 

 # Tool invocations 

# Executable "hello" depends on the files hello.o and run.o. 

main: main.o LineParser.o 
	 gcc -g -m32 -Wall -o main main.o LineParser.o 

 # Depends on the source and header files 

main.o: task2.c
	gcc -m32 -g -Wall -c -o main.o task2.c 

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 



 #tell make that "clean" is not a file name! 

.PHONY: clean 

 #Clean the build directory 

clean: 
	rm -f *.o main
