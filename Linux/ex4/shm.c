#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
int main(int argc ,char *argv[]) {
	int shm;
	pid_t pid;
	char *writebuf,*readbuf; if(argc != 2)
	{
		fprintf(stdout,"Usage: ./shm [test-string]\n");
		exit(1);
	}

	/*创建共享内存*/
	if( (shm = shmget(IPC_PRIVATE,100,IPC_CREAT|IPC_EXCL|0666)) == -1) {
		fprintf(stderr,"Create a share memory failed \n");
		exit(1);
	}
	pid = fork();
	if(pid < 0)
	{
		printf("fork error \n");
		return(1);
	}
	else if( pid == 0) {
		/*子进程连接共享内存并更新数据*/ fprintf(stdout,"child process run \n");
		if( (writebuf = shmat(shm,0,0)) == (char *)-1) {
			fprintf(stdout,"child process shmat error \n");
			return 0;
		}
		memset(writebuf,0,100);
		strncpy(writebuf,argv[1],100);
		fprintf(stderr,"Child process write : %s\n",writebuf);
		sleep(2);
		return(0);
	}
	else if(pid > 0) {

		/*父进程等待一段时间后获取更新后的共享内存数据*/ fprintf(stdout,"parent process run \n");
		sleep(2);
		if( (readbuf = shmat(shm,0,0)) == (char *)-1)
		{
			fprintf(stdout,"parent process shmat error \n");
			return 0;
		}
		fprintf(stdout,"Parent process read %s\n",readbuf);
		shmdt(&shm);
		wait(NULL);
		return(0);
	} 
}
