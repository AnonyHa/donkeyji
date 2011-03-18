#ifndef _GAME_SERVER_H
#define _GAME_SERVER_H

#include <event.h>

struct game_server {
	int _fd;
	int _state;
	struct bufferevent* _bev;
};

struct game_server* gs_new();

int gs_init(struct game_server* gs);

void gs_free(struct game_server* gs);
#endif
