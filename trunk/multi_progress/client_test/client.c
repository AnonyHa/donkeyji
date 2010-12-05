#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int port = 10031;
const char* ip = "127.0.0.1";

int create_socket()
{
	int sock;
	int ret;
	struct sockaddr_in addr;
	int i;
	char buf[1024];

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return -1;

	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));//nonblocking socket¡¢º¥∑µªÿ
	if (ret == -1) {
		return -1;
	}

	for (i=0; i<10; i++) {
		ret = send(sock, "$hello?x=4&y=5#", 15, 0);
		if (ret < 0) {
			perror("send");
			return -1;
		}
		//printf("send %d\n", ret);
	}
	int shutdown = 0;
	while (!shutdown) {
		ret = recv(sock, buf, 1024, 0);
		if (ret < 0) {
			perror("recv");	
			shutdown = 1;
		} else {
			printf("recv len = %d, data = %s\n", ret, buf);
			break;
		}
	}

	return 0; 
}

int main()
{
	int ret;
	ret = create_socket();
	if (ret < 0)
		return -1;
	return 0;
}
