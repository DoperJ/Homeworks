#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>

using std::cout;
using std::cin;
using std::endl;

int initServer() {
    sockaddr_in my_addr;
    memset(&my_addr,0,sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htons(INADDR_ANY);
    my_addr.sin_port = htons(1234);
    int server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        cout << "Unable to create a socket for my_addr!" << endl;
        exit(1);
    }
    if (bind(server_sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr))) {
        cout << "Unable to bind!" << endl;
        exit(1);
    }
    // accept 20 connection concurrently at most
    if (listen(server_sockfd, 20) == -1) {
        cout << "Unable to listen" << endl;
        exit(1);
    }
    cout << "Server ready to listen..." << endl;
    return server_sockfd;
}

void sendFileList(int client_sockfd)
{
    DIR *dir;
    struct dirent *ptr;
    char buffer[1024];
    dir = opendir(".");
    if (!dir) {
        cout << "No File Directory!" << endl;
        exit(1);
    }
    while((ptr = readdir(dir)) != NULL)
    {
        memset(buffer, 0, sizeof(buffer));
        if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, "..")
            && strcmp(ptr->d_name, "ftp_server.cpp") && strcmp(ptr->d_name, "ftp_server")) {
            strncpy(buffer, ptr->d_name, strlen(ptr->d_name) > 1024? 1024 : strlen(ptr->d_name));
            send(client_sockfd, buffer, 1024, 0);
        }
    }
    char *end_flag = (char *)"end";
    send(client_sockfd, end_flag, sizeof(end_flag), 0);
    closedir(dir);
}

int main() {
    char buffer[1024];
    char path[600];
    getcwd(path, 600);
    strcat(path, "/");
    char filename[512];
    int file_len;
    int server_sockfd = initServer();
    while (true) {
        struct sockaddr_in client;
        socklen_t length = sizeof(client);
        int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client, &length);
        if (client_sockfd == -1) {
            cout << "Unable to accept!" << endl;
            exit(1);
        }
        cout << "Server: get connection from: " << inet_ntoa(client.sin_addr) << "\t";
        if (fork() == 0) {
            memset(buffer, 0, sizeof(buffer));
            memset(filename, 0, sizeof(filename));
            sendFileList(client_sockfd);
            if (recv(client_sockfd, buffer, sizeof(buffer), 0) == -1) {
                cout << "Unable to receive!" << endl;
            }
            file_len = strlen(buffer) > 512 ? 512 : strlen(buffer);
            strncpy(filename, buffer, file_len);
            cout << "Client asked for file: " << filename << endl;
            strcat(path, filename);
            FILE *file = fopen(path, "r");
            if (file) {
                int data_len;
                while((data_len = fread(buffer, sizeof(char), sizeof(buffer), file)) > 0)
                {
                    int send_len = send(client_sockfd,buffer,data_len,0);
                    if(send_len < 0)
                    {
                        cout << "Unable to send file!" << endl;
                        break;
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
                fclose(file);
                cout << "File sent!" << endl;
            }
            else {
                cout << "Unable to open file!" << endl;
                exit(1);
            }
        }
        while(waitpid(-1, NULL, WNOHANG) > 0);
    }
    close(server_sockfd);
    return 0;
}