#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

char* getusername(void) {
	char *username;

	struct passwd *user = getpwuid(geteuid());

	if(user) username = user->pw_name;
	else username = "???";

	return username;
}