#include <cstdio> 
#include <sys/time.h>
#include <time.h> 
#include <unistd.h>
int main(int argc,char * argv[]) 
{ 
	time_t rawtime;
	struct tm* timeinfo;
	while(true)
	{
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		printf("Those output come from child, %s", asctime(timeinfo));
		sleep(1);
	}
	return 0;
}
