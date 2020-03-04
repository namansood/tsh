#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "../includes/common.h"

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
Input string: arg1 arg2 arg3 arg4 arg5
Duplicated into output string, output string modified
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

	char *output = (char *) malloc((strlen(input) + 1) * sizeof(char));

	strcpy(output, input);

	int strings = 0, multiplier = 1, i = 0;

	char byte, *starter = &output[i], doingQuotes = '\0';

	do {
		byte = output[i];
		
		if((byte == ' ' || byte == '\n') && (doingQuotes == '\0')) {
			ret[strings++] = starter;
			output[i] = '\0';
			starter = &output[i+1];
		}

		else if(byte == '\"' || byte == '\'') {
			if(!doingQuotes) {
				doingQuotes = byte;
				// move everything in memory one character back (plus one for null)
				memmove(&output[i], &output[i+1], (strlen(&output[i+1]) + 1) * sizeof(char));
				// don't need to do i++ because you're already at the next char now
				continue;
			}
			else if(byte == doingQuotes) {
				doingQuotes = '\0';
				// move everything in memory one character back (plus one for null)
				memmove(&output[i], &output[i+1], (strlen(&output[i+1]) + 1) * sizeof(char));
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
