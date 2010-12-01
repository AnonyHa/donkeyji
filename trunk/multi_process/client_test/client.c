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
	int _sock;
	int ret;
	struct sockaddr_in addr;
	int i;

	_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock == -1)
		return -1;

	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	ret = connect(_sock, (struct sockaddr*)&addr, sizeof(addr));//nonblocking socket¡¢º¥∑µªÿ
	if (ret == -1) {
		return -1;
	}

	for (i=0; i<20; i++) {
		ret = send(_sock, "hello", 5, 0);
		if (ret < 0) {
			perror("send");
			return -1;
		}
		printf("send %d\n", ret);
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
