#include <fcntl.h>
#include "LineParser.h"
#include <stdlib.h>
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

void freeProcessList(process* process_list){
    freeCmdLines(process_list->cmd);
    free(process_list);
    process_list = NULL;
}



void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
        
    process* pr = (process*)malloc(sizeof(process));
    pr->cmd = cmd;
    pr->pid = pid;
    if(cmd->blocking != 0){
        pr->status = TERMINATED;
    }
    else{
        pr->status = RUNNING;
    }
    pr->next = NULL;
    process* temp = *process_list;
    if(temp==NULL){
        *process_list = pr;
    }
    else{
        while(temp->next!=NULL){
            temp = temp->next;
        }
        temp->next = pr;
    }
}
void updateProcessStatus(process* process_list, int pid, int status){
    process* temp = process_list;
    while (temp!= NULL){
        if(temp->pid == pid){
            temp->status = status;
            break;
        }
        temp = temp->next;
    }
}
void updateProcessList(process **process_list){
    process* temp = *process_list;
    while(temp!=NULL){
        int status;
        int ret_pid;
        if((ret_pid = waitpid(temp->pid,&status,WNOHANG|WUNTRACED|WCONTINUED))==temp->pid){
            if(WIFSTOPPED(status)){
                updateProcessStatus(*process_list,temp->pid,SUSPENDED);
            }
            else if(WIFEXITED(status)||WIFSIGNALED(status)){
                updateProcessStatus(*process_list,temp->pid,TERMINATED);
            }
            else if(WIFCONTINUED(status))
            {
                updateProcessStatus(*process_list,temp->pid,RUNNING);
            }
        }
        temp = temp->next;
    }
}

void deleteprocess(process** process_list, int pid){
    process* curr =  *process_list;
    process* temp = NULL;
    if(curr->pid == pid){
        *process_list = curr->next;  
    }
    else{
        while(curr->next!=NULL)
        {
            if(curr->next->pid == pid){
                temp = curr->next;
                curr->next = temp->next;
                freeProcessList(temp);
            }
        }
    }

}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("PID\tCommand\tSTATUS\n");
    process* temp = *process_list;
    while(temp!=NULL){
        char* status;
        switch(temp->status){
            case(-1):
            status = "TERMINATED";
            break;
            case(0):
            status = "SUSPENDED";
            break;
            case(1):
            status = "RUNNING";
            break; 
            default:
            status = "TERMINATED";
            break;
        }
        printf("%d\t%s\t%s\n",temp->pid,temp->cmd->arguments[0],status);
        if(strncmp(status,"TERMINATED",10)==0){
            deleteprocess(process_list,temp->pid);
        }
        
        temp = temp->next;
    }

}





void handler(int sig)
{
	printf("\nRecieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP)
	{
		signal(SIGTSTP, SIG_DFL);
	}
	else if (sig == SIGCONT)
	{
		signal(SIGCONT, SIG_DFL);
	}
	signal(sig, SIG_DFL);
	raise(sig);
}

int execute(cmdLine* line){
    int status = 0;
    int pid;
    if(strcmp(line->arguments[0],"cd")==0){
        if(chdir(line->arguments[1])!=0){
            perror("fail");
        }
        return -1;
    }
    else{
        if(!(pid=fork())) {
            if(line->inputRedirect !=NULL){
                close(0);
                open(line->inputRedirect,O_RDONLY);
            }
            if(line->outputRedirect !=NULL){
                close(1);
                open(line->outputRedirect,O_WRONLY|O_CREAT,438);//file premission 666 in octal.
            }
            int i;
            for(i=0;i<line->argCount;i++){
                printf("%s\n",line->arguments[i]);
            }
            execvp(line->arguments[0], line->arguments);
            perror("fail");
            exit(1);
        }

    }
    if(line->blocking!=0){
        waitpid(pid,&status,0);

    }
    return pid;
}




int main(int argc, char **argv)
{
    process** process_list = (process**) calloc(1,sizeof(process*));
    int debug_flag = 0;
    int pid;
    if (argc>1 && strncmp(argv[1],"-d",2)==0){
        debug_flag = 1;
    }
	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);


	while (1)
	{
        char current_directory [PATH_MAX];
        getcwd( current_directory, PATH_MAX );
        printf( "The current directory is: %s\n", current_directory );
        char line [2048];
        fgets(line ,2048,stdin);
        if(strncmp(line,"quit",4)==0){
            break;
            //exit(0);
        }
        if(strncmp(line,"procs",5)==0){
            printProcessList(process_list);
            continue;
        }
        cmdLine* cmd_line = parseCmdLines(line);
        if(strncmp(line,"suspend",7)==0 && cmd_line->argCount>1){
            int suspended = atoi(cmd_line->arguments[1]);
            kill(suspended, SIGSTOP);
            freeCmdLines(cmd_line);
            continue;
        }
        if(strncmp(line,"kill",4)==0 && cmd_line->argCount>1){
            int suspended = atoi(cmd_line->arguments[1]);
            kill(suspended,  SIGKILL);
            freeCmdLines(cmd_line);
            continue;
        }
        if(strncmp(line,"wake",4)==0 && cmd_line->argCount>1){
            int suspended = atoi(cmd_line->arguments[1]);
            kill(suspended,  SIGCONT);
            freeCmdLines(cmd_line);
            continue;
        }
        else {
            pid = execute(cmd_line);
            if(debug_flag){
                printf("The PID is: %d\nThe command is: %s\n",pid,line);
            }
            if(pid != -1){
                addProcess(process_list,cmd_line,pid);
            }
            else{
                freeCmdLines(cmd_line);
            }
        }
	}
    free(process_list);

	return 0;
}