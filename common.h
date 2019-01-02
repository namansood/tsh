char* getusername() {
	char *username;

	struct passwd *user = getpwuid(geteuid());

	if(user) username = user->pw_name;
	else username = "???";

	return username;
}