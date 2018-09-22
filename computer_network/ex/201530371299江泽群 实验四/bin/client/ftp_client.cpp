#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

int initClient() {
    struct sockaddr_in client_addr;
    char buffer[1024];
    memset(&client_addr, 0, sizeof(client_addr));
    memset(buffer, 0, 1024);
    client_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &(client_addr.sin_addr));
    client_addr.sin_port = htons(0);
    int client_sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(client_sockfd < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    if(bind(client_sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    char * client_ip = inet_ntoa(client_addr.sin_addr);
    cout << "Client socket is bound with ip: " << client_ip << endl;
    return client_sockfd;
}

int initServer(int client_sockfd, char *server_ip) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    server_addr.sin_port = htons(1234);
    socklen_t server_addr_length = sizeof(server_addr);

    if(connect(client_sockfd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", server_ip);
        exit(1);
    }
    return 1;
}

void receiveFileList(int client_sockfd) {
    int recv_len;
    char buffer[1024];
    cout << "File List: " << endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        recv_len = recv(client_sockfd, buffer, 1024, 0);
        if(recv_len < 0)
        {
            cout << "Unable to receive filename from server!" << endl;
            break;
        }
        if (!strcmp(buffer, "end")) {
            break;
        }
        cout << buffer << endl;
    }
}

FILE* chooseFile(int client_sockfd, char *&filename) {
    char buffer[1024];
    char path[600];
    getcwd(path, 600);
    strcat(path, "/");
    memset(buffer, 0, sizeof(buffer));
    cout << "Input filename to choose file: ";
    filename = new char[512];
    cin >> filename;
    strncpy(buffer, filename, strlen(filename) > 1024? 1024 : strlen(filename));
    send(client_sockfd, buffer, 1024, 0);
    strcat(path, filename);
    FILE * fp = fopen(path, "w");
    if(!fp )
    {
        cout << "Unable to write " << filename << endl;
        printf("File:\t%s Can Not Open To Write\n", filename);
        exit(1);
    }
    return fp;
}

void downloadFile(int client_sockfd, FILE *fp, char *filename) {
    char buffer[1024];
    int recv_len;
    int write_len;
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        recv_len = static_cast<int>(recv(client_sockfd, buffer, 1024, 0));
        if(recv_len < 0)
        {
            cout << "Unable to receive filename from server!" << endl;
            break;
        }
        write_len = static_cast<int>(fwrite(buffer, sizeof(char), recv_len, fp));
        if (write_len<recv_len)
        {
            cout << "Unable to write " << filename << endl; 
            break;
        }
        if (recv_len <= 1024) {
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int client_sockfd = initClient();
    char *filename;
    char *server_ip = (char *)"172.28.234.86";
    initServer(client_sockfd, server_ip);

    receiveFileList(client_sockfd);
    FILE *fp = chooseFile(client_sockfd, filename);
    
    cout << "Start to download..." << endl;
    cout << filename << endl;
    downloadFile(client_sockfd, fp, filename);
    cout << "File " << filename << " is stored." << endl;
    fclose(fp);
    close(client_sockfd);
    return 0;
}