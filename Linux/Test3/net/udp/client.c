#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in server_address,preply_address;	
	socklen_t server_len,preply_len;
	
	char msg_client[100] = "This message is from client";
	char msg_rec[100];
	int n;
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	server_len = sizeof(server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("192.168.0.1");
	server_address.sin_port = htons(2012);
	
	sendto(sockfd,msg_client,strlen(msg_client),0,(struct sockaddr *)&server_address,server_len);

	n = recvfrom(sockfd,msg_rec,sizeof(msg_rec),0,(struct sockaddr *)&preply_address,&preply_len);
	msg_rec[n] = '\0';		
	printf("%s\n",msg_rec);
									
	return 0;
}
