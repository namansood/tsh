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

	int ret;

	if(dir[0] == '~') {
		char *parsedDir = getDirWithHome(dir);

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