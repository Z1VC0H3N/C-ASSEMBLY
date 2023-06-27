#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>
#include "LineParser.h"
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
	//process** prPtr = (process**)malloc(sizeof (process*));
	process* newProc = (process*)malloc(sizeof (process));
	newProc->next = *process_list;
	newProc->cmd = cmd;
	newProc->pid = pid;
	newProc->status = RUNNING;
	*process_list = newProc;
}
void deleteAllTerminated(process** process_list){
	process* p = *process_list;
	process* before=NULL;
	process* next=NULL;
	while (p != NULL){
		next = p->next;
		if (p->status == TERMINATED){
			if(before==NULL){
				*process_list = p->next;
			}
			else{
				before->next=p->next;
				before=p;
			}
			p->next = NULL;
			freeProcessList(p);
		}
		p = next; 
	}
}

void printProcessList(process** process_list){
	process* p = *process_list;
	char* status = "Error";
	updateProcessList(process_list);
	printf("PID\tCommand\tSTATUS\n");
	while (p != NULL){
		status = "Error";
		if (p->status == RUNNING) status = "Running";
		if (p->status == SUSPENDED) status = "SUSPENDED";
		if (p->status == TERMINATED) status = "Terminated";
		printf("%d\t%s\t%s\n", p->pid, p->cmd->arguments[0], status);
		p = p->next;
	}
	deleteAllTerminated(process_list);
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

int execute(cmdLine *cmd, int isDebag, char* buf, process** process_list){
	int pid = 0;
	int status;
	if (strncmp(cmd->arguments[0], "cd", 2) == 0){
		chdir(cmd->arguments[1]);
		perror("chdir fail");
		freeCmdLines(cmd);
	}
	else if (strncmp(cmd->arguments[0], "procs", 5) == 0){
		printProcessList(process_list);
		freeCmdLines(cmd);
		//perror("procs fail");
	}
	else if (strncmp(cmd->arguments[0], "kill", 4) == 0){
		kill(atoi(cmd->arguments[1]),SIGINT);
		freeCmdLines(cmd);
		//perror("procs fail");
	}
	else if (strncmp(cmd->arguments[0], "suspend", 7) == 0){
		kill(atoi(cmd->arguments[1]),SIGTSTP);
		freeCmdLines(cmd);
		//perror("procs fail");
	}
	else if (strncmp(cmd->arguments[0], "wake", 4) == 0){
		kill(atoi(cmd->arguments[1]),SIGCONT);
		freeCmdLines(cmd);
		//perror("procs fail");
	}
	else{
		status = RUNNING;
		pid=fork();
		if (!pid){
			if (strncmp(cmd->arguments[0], "cd", 2) != 0){
				execvp(cmd->arguments[0] ,cmd->arguments);
				perror("execvp fail");
			}
			exit(1);
		}
		if (isDebag == 1){
		fprintf(stderr, "Pid: %d\n", pid);
		fprintf(stderr, "command: %s\n", buf);
		}
	
		if (cmd->blocking != 0)
			waitpid(pid,&status ,0);
		addProcess(process_list, cmd, pid);
		
	}
	
	return pid;
}
void freeProcessList(process* process_list){
	if(process_list->next!=NULL){
		freeProcessList(process_list->next);
	}
	freeCmdLines(process_list->cmd);
	free(process_list);
}
void updateProcessList(process **process_list){
	process* tmp = *process_list;
	int returnd_pid;
	int status;
	while(tmp!=NULL){
		returnd_pid = waitpid(tmp->pid,&status,WNOHANG | WUNTRACED| WCONTINUED);
		//printf("pid:%d\n",returnd_pid);
		if(returnd_pid==-1){
			updateProcessStatus(tmp,tmp->pid,TERMINATED);
		}
		else if(returnd_pid!=0){
			//printf("%d\n",status);
			if(WIFSTOPPED(status)){
				updateProcessStatus(tmp,tmp->pid,SUSPENDED);
			}
			if(WIFEXITED(status) || WIFSIGNALED(status)){
				updateProcessStatus(tmp,tmp->pid,TERMINATED);
			}
			if(WIFCONTINUED(status)){
				updateProcessStatus(tmp,tmp->pid,RUNNING);

			}
		}
		tmp=tmp->next;
	}
}
void updateProcessStatus(process* process_list, int pid, int status){
	if(process_list==NULL){
		return;
	}
	if(process_list->pid!=pid){
		updateProcessStatus(process_list->next,pid,status);
	}
	process_list->status = status;
}

int main(int argc, char **argv)
{
	int isDebag = 0;
	int i = 0;
	char path [PATH_MAX];
	char* buf = malloc(2048);
	struct cmdLine* cmd;
	process** process_list = (process**)malloc(sizeof(process*));
	*process_list = NULL;
	//int pid;
	 for(i=1; i<argc; i++){
        if(strncmp(argv[i],"-d", 2)==0){
            isDebag = 1;
            fprintf(stderr,"%s\n","-D");
        }
	 }
	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);
	while (1)
	{
		getcwd(path, PATH_MAX);
		printf("Path directory: %s\n", path);
		fgets(buf, 2048, stdin);
		if (feof (stdin))
			exit(0);
		if (strncmp(buf, "quit", 4) == 0){
			printf("Quitting...\n");
			free(buf);
			freeCmdLines(*process_list);
			free(process_list);
			exit(0);
		}
		cmd = parseCmdLines(buf);
		execute(cmd, isDebag, buf, process_list);
	}
	
	return 0;
}