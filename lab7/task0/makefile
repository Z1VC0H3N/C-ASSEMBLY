 #format is target-name: target dependencies 

 

#{-tab-}actions 

 # All Targets 

all: main 

 # Tool invocations 

# Executable "hello" depends on the files hello.o and run.o. 

main: main.o  
	 gcc -g -m32 -Wall -o main main.o  

 # Depends on the source and header files 

main.o: task0.c
	gcc -m32 -g -Wall -c -o main.o task0.c 




 #tell make that "clean" is not a file name! 

.PHONY: clean 

 #Clean the build directory 

clean: 
	rm -f *.o main
