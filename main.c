#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "includes/common.h"
#include "includes/commands.h"
#include "includes/input.h"

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
				char **splitCmd = split(cmd);
				int result = run(splitCmd);
				free(splitCmd);
				returnstatus(result);
			}

			free(cmd);
		}
	}

	else {
		char **args = copyArgv(argc, argv);
		int result = run(args + 1);
		freeArgvCopy(argc, args);
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
	char *outFilename = NULL, *inFilename = NULL;

	for(int i = 0; input[i] != NULL; ++i) {
		if(strncmp(input[i], "<", 1) == 0) {
			if(input[i+1] != NULL) {
				if(input[i+1][0] == '~') {
					inFilename = getDirWithHome(input[i+1]);
				}
				else {
					inFilename = malloc(strlen(input[i+1]) * sizeof(char));
					strncpy(inFilename, input[i+1], strlen(input[i+1]));
				}
				free(input[i]);
				input[i] = NULL;
				if(input[i+2] != NULL && strncmp(input[i+2], ">", 1) != 0) {
					printf("Warning: ingoring arguments after infile %s.\n", input[i+1]);
					break;
				}
				// skip infile
				++i;
			}
			else {
				puts("No file specified after '<', exiting.");
				return -1;
			}
		}

		else if(strncmp(input[i], ">", 1) == 0) {
			if(input[i+1] != NULL) {
				if(input[i+1][0] == '~') {
					outFilename = getDirWithHome(input[i+1]);
				}
				else {
					outFilename = malloc(strlen(input[i+1]) * sizeof(char));
					strncpy(outFilename, input[i+1], strlen(input[i+1]));
				}
				free(input[i]);
				input[i] = NULL;
				if(input[i+2]) {
					printf("Warning: ingoring arguments after outfile %s.\n", input[i+1]);
				}
				break;
			}
			else {
				puts("No file specified after '>', exiting.");
				return -1;
			}
		}
	}

	int pid = fork(), status = -1;

	if(pid == 0) {
		if(outFilename) {
			// open file as write-only, blank out file if it exists,
			// create with permissions RW-R-R- if it doesn't exist,
			// and get a "file descriptor" back that can be used
			// to point stdout to
			int outFile = open(outFilename, O_WRONLY | O_TRUNC | O_CREAT, 
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			// map the opened file to stdout
			dup2(outFile, STDOUT_FILENO);

			// no longer needed to have the file descriptor open,
			// dup is sufficient
			close(outFile);
		}

		if(inFilename) {
			// same as above, this time open() doesn't
			// require permissions since the file (hopefully)
			// already exists
			int inFile = open(inFilename, O_RDONLY);
			dup2(inFile, STDIN_FILENO);
			close(inFile);
		}

		// note: use execve for environment variables support
		status = execvp(input[0], input);
		exit(status);
	}

	else {
		wait(&status);
	}

	if(inFilename) free(inFilename);
	if(outFilename) free(outFilename);

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