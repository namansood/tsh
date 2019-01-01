void pwd() {
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

	int ret = chdir(input[1]);

	if(ret) {
		if(ret == -1) printf("Directory not found: %s\n", input[1]);
		else printf("Error: %d\n", ret);
	}
}