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
	
	int server_sockfd,client_sockfd;
	
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
	
	
	char msg_client[100];
	
	char *msg_server = "hello client:ChenRuipeng&LiJianwei~!";
	
	int n;
	printf("server is waiting... \n");	


	
	client_sockfd = accept(server_sockfd,(struct sockaddr*)&client_address,&client_len);
	
	
	
	while(1)
	
	{	
		
		n = read(client_sockfd,msg_client,100);
		
		msg_client[n] = '\0';  
		
		int execvpe(const char *file, char *const argv[],
char *const envp[]);
			
		
		char cmd;
		
		write(client_sockfd,msg_server,strlen(msg_server));
		
		cmd=msg_client[0];
		
		
		switch(cmd)
		
		{
			
			case 'A':	
						
			system("pwd");	
						
			break;					
					
			
			case 'B':
						
			system("touch crpljw.txt");	
								
						
			break;

			
			case 'C':
						
			system("ps -aux");	
						
			break;

			
			default:
						
			printf("defaut error \n");	
						
			break;
										
		
					}

		
	
					
	}
	
		close(client_sockfd);
	
		return 0;

}

