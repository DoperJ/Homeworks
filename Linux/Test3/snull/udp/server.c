#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc,char **argv)
{	
		int server_sockfd;
		char *client_ip;
		struct sockaddr_in server_address, client_address;
		socklen_t server_len,client_len;
		char msg_send[100] = "This message is from server";
		char msg_rec[100];
		int n;

		server_sockfd = socket(AF_INET,SOCK_DGRAM,0);						
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		server_address.sin_port = htons(2012);
		
		server_len = sizeof(server_address);
		client_len = sizeof(client_address);
		bind(server_sockfd,(struct sockaddr *)&server_address,server_len);
		
		while(1)
		{
			printf("server is waiting...\n");
			n = recvfrom(server_sockfd,msg_rec,sizeof(msg_rec),0,(struct sockaddr *)&client_address,&client_len);
			msg_rec[n] = '\0';
			printf("%s\n",msg_rec);
			sendto(server_sockfd,msg_send,sizeof(msg_send),0,(struct sockaddr *)&client_address,client_len);
		}

		return 0;
}
