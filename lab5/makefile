 #format is target-name: target dependencies 

 

#{-tab-}actions 

 # All Targets 

all: main 

 # Tool invocations 

# Executable "hello" depends on the files hello.o and run.o. 

main: main.o
	 gcc -g -m32 -Wall -o main main.o

 # Depends on the source and header files 

main.o: main.s
	nasm -g -f elf -w+all -o main.o main.s  


 #tell make that "clean" is not a file name! 

.PHONY: clean 

 #Clean the build directory 

clean: 
	rm -f *.o main
