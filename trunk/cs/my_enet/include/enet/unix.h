#ifndef _UNIX_H
#define _UNIX_H

#include <stdlib.h>
#include <sys/types.h>

typedef int ENetSocket;

enum {
	ENET_SOCKET_NULL = -1
};

#define ENET_HOST_TO_NET_16(value) (htons(value))
#define ENET_HOST_TO_NET_32(value) (htonl(value))

#define ENET_NET_TO_HOST_16(value) (ntohs(value))
#define ENET_NET_TO_HOST_32(value) (ntohl(value))

typedef struct
	{} ENetBuffer;

#define ENET_CALLBACK
#define ENET_API extern

typedef fd_set ENetSocketSet;

#endif
