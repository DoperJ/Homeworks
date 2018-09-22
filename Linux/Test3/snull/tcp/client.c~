#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>



int main(int argc,char **argv)
{
	int client_sockfd;
	socklen_t len;
	struct sockaddr_in sock_address;
	int ret;
	char *msg_send = "hello server";
	char msg_rec[100];
	int n;
	
	client_sockfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&sock_address, sizeof(sock_address));
	sock_address.sin_family=AF_INET;
	sock_address.sin_addr.s_addr=inet_addr("192.168.0.1");
	sock_address.sin_port=htons(8012);
	
	ret=connect(client_sockfd,(struct sockaddr *)&sock_address,sizeof(sock_address));
	if(ret<0)
	{	
			printf("connect error\n");
			return -1;
	}

	write(client_sockfd,msg_send,strlen(msg_send));

	n = read(client_sockfd,msg_rec,100);
	msg_rec[n] = '\0';
	printf("message form server: %s\n",msg_rec);
close(client_sockfd);

	return 0;	

}
