#include <iostream>
#include <unistd.h>

int main(void) {
	pid_t pid;
	int status;
	const char *cmd = "./an_ch2_1b";
	if ((pid = fork()) < 0) {
		printf("fork error.");
	}
	else if (pid == 0) {	// child
		//execlp(cmd, cmd, (char *)0);
		system(cmd);
	}

	if ((pid = waitpid(pid, &status, 0)) < 0) {
		printf("waitpid error\n");
	}

	return 0;
}
