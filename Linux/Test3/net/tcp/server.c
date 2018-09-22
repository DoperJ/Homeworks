#include <sys/types.h>

#include <sys/socket.h>

#include <stdio.h>

#include <netinet/in.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>



int main(int argc,char **argv)
{	
	char *client;

	char buf[100];
	getcwd(buf,sizeof(buf));
	
	int server_sockfd,client_sockfd;
	char dirt[100];
	socklen_t server_len,client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(8012);
	server_len=sizeof(server_address);
	
	bind(server_sockfd,(struct sockaddr *)&server_address,server_len);
	listen(server_sockfd,10);
	
	while(1)
	{	
		char msg_client[100];
		char *msg_server = "hello client";
		int n=0;

		printf("server is waiting... \n");

		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address,&client_len);

		n = read(client_sockfd,msg_client,100);
		msg_client[n] = '\0';
		switch(msg_client[0])
{		
		case 'A':printf("%s\n",buf);break;		
		case 'B':puts("Linyuetao");break;
		}
		
		printf("receive message form client: %s\n",msg_client);

		write(client_sockfd,msg_server,strlen(msg_server));
		close(client_sockfd);
	}

	return 0;
}
