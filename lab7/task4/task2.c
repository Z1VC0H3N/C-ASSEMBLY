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
#define HISLEN 3

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;
typedef struct history{
    char* command;
    int size;
    struct history *next;
}history;

void freeProcessList(process* process_list){
    freeCmdLines(process_list->cmd);
    free(process_list);
    process_list = NULL;
}
void freeComand(history* his_list){
    free(his_list);
}
void printCommand(history** his_list){
    history* temp = *his_list;
    int counter = 1;
    while(temp != NULL){
        printf("%d. %s\n",counter,temp->command);
        counter++;
        temp = temp->next;
    }
}
history* getCommand(history** his_list,int pos){
    int counter =1;
    history* temp = *his_list;
    while(counter != pos){
        if(temp == NULL){
            printf("not valid command\n");
            return NULL;
        }
        temp = temp->next;
        counter ++;
    }
    return temp;
}
history* getLast(history** his_list){
    history* temp  = *his_list;
    if(temp == NULL){
        return NULL;
    }
    else{
        while(temp->next!= NULL){
            temp = temp ->next;
        }
        return temp;
    }
}
void addCommand(history** his_list, const char command []){
    history* h = (history*) malloc(sizeof(history));
    char* clone = (char*)malloc(strlen(command) + 1);
    strcpy(clone,command);
    h->command = clone;
    h->size = 1;
    h->next = NULL;
    history* temp = *his_list;
    if(temp == NULL){
        *his_list = h;
    }
    else{
        while(temp->next != NULL){
            temp->size++;
            temp = temp->next;
        }
        temp->size++;
        temp->next = h;
    }
    temp = *his_list;
    if(temp->size>HISLEN){
        history* temp1 = temp;
        *his_list = temp->next;
        freeComand(temp1);
    }
    

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



int pipPrograms(cmdLine* line){
    int status;
    cmdLine* first_action = line;
    cmdLine* second_action = line->next;
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
        if(first_action->inputRedirect !=NULL){
                close(0);
                open(first_action->inputRedirect,O_RDONLY);
        }
            //fprintf(stderr,"closing: %d\n",old);
            //printf("%s\n",first_action[0]);
        execvp(first_action->arguments[0],first_action->arguments);
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
            if(second_action->outputRedirect !=NULL){
                close(1);
                open(second_action->outputRedirect,O_WRONLY|O_CREAT,438);//file premission 666 in octal.
            }
            //fprintf(stderr,"closing: %d\n",old);
            printf("has the right out\n");
            execvp(second_action->arguments[0], second_action->arguments);
            perror("fail2");
            exit(1);
            

        }
        else{
            //fprintf(stderr,"parent_process>created process with id: %d\n",p);
            waitpid(p,&status,0);

        }

    }
    return p;
}

int execute(cmdLine* line){
    int status = 0;
    int pid;
    if(line->next!=NULL){
        pid = pipPrograms(line);
    }
    else if(strcmp(line->arguments[0],"cd")==0){
        if(chdir(line->arguments[1])!=0){
            perror("fail");
            exit(1);
        }
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
    history** his_list = (history**) calloc(1,sizeof(history*));
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
        if(strncmp(line,"history",7)==0){
            printCommand(his_list);
            continue;
        }
        if(strncmp(line,"!!",2)==0){
            history* temp = getLast(his_list);
            if(temp == NULL){
                printf("has no history\n");
            }
            else{
                execute(parseCmdLines(temp->command));
            }
            continue;
        }
        if(strncmp("!",line,1) == 0){
            int pos = atoi(line+1);
            history* temp = getCommand(his_list,pos);
            if(temp == NULL){
                printf("has no number in the history\n");
            }
            else{
                execute(parseCmdLines(temp->command));
            }
            continue;

        }

        cmdLine* cmd_line = parseCmdLines(line);
        pid = execute(cmd_line);
        if(debug_flag){
            printf("The PID is: %d\nThe command is: %s\n",pid,line);
        }
        if(pid != -1){
            addProcess(process_list,cmd_line,pid);
            addCommand(his_list,line);
        }
        else{
            freeCmdLines(cmd_line);
        }
	}
    free(process_list);

	return 0;
}