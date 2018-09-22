/*进程 msgSend : 创建消息队列并发送消息*/ #include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
/*定义消息数据结构*/ 
struct msgbuf{
	long type;
	char data[255]; 
};

int main(int argc ,char *argv[]) {
	key_t key;
	int msgid;
	struct msgbuf message; if(argc != 2)
	{
		fprintf(stdout,"Usage: ./msgSend [message String]\n");
		return -1; 
	}
	/*获取文件键值*/

	key = ftok("msg",'a');
	fprintf(stdout,"key = %d\n",key); /*创建消息队列*/
	if( (msgid = msgget(key,IPC_CREAT|0666)) == -1) {
		fprintf(stdout,"Create error \n");
		return -1; 
	}
	memset(&message,0,sizeof(message)); /*填充消息数据结构*/
	message.type = 2;
	strcpy(message.data,argv[1]); /*将消息添加到消息队列*/ msgsnd(msgid,&message,sizeof(message.data),0); //pause();
	return 0; 
}
