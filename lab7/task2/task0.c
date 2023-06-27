
#include <stdlib.h>
#include <linux/limits.h>




#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

int main(int argc,char** argv){
    int status;
    int i;
    int first_count = 2;
    int second_count = 4;
    char** first_action = (char**) malloc(first_count*sizeof(char*));
    char** second_action = (char**) malloc(second_count*sizeof(char*));
    first_action[0] = "ls";
    first_action[1] = "-l";
    second_action[0] = "tail";
    second_action[1] = "-n";
    second_action[2] = "2";
    /*for(i=0;i<first_count;i++){
        printf("%s\n",first_action[i]);
    }
    for(i=0;i<second_count;i++){
        printf("%s\n",second_action[i]);
    }*/
    int fd [2];
    if(pipe(fd)==-1){
        printf("pip fail");
        exit(1);
    }
    //fprintf(stderr,"prarnt process forking\n");
    int p = fork();
    if(p==0)
    {
        //fprintf(stderr,"parent_process>closing the read end of the pipe…\n");
        close(fd[0]);
        close(1);
        int old = fd[1];
        dup(old);
            close(old);
            //fprintf(stderr,"closing: %d\n",old);
            //printf("%s\n",first_action[0]);
        execvp(first_action[0],first_action);
                    perror("fail1");
            exit(1);
        
    }
    else
    {
        waitpid(p,&status,0);
        //fprintf(stderr,"parent_process>created process with id: %d\n",p);
        //printf("arrive");
        close(fd[1]);
        //fprintf(stderr,"parent_process>closing the write end of the pipe…%d\n",fd[1]);
        //fprintf(stderr,"prarnt process> forking\n");
        p = fork();
        if(p==0)
        {
            
            close(0);
            int old = fd[0];
            dup(old);
            close(old);
            //fprintf(stderr,"closing: %d\n",old);
            //printf("has the right out\n");
            execvp(second_action[0], second_action);
            perror("fail2");
            exit(1);
            

        }
        else{
            //fprintf(stderr,"parent_process>created process with id: %d\n",p);
            waitpid(p,&status,0);

        }

    }



    free(first_action);
    free(second_action);

}