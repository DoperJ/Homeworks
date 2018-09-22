#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <errno.h>
#define MAXLEN 80

using std::cout;
using std::cin;
using std::endl;
using std::string;

char cwd[MAXLEN];//当前工作目录

char *prompt;

char* parseCmd(char* cmd, char** argarr, int* argc);

int main(int argc, char** argv) {
    char cmd[80];
    char* source = NULL;
    //char* my_argv[20];
    int status;
    int my_argc;
    //读取命令及参数
    while(true) {
        getcwd(cwd,MAXLEN);
        cout << cwd << "$ ";
        cout.flush();
        char buf[1024];
        auto buf_len = static_cast<int>(read(0, buf, 1024));
        if(buf_len > 0)//有读取到字符
        {
            int i = 0;
            for( i = 0; i < buf_len; ++i)
            {
                if(buf[i] == '\b' && i >= 1)
                {
                    int j = 0;
                    for( j = i+1; j < buf_len; ++j)
                    {
                        buf[j-2] = buf[j];
                    }
                    buf[buf_len-2] = 0;
                    i--;
                }
                else if(buf[i] == '\b' && i == 0)
                {
                    int j = 0;
                    for( j = 1; j < buf_len; ++j)
                    {
                        buf[j-1] = buf[j];
                    }
                    buf[buf_len-1] = 0;
                }
                else
                {
                    continue;
                }
            }
            buf[buf_len] = 0;
        }
        else
        {
            continue;
        }
        //将读取到的字符串分成多个字符串
        char* start = buf;
        int i =1;
        char* my_argv[10] = {0};
        my_argv[0] = start;
        while(*start)
        {
            if(isspace(*start))
            {
                *start = 0;
                start++;
                my_argv[i++] = start;
            }
            else
            {
                ++start;
            }
        }
        my_argv[i-1] = NULL;
        my_argc = i - 1;

        if(my_argc == 0) continue;
        
        //是否退出程序
        if(strcasecmp(my_argv[0], "exit") == 0) {
            exit(0);//不管exit后面是否还有参数，直接退出
        }
        
        //切换父进程的工作目录
        if(!strcmp(my_argv[0], "cd")) {
            chdir(my_argv[1]);
            continue;
        }

        if(!strcmp(my_argv[my_argc-1], "&")) { //后台命令
            char* a[20];//临时变量
            int i;
            for(i=0; i<my_argc-1; i++) {
                a[i] = static_cast<char *>(malloc(strlen(my_argv[i]) + 1));
                strcpy(a[i],my_argv[i]);
            }
            a[i] = NULL;
            if(fork() == 0) {
                int ret = (execvp(a[0], a) == -1);
                if(ret == -1) { // 执行失败
                    cout << "Failure occurs when " << a[0] << " is executed. "
                         << strerror(errno) << endl;
                }
                exit(1);//退出子进程
            }
            for(i=0; i<my_argc-1; i++) { //释放内存
                free(a[i]);
            }
            continue;
        }

        //创建子进程执行命令
        if(fork() == 0) {
            int ret = (execvp(my_argv[0], my_argv) == -1);
            if(ret == -1) { // 执行失败
                cout << "Failure occurs when " << my_argv[0] << " is executed. "
                     << strerror(errno) << endl;
            }
            exit(1);//退出子进程
        }
        wait(&status);
    }
}