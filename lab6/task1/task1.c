#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>
#include "LineParser.h"

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

int execute(cmdLine *cmd, int isDebag, char* buf)
{
	int pid;
	int status;
	if (strncmp(cmd->arguments[0], "cd", 2) == 0){
		chdir(cmd->arguments[1]);
		perror("chdir");
	}
	else{
		pid=fork();
		if (!pid){
			execvp(cmd->arguments[0] ,cmd->arguments);
			perror("execvp fail");
			exit(1);
		}
		if (isDebag == 1){
			fprintf(stderr, "Pid: %d\n", pid);
			fprintf(stderr, "command: %s\n", buf);
		}
		if (cmd->blocking != 0)
			waitpid(pid,&status ,0);
	}
	return pid;
}

int main(int argc, char **argv)
{
	int isDebag = 0;
	int i = 0;
	char path [PATH_MAX];
	char* buf = malloc(2048);
	struct cmdLine* cmd;
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
			exit(0);
		}
		cmd = parseCmdLines(buf);
		execute(cmd, isDebag, buf);
	}
	free(buf);
	return 0;
}