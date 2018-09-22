#include <stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>

// 显示颜色定义
#define DEFAULT "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;38;44m"
#define YELLOW "\033[1;33m"
#define CMDCOLOR "\033[1;38;42m"

// 错误类型号定义
#define ARG_ERROR 1
#define CREATE_SOCKET_ERROR 2
#define SET_SOCKET_ERROR 3
#define SOCKET_ERROR 4
#define LISTEN_ERROR 5
#define MESSAGE_SEND_ERROR 6
#define INVALID_PORT 7
#define INVALID_HOST 8
#define CONNECT_ERROR 9

// 登陆状态
#define LOG_IN_SUCS 0
#define LOG_IN_FAIL 1

// 应答码
#define SERVICE_READY 220
#define NEED_PASS 331
#define LOGIN_SUCS 230
#define CONTROL_CLOSE 221
#define PATHNAME_CREATE 257
#define PASV_MODE 227
#define NO_SUCH_FILE 550

// 命令号
#define GET 1
#define PUT 2
#define PWD 3
#define DIR 4
#define CD 5
#define HELP 6
#define QUIT 7
#define NOCMD -1

// 其他
#define DEFAULT_PORT "21"
#define MAX_PORT 65536
#define big_size 1024
#define BUF_SIZE 1024
#define MSG_SIZE 256
#define small_size 50
#define USR_PAS_SIZE 50

// 控制连接和数据连接的socket
struct sockaddr_in server, server_data;
// 服务器主机hostent*指针
struct hostent* hent;
// 用户名字串
char user[USR_PAS_SIZE];
// 密码字串
char pass[USR_PAS_SIZE];
// 数据连接端口
int data_port;

// 打印当前时间，用于显示发送命令和接收应答的时间戳
void print_time() {
	time_t t;
	struct tm* timenow;
	time(&t);
	timenow = localtime(&t);
	printf("[");
	if (timenow->tm_hour >= 0 && timenow->tm_hour <= 9)
		printf("0");
	printf("%d:", timenow->tm_hour);
	if (timenow->tm_min >= 0 && timenow->tm_min <= 9)
		printf("0");
	printf("%d:", timenow->tm_min);
	if (timenow->tm_sec >= 0 && timenow->tm_sec <= 9)
		printf("0");
	printf("%d] ", timenow->tm_sec);
}

// 当出现错误时的处理，参数为 错误信息字串 和 错误号
void error_handle(char* err_info, int err_no) {
	printf(RED "# error %d: %s\n" DEFAULT, err_no, err_info);
	exit(err_no);
}

// 发送命令，参数为 socket号 、 命令标示 和 命令参数（可选）
void send_command(int sock_fd, const char* cmd, const char* info) {
	int ret;
	char buf[BUF_SIZE] = {0};
	char buf2[BUF_SIZE] = {0};
	strcpy(buf, cmd);
	if ((info != NULL) && (strncmp(cmd, "PASS", 4) != 0)) {
		strcat(buf, info);
		strcat(buf, "\r\n");
		printf(YELLOW);
		print_time();
		printf("%s" DEFAULT, buf);
	}
	else if (strncmp(cmd, "PASS", 4) == 0) {
		strcpy(buf2, buf);
		strcat(buf2, "(Hidden)\n");
		strcat(buf, info);
		strcat(buf, "\r\n");
		printf(YELLOW);
		print_time();
		printf("%s" DEFAULT, buf2);
	}

	if ((ret = send(sock_fd, buf, strlen(buf), 0)) < 0) {
		char errormsg[BUF_SIZE] = {0};
		strcpy(errormsg, "Send command error: ");
		strcat(errormsg, buf);
		error_handle(errormsg, MESSAGE_SEND_ERROR);
	}
}

// 控制连接获取服务器应答，返回应答码
int get_reply(int sockfd) {
	int r_code, bytes;
	char buf[BUF_SIZE] = {0};
	char nbuf[5] = {0};	

	if ((bytes = read(sockfd, buf, BUF_SIZE - 2)) > 0) {
		r_code = atoi(buf);
		buf[bytes] = '\0';
		printf(GREEN);		
		print_time();
		printf("%s" DEFAULT, buf);
	}
	else
		return -1;
	if (buf[3] == '-') {
		char* newline = strchr(buf, '\n');
		if (*(newline+1) == '\0') {
			while ((bytes = read(sockfd, buf, BUF_SIZE - 2)) > 0) {
				buf[bytes] = '\0';
				printf(GREEN);		
				print_time();				
				printf("%s" DEFAULT, buf);
				if (atoi(buf) == r_code)
					break;
			}
		}
	}
	if (r_code == PASV_MODE) {
		char* begin = strrchr(buf, ',')+1;
		char* end = strrchr(buf, ')');
		strncpy(nbuf, begin, end - begin);
		nbuf[end-begin] = '\0';
		data_port = atoi(nbuf);
		buf[begin-1-buf] = '\0';
		end = begin - 1;
		begin = strrchr(buf, ',')+1;
		strncpy(nbuf, begin, end - begin);
		nbuf[end-begin] = '\0';
		data_port += 256 * atoi(nbuf);
	}

	return r_code;
}

// 数据连接获取服务器传输的数据
void get_data(int sockfd) {
	int r_code, bytes;
	char buf[BUF_SIZE] = {0};

	while ((bytes = read(sockfd, buf, BUF_SIZE - 2)) > 0) {
		buf[bytes] = '\0';
		printf("%s", buf);
	}
}

// 连接到服务器，参数为 IP地址 和 端口号，返回是否连接成功的编号
int connect_to_host(char* ip, char* pt) {
	int sockfd;
	int port = atoi(pt);
	char errormsg[BUF_SIZE];
	struct timeval tout;		
	
	if (port <= 0 || port >= MAX_PORT) {
		strcpy(errormsg, "Invalid Port Number: ");
		strcat(errormsg, pt);
		error_handle(errormsg, INVALID_PORT);
	}
	//memset(0, &server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((server.sin_addr.s_addr = inet_addr(ip)) < 0) {
		if ((hent = gethostbyname(ip)) != 0)
			memcpy(&server.sin_addr, hent->h_addr, sizeof(&(server.sin_addr)));
		else {
			strcpy(errormsg, "Invalid Host: ");
			strcat(errormsg, ip);
			error_handle(errormsg, INVALID_HOST);
		}
	}
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error_handle("Create Socket Error", CREATE_SOCKET_ERROR);
	}
	
	//tout.tv_sec = 0;
	///tout.tv_usec = 100000;
	//setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO, &tout,sizeof(tout));
	
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
		error_handle("Cannot connect to server", CONNECT_ERROR);
	printf(BLUE "Successfully connected to server: %s:%d\n" DEFAULT, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
	return sockfd;
}

// 键入用户名
void input_user() {
	char buf[BUF_SIZE];
	printf(WHITE "User (Null for Anonymous):" DEFAULT);
	printf(" ");
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] != '\n')
		strncpy(user, buf, strlen(buf) - 1);
	else
		strncpy(user, "anonymous", 9);
}

// 键入密码
void input_pass() {
	char buf[BUF_SIZE];
	printf(WHITE "Password (Null for no password):" DEFAULT);
	printf(" ");
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] != '\n')
		strncpy(pass, buf, strlen(buf) - 1);
	else
		strncpy(pass, "anonymous", 9);
}

// 用户登录模块，调用input_user()和input_pass()
int user_log_in(int sockfd) {
	bzero(user, sizeof(user));
	bzero(pass, sizeof(pass));
	input_user();
	send_command(sockfd, "USER ", user);
	if (get_reply(sockfd) == NEED_PASS) {
		input_pass();
		send_command(sockfd, "PASS ", pass);
		if (get_reply(sockfd) != LOGIN_SUCS) {
			printf(RED "Password wrong.\n" DEFAULT);
			return LOG_IN_FAIL;
		}
		else {
			printf(BLUE "User %s Log-in Success.\n" DEFAULT, user);
			return LOG_IN_SUCS;
		}
	}
	else {
		printf("error\n");
		return LOG_IN_FAIL;
	}
}

// 提取用户输入命令的关键字，返回程序中与FTP功能相应的代码
int command_num(char* cmd) {
	cmd[strlen(cmd)-1] = '\0';	
	if (strncmp(cmd, "get", 3) == 0)
		return GET;
	if (strncmp(cmd, "put", 3) == 0)
		return PUT;
	if (strcmp(cmd, "pwd") == 0)
		return PWD;
	if (strcmp(cmd, "dir") == 0)
		return DIR;
	if (strncmp(cmd, "cd", 2) == 0)
		return CD;
	if (strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0)
		return HELP;
	if (strcmp(cmd, "quit") == 0)
		return QUIT;
	return NOCMD;
}

// FTP协议：取远方的一个文件
void command_get(int sockfd, char* cmd) {
	int data_sock, i, bytes;
	char filename[BUF_SIZE];
	char buf[BUF_SIZE];
	
	for (i = 0; i < strlen(cmd); i++) {
		if (cmd[i] == ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf("Command error: %s", cmd);
		return;
	}
	for (; i < strlen(cmd); i++) {
		if (cmd[i] != ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf("Command error: %s", cmd);
		return;
	}
	strncpy(filename, cmd+i, strlen(cmd+i)+1);

	send_command(sockfd, "TYPE ", "I");
	get_reply(sockfd);
	send_command(sockfd, "PASV", "");
	if (get_reply(sockfd) != PASV_MODE) {
		printf(RED "error.\n" DEFAULT);
		return;
	}
		
	//printf("%d", data_port);
	server.sin_port = htons(data_port);
	if ((data_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error_handle("Create Socket Error", CREATE_SOCKET_ERROR);
	}

	if (connect(data_sock, (struct sockaddr*)&server, sizeof(server)) < 0)
		error_handle("Cannot connect to server", CONNECT_ERROR);
	printf(BLUE "Data Connection Successfully: %s:%d\n" DEFAULT, inet_ntoa(server.sin_addr), ntohs(server.sin_port));	

	send_command(sockfd, "RETR ", filename);
	if (get_reply(sockfd) == NO_SUCH_FILE) {
		close(sockfd);
		return;
	}
	FILE* dst_file;
	if ((dst_file = fopen(filename, "wb")) == NULL) {
		printf("error.");
		close(sockfd);
		return;
	}
	
	while ((bytes = read(data_sock, buf, BUF_SIZE)) > 0) {
		fwrite(buf, 1, bytes, dst_file);
		//printf("%s", buf);
	}
	
	close(data_sock);
	get_reply(sockfd);
	fclose(dst_file);

	//printf("command_get\n");
}

// FTP协议：传给远方一个文件
void command_put(int sockfd, char* cmd) {
	int data_sock, i, bytes;
	char filename[BUF_SIZE];
	char buf[BUF_SIZE];
	
	for (i = 0; i < strlen(cmd); i++) {
		if (cmd[i] == ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf("Command error: %s", cmd);
		return;
	}
	for (; i < strlen(cmd); i++) {
		if (cmd[i] != ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf("Command error: %s", cmd);
		return;
	}
	strncpy(filename, cmd+i, strlen(cmd+i)+1);

	send_command(sockfd, "PASV", "");
	if (get_reply(sockfd) != PASV_MODE) {
		printf("error.");
		return;
	}
	FILE* src_file;
	if ((src_file = fopen(filename, "rb")) == NULL) {
		printf("error.");
		return;
	}	
	//printf("%d", data_port);
	server.sin_port = htons(data_port);
	if ((data_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error_handle("Create Socket Error", CREATE_SOCKET_ERROR);
	}

	if (connect(data_sock, (struct sockaddr*)&server, sizeof(server)) < 0)
		error_handle("Cannot connect to server", CONNECT_ERROR);
	printf(BLUE "Data Connection Successfully: %s:%d\n" DEFAULT, inet_ntoa(server.sin_addr), ntohs(server.sin_port));	

	send_command(sockfd, "STOR ", filename);
	if (get_reply(sockfd) == NO_SUCH_FILE) {
		close(data_sock);
		fclose(src_file);
		return;
	}
	
	while ((bytes = fread(buf, 1, BUF_SIZE, src_file)) > 0) {
		send(data_sock, buf, bytes, 0);
		//printf("%s", buf);
	}
	
	close(data_sock);
	get_reply(sockfd);
	fclose(src_file);

	//printf("command_put\n");
}

// FTP协议：显示远方当前目录
void command_pwd(int sockfd) {
	send_command(sockfd, "PWD", "");
	if (get_reply(sockfd) != PATHNAME_CREATE)
		error_handle("Wrong Reply for PWD.", CONNECT_ERROR);
	//printf("command_pwd\n");
}

// FTP协议：列出远方当前目录
void command_dir(int sockfd) {
	int data_sock;
	send_command(sockfd, "PASV", "");
	if (get_reply(sockfd) != PASV_MODE) {
		printf("error.");
		return;
	}

	server.sin_port = htons(data_port);
	if ((data_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error_handle("Create Socket Error", CREATE_SOCKET_ERROR);
	}

	if (connect(data_sock, (struct sockaddr*)&server, sizeof(server)) < 0)
		error_handle("Cannot connect to server", CONNECT_ERROR);
	printf(BLUE "Data Connection Successfully: %s:%d\n" DEFAULT, inet_ntoa(server.sin_addr), ntohs(server.sin_port));	

	send_command(sockfd, "LIST ", "-al");
	get_reply(sockfd);
	printf("\n");
	get_data(data_sock);
	printf("\n");
	close(data_sock);
	get_reply(sockfd);
	
	//printf("command_dir\n");
}

// FTP协议：改变远方当前目录
void command_cd(int sockfd, char* cmd) {
	int i = 0;
	char buf[BUF_SIZE];

	for (i = 0; i < strlen(cmd); i++) {
		if (cmd[i] == ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf(RED "Command error: %s\n" DEFAULT, cmd);
		return;
	}
	for (; i < strlen(cmd); i++) {
		if (cmd[i] != ' ')
			break;
	}
	if (i == strlen(cmd)) {
		printf(RED "Command error: %s\n" DEFAULT, cmd);
		return;
	}
	strncpy(buf, cmd+i, strlen(cmd+i)+1);
	send_command(sockfd, "CWD ", buf);
	get_reply(sockfd);
	//printf("command_cd\n");
}

// FTP协议：显示你提供的命令
void command_help() {
	printf("\n");
	printf(CMDCOLOR " get " DEFAULT);
	printf("\n\t get a file from server.\n");
	printf(CMDCOLOR " put " DEFAULT);
	printf("\n\t send a file to server.\n");
	printf(CMDCOLOR " pwd " DEFAULT);
	printf("\n\t get the present directory on server.\n");
	printf(CMDCOLOR " dir" DEFAULT);
	printf("\n\t list the directory on server.\n");
	printf(CMDCOLOR " cd " DEFAULT);
	printf("\n\t change the directory on server.\n");
	printf(CMDCOLOR " ?/help " DEFAULT);
	printf("\n\t help you know how to use the command.\n");
	printf(CMDCOLOR " quit " DEFAULT);
	printf("\n\t quit FTP client.\n");
	printf("\n");
	//printf("command_help\n");
}

// FTP协议：退出返回
void command_quit(int sockfd) {
	send_command(sockfd, "QUIT", "");
	if (get_reply(sockfd) == CONTROL_CLOSE)
		printf(BLUE "Logged out.\n" DEFAULT);
	//printf("command_quit\n");
}

// 主函数，调用登录函数user_log_in，然后进行交互
int main(int argc, char* argv[]) {
	int sockfd, quitnum = 0;
	char buf[BUF_SIZE];
	if (argc != 2 && argc != 3) {
		printf(YELLOW "Usage: %s <host>[ <port>]\n" DEFAULT, argv[0]);
		exit(ARG_ERROR);
	}
	if (argc == 2)
		sockfd = connect_to_host(argv[1], DEFAULT_PORT);
	else
		sockfd = connect_to_host(argv[1], argv[2]);

	if (get_reply(sockfd) != SERVICE_READY)
		error_handle("Service Error", CONNECT_ERROR);
	while (user_log_in(sockfd) != LOG_IN_SUCS) {
		printf(RED "User Log-on Failed. Please try again.\n" DEFAULT);
	}
	while (1) {
		printf(WHITE "[FTP User Command]" DEFAULT);
		printf(" ");
		fgets(buf, sizeof(buf), stdin);
		
		switch (command_num(buf)) {
			case GET:
				command_get(sockfd, buf);
				break;
			case PUT:
				command_put(sockfd, buf);
				break;
			case PWD:
				command_pwd(sockfd);
				break;
			case DIR:
				command_dir(sockfd);
				break;
			case CD:
				command_cd(sockfd, buf);
				break;
			case HELP:
				command_help();
				break;
			case QUIT:
				command_quit(sockfd);
				quitnum = 1;
				break;
			default:
				command_help();
				break;
		}

		if (quitnum == 1) {
			break;
		}
	}

	
	/*char buf[big_size] = {0}, file[small_size], host_name[small_size] = {0};
	char sendmsg[MSG_SIZE] = {0};
	char user[] = "USER comic9\n";
	char pass[] = "PASS allhailnet9\n";

	get_reply(sockfd);
	strcpy(sendmsg, "USER ");
	strcat(sendmsg, "comic9\r\n");

	//printf("%d %s", sizeof(user), user);
	write(sockfd, user, sizeof(user));
	get_reply(sockfd);

	write(sockfd, pass, sizeof(pass));
	get_reply(sockfd);
	*/
	close(sockfd);
}
