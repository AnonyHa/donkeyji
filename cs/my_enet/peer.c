#include "enet/enet.h"

int enet_peer_send(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet)
{}

ENetPacket* enet_peer_receive(ENetPeer* peer, enet_uint8 channelID)
{
}

static void enet_peer_reset_outgoing_commands(ENetList* queue)
{
	ENetOutgoingCommand* outgoingCommand;
	while (!enet_list_empty(queue)) {
		outgoingCommand = (ENetOutgoingCommand*)enet_list_remove(enet_list_begin(queue));
		if (outgoingCommand->packet != NULL) {
			--outgoingCommand->packet->referenceCount;
			if (outgoingCommand->packet->referenceCount == 0)
				enet_packet_destroy(outgoingCommand->packet);
		}
		enet_free(outgoingCommand);
	}
}

static void enet_peer_reset_incoming_commands(ENetList* queue)
{
	ENetIncomingCommand* incomingCommand;
	while (!enet_list_empty(queue)) {
		incomingCommand = (ENetIncomingCommand*)enet_list_remove(enet_list_begin(queue));
		if (incomingCommand->packet != NULL) {
			--incomingCommand->packet->referenceCount;
			if (incomingCommand->packet->referenceCount == 0)
				enet_packet_destroy(incomingCommand->packet);
		}
		if (incomingCommand->fragments != NULL)
			enet_free(incomingCommand->fragments);

		enet_free(incomingCommand);
	}
}

void enet_peer_reset_queues(ENetPeer* peer)
{
	ENetChannel* channel;
	while (!enet_list_empty(&peer->acknowledgements))
		enet_free(enet_list_remove(enet_list_begin(&peer->acknowledgements)));

	enet_peer_reset_outgoing_commands(&peer->sentReliableCommands);
	enet_peer_reset_outgoing_commands(&peer->sentUnreliableCommands);
	enet_peer_reset_outgoing_commands(&peer->outgoingReliableCommands);
	enet_peer_reset_outgoing_commands(&peer->outgoingUnreliableCommands);

	//保证peer->channels为空，因为此时peer->channels并不创建，在处理对方连接时才创建peer->channels
	if (peer->channels != NULL && peer->channelCount > 0) {
		for (channel = peer->channels; channel < &peer->channels[peer->channelCount]; channel++) {
			enet_peer_reset_incoming_commands(&channel->incomingReliableCommands);
			enet_peer_reset_incoming_commands(&channel->incomingUnreliableCommands);
		}
		enet_free(peer->channels);
	}
	peer->channels = NULL;
	peer->channelCount = 0;
}

void enet_peer_reset(ENetPeer* peer)
{
	peer->outgoingPeerID = ENET_PROTOCOL_MAXIMUM_PEER_ID;
	peer->state = ENET_PEER_STATE_DISCONNECTED;

	peer->incomingDataTotal = 0;
	peer->outgoingDataTotal = 0;

	peer->lastSendTime = 0;
	peer->lastReceiveTime = 0;

	peer->nextTimeout = 0;
	peer->earliestTimeout = 0; 

	peer->packetLossEpoch = 0;
	peer->packetSent = 0;
	peer->packetLost = 0;
	peer->packetLoss = 0;
	peer->packetLossVariance = 0;

	peer->lastRoundTripTime = ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer->lowestRoundTripTime = ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer->lastRoundTripTimeVariance = 0;
	peer->highestRoundTripTimeVariance = 0;
	peer->roundTripTime = ENET_PEER_DEFAULT_ROUND_TRIP_TIME;
	peer->roundTripTimeVariance = 0;

	peer->reliableDataInTransit = 0;

	enet_peer_reset_queues(peer);
}


//command包装成outgoing，放进queue
ENetOutgoingCommand* enet_peer_queue_outgoing_command(ENetPeer* peer, const ENetProtocol* command, ENetPacket* packet, enet_uint32 offset, enet_uint16 length)
{
	ENetChannel* channel = &peer->channels[command->header.channelID];
	ENetOutgoingCommand* outgoingCommand;

	peer->outgoingDataTotal += enet_protocol_command_size(command->header.command) + length;
	outgoingCommand = (ENetOutgoingCommand*)enet_malloc(sizeof(ENetOutgoingCommand));

	if (command->header.channelID == 0xff) {
		//用到那个座位outgoing的确认序列号
		++peer->outgoingReliableSequenceNumber;//peer的确认序列号
		outgoingCommand->reliableSequenceNumber = peer->outgoingReliableSequenceNumber;
		outgoingCommand->unreliableSequenceNumber = 0;
	} else {

	}
	
	outgoingCommand->sentTime = 0;
	outgoingCommand->roundTripTimeout = 0;
	outgoingCommand->roundTripTimeoutLimit = 0;
	outgoingCommand->fragmentOffset = offset;
	outgoingCommand->fragmentLength = length; 
	outgoingCommand->packet = packet;
	outgoingCommand->command = *command;

	outgoingCommand->command.header.reliableSequenceNumber = ENET_HOST_TO_NET_16(outgoingCommand->reliableSequenceNumber);

	if (packet != NULL)
		++packet->referenceCount;

	if (command->header.command & ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE)
		enet_list_insert(enet_list_end(&peer->outgoingReliableCommands), outgoingCommand);
	else
		enet_list_insert(enet_list_end(&peer->outgoingUnreliableCommands), outgoingCommand);
}
