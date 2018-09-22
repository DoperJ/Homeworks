#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
/*定义消息数据结构*/ 
struct msgbuf{
	long type;
	char data[255]; 
};

int main() {
	key_t key;
	int msgid;
	struct msgbuf message; /*获取文件键值*/
	key = ftok("msg",'a'); /*创建消息队列*/
	if( (msgid = msgget(key,0666)) == -1) {
		fprintf(stdout,"Open error\n");
		exit(1); 
	}
	memset(&message,0,sizeof(message)); /*从消息队列中获取消息*/ if(msgrcv(msgid,&message,sizeof(message.data),2,0)== -1) {
		fprintf(stdout,"Read error\n");
		exit(1); 
	}
	fprintf(stdout,"read from message : %s\n",message.data);
	exit(0); 
}
