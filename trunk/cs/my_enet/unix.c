#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

#include "enet/enet.h"

static enet_uint32 timeBase = 0;

//--------------enet-----------------
int enet_initialize(void)
{
	return 0;
}

int enet_deinitialize(void)
{
	return 0;
}

//-----------------time-------------------
enet_uint32 enet_time_get(void)
{
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000 - timeBase;
}

void enet_time_set(enet_uint32 newTimeBase)
{
}

//----------------socket-------------------
ENetSocket enet_socket_create(ENetSocketType type)
{
	return socket(PF_INET, type == ENET_SOCKET_TYPE_DATAGRAM ? SOCK_DGRAM : SOCK_STREAM, 0);
}

int enet_socket_bind(ENetSocket socket, const ENetAddress* address)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(struct sockaddr_in));

	sin.sin_family = AF_INET;

	if (address != NULL) {
		sin.sin_port = ENET_HOST_TO_NET_16(address->port);
		sin.sin_addr.s_addr = address->host;
	} else {
		sin.sin_port = 0;
		sin.sin_addr.s_addr = INADDR_ANY;
	}

	return bind(socket, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
}

int enet_socket_listen(ENetSocket socket, int backlog)
{
	return listen(socket, backlog < 0 ? SOMAXCONN : backlog);//SOMAXCONN
}

int enet_socket_send(ENetSocket socket, const ENetAddress* address, const ENetBuffer* buffers, size_t bufferCount)
{
	struct msghdr msgHdr;
	struct sockaddr_in sin;
	int sentLength;
	memset(&msgHdr, 0, sizeof(struct msghdr));

	if (address != NULL) {
		sin.sin_family = AF_INET;
		sin.sin_port = ENET_HOST_TO_NET_16(address->port);
		sin.sin_addr.s_addr = address->host;

		msgHdr.msg_name = &sin;//address
		msgHdr.msg_namelen = sizeof(struct sockaddr_in);
	}

	msgHdr.msg_iov = (struct iovec*)buffers;
	msgHdr.msg_iovlen = bufferCount;

	sentLength = sendmsg(socket, &msgHdr, MSG_NOSIGNAL);

	if (sentLength == -1) {
		if (errno == EWOULDBLOCK) {
			return 0;
		}

		return -1;
	}

	return sentLength;
}

//host->receivedAddress传给address
int enet_socket_receive(ENetSocket socket, ENetAddress* address, ENetBuffer* buffers, size_t bufferCount)
{
	struct msghdr msgHdr;
	struct sockaddr_in sin;
	int recvLength;
	memset(&msgHdr, 0, sizeof(struct msghdr));

	if (address != NULL) {
		msgHdr.msg_name = &sin;
		msgHdr.msg_namelen = sizeof(struct sockaddr_in);
	}

	msgHdr.msg_iov = (struct iovec*)buffers;
	msgHdr.msg_iovlen = bufferCount;

	//会填充sin
	recvLength = recvmsg(socket, &msgHdr, MSG_NOSIGNAL);

	if (recvLength == -1) {
		if (errno == EWOULDBLOCK) {
			return 0;
		}

		return -1;
	}

	if (msgHdr.msg_flags & MSG_TRUNC) { //????
		return -1;
	}

	//接收的地址填充到host->receivedAddress
	if (address != NULL) {
		address->host = (enet_uint32)sin.sin_addr.s_addr;
		address->port = ENET_NET_TO_HOST_16(sin.sin_port);
	}
}

//timeout: 毫秒
int enet_socket_wait(ENetSocket socket, enet_uint32* condition, enet_uint32 timeout)
{
	fd_set readSet, writeSet;
	struct timeval timeVal;
	int selectCount;

	timeVal.tv_sec = timeout / 1000;
	timeVal.tv_usec = (timeout % 1000) * 1000;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);

	if (*condition & ENET_SOCKET_WAIT_SEND) {
		FD_SET(socket, &writeSet);
	}

	if (*condition & ENET_SOCKET_WAIT_RECEIVE) {
		FD_SET(socket, &readSet);
	}

	selectCount = select(socket + 1, &readSet, &writeSet, NULL, &timeVal);

	if (selectCount < 0) {
		return -1;
	}

	*condition = ENET_SOCKET_WAIT_NONE;

	if (selectCount == 0) {
		return 0;
	}

	if (FD_ISSET(socket, &writeSet)) {
		*condition |= ENET_SOCKET_WAIT_SEND;
	}

	if (FD_ISSET(socket, &readSet)) {
		*condition |= ENET_SOCKET_WAIT_RECEIVE;
	}

	return 0;
}

int enet_socket_set_option(ENetSocket socket, ENetSocketOption option, int value)
{
	int result = -1;

	switch (option) {
	case ENET_SOCKET_NONBLOCK:
		result = fcntl(socket, F_SETFL, O_NONBLOCK | fcntl(socket, F_GETFL));
		break;
	case ENET_SOCKET_BROADCAST:
		result = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(int));
		break;
	case ENET_SOCKET_RCVBUF:
		result = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&value, sizeof(int));
		break;
	case ENET_SOCKET_SNDBUF:
		result = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&value, sizeof(int));
		break;
	case ENET_SOCKET_REUSEADDR:
		result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(int));
		break;
	default:
		break;
	}

	return result == -1 ? -1 : 0;
}

void enet_socket_destroy(ENetSocket socket)
{
	close(socket);
}
//---------------address------------------
//name = "127.0.0.1", gethostbyname也会成功
int enet_address_set_host(ENetAddress* address, const char* name, enet_uint16 port)
{
	struct hostent* hostEntry = NULL;
	struct hostent hostData;

	hostEntry = gethostbyname(name);

	if (hostEntry == NULL || hostEntry->h_addrtype != AF_INET) {
		if (!inet_aton(name, (struct in_addr*)&address->host)) {
			return -1;
		}

		return 0;
	}

	//save ip address
	address->host = *(enet_uint32*)hostEntry->h_addr_list[0];
	address->port = port;

	return 0;
}

//ip地址转换成字符串
int enet_address_get_host_ip(const ENetAddress* address, char* name, size_t nameLength)
{
	if (inet_ntop(AF_INET, &address->host, name, nameLength) == NULL) {
		return -1;
	}

	return 0;
}
