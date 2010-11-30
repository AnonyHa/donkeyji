#include "afxdb.h"

struct game_server* gs_new()
{
	struct game_server* gs = (struct game_server*)malloc(sizeof(struct game_server));
	if (gs == NULL)
		return NULL;

	gs->_bev = NULL;//等到接收到连接
}

void gs_free(struct game_server* gs)
{
	if (gs == NULL)
		return;

	if (gs->_bev)
		bufferevent_free(gs->_bev);

	free(gs);
}

int gs_init(struct game_server* gs)
{}
