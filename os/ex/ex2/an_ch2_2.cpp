#include <cstdio>
#include <unistd.h>
#include <pthread.h>

int shared_var = 0;

void *thread_son(void *ptr) {
	while(true) {
		shared_var--;
		printf("shared_var in the son thread: %d\n", shared_var);
		sleep(1);
	}
}

int main(void) {
	pthread_t pid;
	int status;
	int ret;
	ret = pthread_create(&pid, NULL, thread_son, NULL);
	if (ret != 0) {
		printf("create pthread error");
		return -1;
	}
	while (true) {
		shared_var++;
		printf("shared_var in the parent thread: %d\n", shared_var);
		sleep(1);
	}
	pthread_join(pid, NULL);
	return 0;
}
