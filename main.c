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

char* input(void);
void prompt(void);

int run(char**);
char** split(char*);

void returnstatus(int);

int main(int argc, char **argv) {
	if(argc == 1) {
		int keepRunning = 1;

		while(keepRunning) {
			char *cmd = input();

			if(strncmp(cmd, "exit", 4) == 0) {
				puts("bye lmao");
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
allocate memory for 32 chars
input chars one by one
if we have put in 32 chars, reallocate space for 32 more
so on for 64, 96, so on
return pointer to first char

*/

#define bufsize 32

char* input(void) {
	prompt();

	char *str = (char*) malloc(bufsize * sizeof(char));

	int bytes = 0, multiplier = 1;

	char byte = '\0';

	while(byte != '\n') {
		byte = getchar();
		if(byte == '\n') break;
		str[bytes++] = byte;

		// plus one to accomodate null character
		if(bytes + 1 >= multiplier * bufsize) {
			multiplier++;
			str = realloc(str, multiplier * bufsize * sizeof(char));
		}
	}

	str[bytes] = '\0';

	return str;
}

/*

how this function works:
geteuid() returns the current user's uid
getpwuid() returns user details of a uid from /etc/passwd
getcwd() puts current working directory in cwd string

*/

void prompt(void) {
	char cwd[PATH_MAX] = "???";

	char *username = getusername();

	getcwd(cwd, sizeof(cwd));

	printf("%s at %s:\n> ", username, cwd);
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
		for(int i  = 0; input[i]; i++) {
			printf("argument: %s\n", input[i]);
		}
		// note: use execve for environment variables support
		status = execvp(input[0], input);
		exit(status);
	}

	else {
		wait(&status);
	}

	return status;
}

/*

how this function works:
Input string: arg1 arg2 arg3 arg4 arg5
Every space is replaced by null character to make strings
And ret is a char pointer array with pointers to each of these strings
ret is allocated variably similarly to the string of input()

quotes support:
doingQuotes stores null char (falsey value) if a quote isn't open,
quote char if quote is open
while a quote is open, suspend splitting cause it's all one argument
close quote when same quote char re-encountered

*/

#define argcount 4

char** split(char* input) {
	char **ret = (char **) malloc(argcount * sizeof(char *));

	int strings = 0, multiplier = 1, i = 0;

	char byte, *starter = &input[i], doingQuotes = '\0';

	do {
		byte = input[i];
		
		if((byte == ' ' || byte == '\n') && (doingQuotes == '\0')) {
			ret[strings++] = starter;
			input[i] = '\0';
			starter = &input[i+1];
		}

		else if(byte == '\"' || byte == '\'') {
			if(!doingQuotes) {
				doingQuotes = byte;
				// if this was starting a string, start at next char
				if(starter == &byte) starter++;
				// move everything in memory one character back (plus one for null)
				memmove(&input[i], &input[i+1], (strlen(&input[i+1]) + 1) * sizeof(char));
				// don't need to do i++ because you're already at the next char now
				continue;
			}
			else if(byte == doingQuotes) {
				doingQuotes = '\0';
				// move everything in memory one character back (plus one for null)
				memmove(&input[i], &input[i+1], (strlen(&input[i+1]) + 1) * sizeof(char));
				// don't need to do i++ because you're already at the next char now
				continue;
			}
		}

		// plus one for terminator string
		if(strings + 1 >= multiplier * argcount) {
			multiplier++;
			ret = realloc(ret, multiplier * argcount * sizeof(char *));
		}

		i++;
	} while(byte != '\0');

	ret[strings++] = starter;

	// last pointer null pointer
	ret[strings] = NULL;

	return ret;
}

void returnstatus(int status) {
	if(status == 0) return;
	else if(status == 65280 || status == 255) {
		printf("File not found.\n");
	}
	else {
		printf("Program exited with code %d.\n", status);
	}
}