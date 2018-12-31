#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* input(void);

int run(char**);
char** split(char*);

int main(int argc, char **argv) {
	while(1) {
		char *cmd = input();

		if(strncmp(cmd, "exit", 4) == 0) {
			break;
			puts("bye lmao");
		}

		int result = run(split(cmd));
		printf("%d\n", result);
	}

	return 0;
}

#define bufsize 32

char* input(void) {
	printf("> ");

	char *str = (char*) malloc(bufsize * sizeof(char));

	int bytes = 0, multiplier = 1;

	char byte = '\0';

	while(byte != '\n') {
		byte = getchar();
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

int run(char** input) {
	int i = 0;

	while(strncmp(input[i], "\n", 1) != 0) {
		printf("%s\n", input[i]);
		i++;
	}

	return i;
}

#define argcount 4

char** split(char* input) {
	char **ret = (char **) malloc(argcount * sizeof(char *));

	int strings = 0, multiplier = 1, i = 0;

	char byte, *starter = &input[i];

	do {
		byte = input[i];
		
		if(byte == ' ') {
			ret[strings++] = starter;
			input[i] = '\0';
			starter = &input[i+1];
		}

		// plus one for terminator string
		if(strings + 1 >= multiplier * argcount) {
			multiplier++;
			ret = realloc(ret, multiplier * argcount * sizeof(char *));
		}

		i++;
	} while(byte != '\0');

	ret[strings++] = starter;

	char terminator[] = "\n";

	ret[strings] = terminator;

	return ret;
}