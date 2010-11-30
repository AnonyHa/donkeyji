#include <string.h>
#include "enet/enet.h"


//客户端调用时address为NULL
//服务器调用时address指定为服务器自身的地址，需要进行bind
ENetHost* enet_host_create(const ENetAddress* address, size_t peerCount, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
{
	ENetHost* host = (ENetHost*)enet_malloc(sizeof(ENetHost));
	ENetPeer* currentPeer;

	if (peerCount > ENET_PROTOCOL_MAXIMUM_PEER_ID) {
		//enet_host_destroy(host);
		return NULL;//???应该清理上面的堆吧
	}

	host->peers = (ENetPeer*)enet_malloc(peerCount * sizeof(ENetPeer));
	memset(host->peers, 0, peerCount * sizeof(ENetPeer));

	host->socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	//need to bind
	if (host->socket == ENET_SOCKET_NULL || (address != NULL && enet_socket_bind(host->socket, address) < 0)) {
		if (host->socket != ENET_SOCKET_NULL)
			enet_socket_destroy(host->socket);
		enet_free(host->peers);
		enet_free(host);
		return NULL;
	}

	enet_socket_set_option(host->socket, ENET_SOCKET_NONBLOCK, 1);
	//enet_socket_set_option(host->socket, ENET_SOCKET_BROADCAST, 1);//??为何要广播
	enet_socket_set_option(host->socket, ENET_SOCKET_RCVBUF, ENET_HOST_RECEIVE_BUFFER_SIZE);
	enet_socket_set_option(host->socket, ENET_SOCKET_SNDBUF, ENET_HOST_SEND_BUFFER_SIZE);

	if (address != NULL)
		host->address = *address;

	host->incomingBandwidth = incomingBandwidth;
	host->outgoingBandwidth = outgoingBandwidth;
	host->peerCount = peerCount;
	host->lastServicedPeer = host->peers;
	host->commandCount = 0;
	host->bufferCount = 0;
	host->receivedAddress.host = ENET_HOST_ANY;
	host->receivedAddress.port = 0;

	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		currentPeer->host = host;	
		currentPeer->incomingPeerID = currentPeer - host->peers;
		currentPeer->data = NULL;
		enet_list_clear(&currentPeer->acknowledgements);
		enet_list_clear(&currentPeer->outgoingReliableCommands);
		enet_list_clear(&currentPeer->outgoingUnreliableCommands);
		enet_list_clear(&currentPeer->sentReliableCommands);
		enet_list_clear(&currentPeer->sentUnreliableCommands);
		enet_peer_reset(currentPeer);
	}

	return host;
}

void enet_host_destroy(ENetHost* host)
{}

ENetPeer* enet_host_connect(ENetHost* host, const ENetAddress* address, size_t channelCount)
{
	ENetPeer* currentPeer;
	ENetChannel* channel;
	ENetProtocol command;

	if (channelCount < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT) {
		channelCount = ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT;
	} else {
		if (channelCount > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT)
			channelCount = ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT;
	}

	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		if (currentPeer->state == ENET_PEER_STATE_DISCONNECTED)
			break;
	}

	if (currentPeer >= &host->peers[host->peerCount])
		return NULL;

	currentPeer->state = ENET_PEER_STATE_CONNECTING;
	currentPeer->address = *address;//连接对方的地址
	currentPeer->channels = (ENetChannel*)enet_malloc(channelCount * sizeof(ENetChannel));
	currentPeer->channelCount = channelCount;
	currentPeer->sessionID = (enet_uint32)enet_rand();

	if (host->outgoingBandwidth == 0)
		currentPeer->windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
	else
		currentPeer->windowSize = (host->outgoingBandwidth / ENET_PEER_WINDOW_SIZE_SCALE) * ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
	
	if (currentPeer->windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE) {
		currentPeer->windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
	} else {
		if (currentPeer->windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
			currentPeer->windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
	}

	for (channel = currentPeer->channels; channel<&currentPeer->channels[currentPeer->channelCount]; channel++) {
		channel->outgoingReliableSequenceNumber = 0;
		channel->incomingReliableSequenceNumber = 0;

		enet_list_clear(&channel->incomingReliableCommands);
		enet_list_clear(&channel->incomingUnreliableCommands);
	}

	command.header.command = ENET_PROTOCOL_COMMAND_CONNECT;
	command.header.channelID = 0xff;
	command.connect.windowSize = ENET_HOST_TO_NET_32(currentPeer->windowSize);
	command.connect.channelCount = ENET_HOST_TO_NET_32(channelCount);
	command.connect.sessionID = currentPeer->sessionID;

	enet_peer_queue_outgoing_command(currentPeer, &command, NULL, 0, 0);

	return currentPeer;
}
