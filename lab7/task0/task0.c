
#include <stdlib.h>
#include <linux/limits.h>




#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

int main(int argc,char** argv){
    int status = 0;
    int fd [2];
    if(pipe(fd)==-1){
        printf("pip fail");
        exit(1);
    }
    int p = fork(); 
    if(p == 0){ // child process
        close(fd[0]);
        printf("writing to file\n");
        write(fd[1],"hello",6);
        close(fd[1]);
    }
    else{
        close(fd[1]);
        waitpid(p,&status,0);
        char str [100];
        printf("read from file\n");
        read(fd[0],str,100);
        printf("%s\n",str);
        close(fd[0]);
    }
}
