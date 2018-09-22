#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h>

void myfunc(int signo) {
	if(signo == SIGINT)
		fprintf(stdout,"We catch the SIGINT signal\n");
	else if(signo == SIGALRM)
		fprintf(stdout,"We catch the SIGALRM signal\n");
}

int main() {
	pid_t pid = getpid();
	fprintf(stdout,"pid = %d ,Waiting for signal SIGINT or SIGALRM\n",pid); 
	//设置 SIGINT 信号的处理函数
	signal(SIGINT ,myfunc);
	//设置 SIGALRM 信号的处理函数
	signal(SIGALRM,myfunc);

	//设置闹钟信号 alarm(30); pause();
	exit(0);
}
