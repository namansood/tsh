#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>

#include "common.h"
#include "commands.h"
#include "input.h"

int run(char**);
void returnstatus(int);

int main(int argc, char **argv) {
	if(argc == 1) {
		int keepRunning = 1;

		while(keepRunning) {
			char *cmd = input();

			if(strncmp(cmd, "exit", 4) == 0) {
				puts("Exiting...");
				keepRunning = 0;
			}

			else if(strncmp(cmd, "cd", 2) == 0) {
				cd(split(cmd));
			}

			else if(strncmp(cmd, "pwd", 3) == 0) {
				pwd();
			}

			else {
				int result = run(split(cmd));
				returnstatus(result);
			}

			free(cmd);
		}
	}

	else {
		int result = run(&(argv[1]));
		returnstatus(result);
	}

	return 0;
}

/*

how this function works:
fork() creates an exact copy of the current program
returns 0 in child process, pid in parent
so in child we run the command (execvp replaces the child process)
and in the parent we wait for it to end and get the return status

*/

int run(char** input) {
	int pid = fork(), status = -1;

	if(pid == 0) {
		// note: use execve for environment variables support
		status = execvp(input[0], input);
		exit(status);
	}

	else {
		wait(&status);
	}

	// free char * first, then char ** that points to it
	free(input[0]);
	free(input);

	return status;
}

void returnstatus(int status) {
	if(!WIFEXITED(status)) return;

	status = WEXITSTATUS(status);

	if(status == 0) return;
	
	if(status == 255) {
		printf("File not found.\n");
	}
	
	else {
		printf("Program exited with code %d.\n", status);
	}
}