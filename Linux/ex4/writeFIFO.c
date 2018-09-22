/* 进程 writeFIFO:写有名管道 */ 
#include <stdio.h>
#include <unistd.h>
void main() {
	FILE * out_file;
	int count = 1;
	char buf[80];
	/*创建 FIFO*/ 
	if(mkfifo(“myfifo”,O_CREAT|O_EXCL) == -1)
	{
		printf(“make fifo error\n”); exit(1);
	}
	out_file = fopen("mypipe", "w");
	if (out_file == NULL)
	{
		printf("Error opening pipe.");
		exit(1);
	}
	sprintf(buf,"this is test data for the named pipe example\n");
	fwrite(buf, 1, 80, out_file);
	fclose(out_file);
}
