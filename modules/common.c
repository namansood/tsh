#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

const int unameLength = 32;

char* getusername(void) {
	char *username;

	struct passwd *user = getpwuid(geteuid());

	if(user) username = user->pw_name;
	else username = "???";

	return username;
}

char* getDirWithHome(char *ogDir) {
	char *homePath = (char *) malloc((unameLength + 6) * sizeof(char));

	strncpy(homePath, "/home/", 7);

	strncat(homePath, getusername(), unameLength);

	int pathLength = strlen(&(ogDir[1])),
		homeLength = strlen(homePath);

	char *parsedDir = (char *) malloc((homeLength + pathLength) * sizeof(char));

	strncpy(parsedDir, homePath, homeLength);

	strncat(parsedDir, &(ogDir[1]), pathLength);

	free(homePath);

	return parsedDir;
}

char **copyArgv(int argc, char **argv) {
	char **ret = malloc(sizeof(char *) * (argc + 1));
	for(int i = 0; i < argc; ++i) {
		int len = strlen(argv[i]);
		ret[i] = malloc(sizeof(char) * len);
		strncpy(ret[i], argv[i], len);
	}
	ret[argc] = NULL;
	return ret;
}

void freeArgvCopy(int argc, char **argv) {
	for(int i = 0; i < argc; ++i) {
		if(argv[i]) free(argv[i]);
	}
	free(argv);
}