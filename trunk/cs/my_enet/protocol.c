#include "enet/enet.h"

static size_t commandSize[ENET_PROTOCOL_COMMAND_COUNT] = {
	0,
	sizeof(ENetProtocolConnect)
};

size_t enet_protocol_command_size(enet_uint8 commandNumber)
{
	return commandSize[commandNumber & ENET_PROTOCOL_COMMAND_MASK];
}

static int enet_protocol_dispatch_incoming_commands(ENetHost* host, ENetEvent* event)
{}

static int enet_protocol_send_outgoing_commands(ENetHost* host, ENetEvent* event)
{}

static int enet_protocol_receive_incoming_commands(ENetHost* host, ENetEvent* event)
{}

int enet_host_service(ENetHost* host, ENetEvent* event, enet_uint32 timeout)
{
	enet_uint32 waitCondition;

	if (event != NULL) {
		event->type = ENET_EVENT_TYPE_NONE;
		event->peer = NULL;
		event->packet = NULL;

		switch (enet_protocol_dispatch_incoming_commands(host, event)) {
		case 1:
			return 1;
		case -1:
			return -1;
		default:
			break;
		}
	}

	host->serviceTime = enet_time_get();
	timeout += host->serviceTime;

	do {
		switch (enet_protocol_send_outgoing_commands(host, event)) {}

		switch (enet_protocol_receive_incoming_commands(host, event)) {}

		switch (enet_protocol_send_outgoing_commands(host, event)) {}

		if (event != NULL) {
			event->type = ENET_EVENT_TYPE_NONE;
			event->peer = NULL;
			event->packet = NULL;

			switch (enet_protocol_dispatch_incoming_commands(host, event)) {
			case 1:
				return 1;
			case -1:
				return -1;
			default:
				break;
			}
		}

		host->serviceTime = enet_time_get();

		waitCondition = ENET_SOCKET_WAIT_RECEIVE;

		if (enet_socket_wait(host->socket, &waitCondition, 0)) {}
	} while(waitCondition == ENET_SOCKET_WAIT_RECEIVE);
}
