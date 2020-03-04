#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "../includes/common.h"

void pwd(void) {
	char cwd[PATH_MAX];
	if(getcwd(cwd, sizeof(cwd))) {
		printf("%s\n", cwd);
	}

	else {
		printf("Error: could not print working directory.\n");
	}
}

void cd(char **input) {
	int n;

	for(n = 0; input[n] != NULL; n++);

	if(n < 2) {
		printf("Error: too few arguments\n");
		return;
	}

	else if(n > 2) {
		printf("Warning: ignoring extra arguments\n");
	}

	char *dir = input[1];

	const int unameLength = 32;

	int ret;

	if(dir[0] == '~') {
		char *homePath = (char *) malloc((unameLength + 6) * sizeof(char));

		strncpy(homePath, "/home/", 7);

		strncat(homePath, getusername(), unameLength);

		int pathLength = strlen(&(dir[1])),
			homeLength = strlen(homePath);

		char *parsedDir = (char *) malloc((homeLength + pathLength) * sizeof(char));

		strncpy(parsedDir, homePath, homeLength);

		strncat(parsedDir, &(dir[1]), pathLength);

		free(homePath);

		ret = chdir(parsedDir);

		free(parsedDir);
	}
	
	else {
		ret = chdir(dir);
	}

	if(ret) {
		if(ret == -1) printf("Directory not found: %s\n", input[1]);
		else printf("Error: %d\n", ret);
	}
}