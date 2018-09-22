#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main ( int argc, char *argv[] )
{
    char* arguments[argc];
    struct rusage start, end;//存储测试结果的结构体
    clock_t start_time, end_time;
    pid_t pid;
    for(int i=0; i<argc-1; i++) {
        arguments[i] = argv[i+1];
    }
    arguments[argc-1] = NULL;
    pid=fork();
    start_time = clock();
    if(pid==0){
        int ret = execvp(arguments[0], arguments);
        if (ret==-1) {
            perror("Execvp error!");
        }
    }
    else if(pid>0){
        getrusage(RUSAGE_CHILDREN, &start);
        int status;
        wait(&status);
        end_time = clock();
        getrusage(RUSAGE_CHILDREN, &end);
        double user_time = (end.ru_utime.tv_sec - start.ru_utime.tv_sec) * 1.0e3
                           + (end.ru_utime.tv_usec - start.ru_utime.tv_usec) / 1.0e3;
        double kernel_time = (end.ru_stime.tv_sec - start.ru_stime.tv_sec) * 1.0e3
                           + (end.ru_stime.tv_usec - start.ru_stime.tv_usec) / 1.0e3;
        double execute_time = static_cast<double>(end_time-start_time)/CLOCKS_PER_SEC*1000;
        cout << "User mode took " << user_time << "ms" << endl;
        cout << "Kernel mode took " << kernel_time << "ms" << endl;
        cout << "Arguments were executed for " << execute_time << "ms" << endl;
        cout << "Process was preempted for " << end.ru_nivcsw - start.ru_nivcsw << " times" << endl;
        cout << "Process gave up for " << end.ru_nvcsw - start.ru_nvcsw << " times" << endl;
        cout << "Process encountered page faults for " << end.ru_minflt - start.ru_minflt
                                                          + end.ru_majflt - start.ru_minflt << " times" << endl;
    }
    else{
        perror("Fork error!");
    }
    return EXIT_SUCCESS;
}
