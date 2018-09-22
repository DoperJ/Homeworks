#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	pid_t parent, child; parent = getpid(); child = fork(); if(child < 0)
	{
		printf("fork error \n"); return(1);
	}
	else if(child == 0) {
		printf("child run : child_pid = %d \n",getpid()); usleep(2000000);
		printf("Kill parent process\n"); kill(parent,SIGKILL);
		usleep(2000000); 
	}
	else if(child > 0) {
		printf("parent run : parent_pid = %d \n",parent);
		while(1); 
	}
}
