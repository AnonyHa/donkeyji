#ifndef _ENET_H
#define  _ENET_H

#ifdef __cplusplus 
extern "C"
{
#endif

#include <stdlib.h>

#include "enet/unix.h"
#include "enet/types.h"
#include "enet/protocol.h"
#include "enet/list.h"
#include "enet/callback.h"

typedef enum
{
	ENET_SOCKET_TYPE_STREAM  = 1,
	ENET_SOCKET_TYPE_DATAGRAM = 2
}ENetSocketType;

typedef enum
{
	ENET_SOCKET_WAIT_NONE = 0,
	ENET_SOCKET_WAIT_SEND = (1<<0),
	ENET_SOCKET_WAIT_RECEIVE = (1<<1)
}ENetSocketWait;

typedef enum
{
	ENET_SOCKET_NONBLOCK = 1,
	ENET_SOCKET_BROADCAST = 2,
	ENET_SOCKET_RCVBUF = 3,
	ENET_SOCKET_SNDBUF = 4,
	ENET_SOCKET_REUSEADDR = 5
}ENetSocketOption;

enum
{
	ENET_HOST_ANY = 0,
	ENET_HOST_BROADCAST = 0XFFFFFFFF,
	ENET_PORT_ANY = 0
};

typedef struct _ENetAddress
{
	enet_uint32 host;
	enet_uint32 port;
}ENetAddress;

typedef enum
{
	ENET_PACKET_FLAG_RELIABLE    = (1<<0),
	ENET_PACKET_FLAG_UNSEQUENCED = (1<<1),
	ENET_PACKET_FLAG_NO_ALLOCATE = (1<<2)
}ENetPacketFlag;

struct _ENetPacket;
typedef void (ENET_CALLBACK* ENetPacketFreeCallback) (struct _ENetPacket*);

typedef struct _ENetPacket
{
	size_t referenceCount;
	enet_uint32 flags;//是否reliable
	enet_uint8* data;
	size_t dataLength;
	ENetPacketFreeCallback freeCallback;
}ENetPacket;

typedef struct _ENetAcknowledgement
{
	ENetListNode acknowledgementList;
	enet_uint32 sentTime;
	ENetProtocol command;
}ENetAcknowledgement;

typedef struct _ENetOutgoingCommand
{
	ENetListNode outgoingCommandList;

	enet_uint16 reliableSequenceNumber;
	enet_uint16 unreliableSequenceNumber;

	enet_uint32 sentTime;

	enet_uint32 roundTripTimeout;
	enet_uint32 roundTripTimeoutLimit;
	enet_uint32 fragmentOffset;
	enet_uint32 fragmentLength;
	enet_uint32 sendAttempts;

	ENetProtocol command;
	ENetPacket* packet;
}ENetOutgoingCommand;

typedef struct _ENetIncomingCommand
{
	ENetListNode outgoingCommandList;

	enet_uint16 reliableSequenceNumber;
	enet_uint16 unreliableSequenceNumber;

	ENetProtocol command;

	enet_uint32 fragmentCount;
	enet_uint32 fragmentRemaining;
	enet_uint32* fragments;

	ENetPacket* packet;
}ENetIncomingCommand;

typedef enum
{
	ENET_PEER_STATE_DISCONNECTED 			 = 0,
	ENET_PEER_STATE_CONNECTING 				 = 1,
	ENET_PEER_STATE_ACKNOWLEDGING_CONNECT 	 = 2, 
	ENET_PEER_STATE_CONNECTION_PENDING		 = 3,
	ENET_PEER_STATE_CONNECTION_SUCCEEDED 	 = 4,
	ENET_PEER_STATE_CONNECTED 				 = 5,
	ENET_PEER_STATE_DISCONNECT_LATER 		 = 6,
	ENET_PEER_STATE_DISCONNECTING 			 = 7,
	ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT = 8,
	ENET_PEER_STATE_ZOMBIE                   = 9
}ENetPeerState;

#ifndef ENET_BUFFER_MAXIMUM
#define ENET_BUFFER_MAXIMUM (1 + 2 * ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS)
#endif

enum
{
	ENET_HOST_RECEIVE_BUFFER_SIZE = 256 * 1024,
	ENET_HOST_SEND_BUFFER_SIZE = 256 * 1024,

	ENET_PEER_DEFAULT_ROUND_TRIP_TIME = 500,
	ENET_PEER_WINDOW_SIZE_SCALE = 64 * 1024,
};

typedef struct _ENetChannel
{
	enet_uint16 outgoingReliableSequenceNumber;
	enet_uint16 incomingReliableSequenceNumber;

	ENetList incomingReliableCommands;
	ENetList incomingUnreliableCommands;
}ENetChannel;

struct _ENetHost;
typedef struct _ENetPeer
{
	struct _ENetHost* host;

	enet_uint16 outgoingPeerID;
	enet_uint16 incomingPeerID;

	enet_uint32 sessionID;

	ENetAddress address;
	void* data;

	ENetPeerState state;

	ENetChannel* channels;
	size_t channelCount;

	enet_uint32 incomingDataTotal;
	enet_uint32 outgoingDataTotal;

	enet_uint32 lastSendTime;
	enet_uint32 lastReceiveTime;

	enet_uint32 nextTimeout;
	enet_uint32 earliestTimeout;

	enet_uint32 packetLossEpoch;
	enet_uint32 packetSent;
	enet_uint32 packetLost;
	enet_uint32 packetLoss;
	enet_uint32 packetLossVariance;

	enet_uint32 lastRoundTripTime;
	enet_uint32 lowestRoundTripTime;
	enet_uint32 lastRoundTripTimeVariance;
	enet_uint32 highestRoundTripTimeVariance;
	enet_uint32 roundTripTime;
	enet_uint32 roundTripTimeVariance;

	enet_uint16 mtu;
	enet_uint32 windowSize;
	enet_uint32 reliableDataInTransit;
	enet_uint16 outgoingReliableSequenceNumber;

	ENetList acknowledgements;
	ENetList outgoingReliableCommands;
	ENetList outgoingUnreliableCommands;
	ENetList sentReliableCommands;
	ENetList sentUnreliableCommands;
}ENetPeer;

//逐步添加
typedef struct _ENetHost
{
	ENetSocket socket;
	ENetAddress address;

	enet_uint32 incomingBandwidth;
	enet_uint32 outgoingBandwidth;

	ENetPeer* peers;
	size_t peerCount;

	enet_uint32 serviceTime;

	ENetPeer* lastServicedPeer;
	//size_t packetSize;

	ENetProtocol commands[ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS];
	size_t commandCount;

	ENetBuffer buffers[ENET_BUFFER_MAXIMUM];
	size_t bufferCount;

	ENetAddress receivedAddress;
	//enet_uint8 receivedData[ENET_PROTOCOL_MAXIMUM_MTU];
}ENetHost;

typedef enum
{
	ENET_EVENT_TYPE_NONE       = 0,
	ENET_EVENT_TYPE_CONNECT    = 1,
	ENET_EVENT_TYPE_DISCONNECT = 2,
	ENET_EVENT_TYPE_RECEIVE    = 3
}ENetEventType;

typedef struct _ENetEvent
{
	ENetEventType type;
	ENetPeer* peer;
	enet_uint8 channelID;
	enet_uint32 data;
	ENetPacket* packet;
}ENetEvent;

//--------------------------------------------------------------
//enet sys
ENET_API int enet_initialize(void);
ENET_API int enet_deinitialize(void);

//unix time
ENET_API enet_uint32 enet_time_get(void);
ENET_API void enet_time_set(enet_uint32);

//unix socket
ENET_API ENetSocket enet_socket_create(ENetSocketType);
ENET_API int enet_socket_bind(ENetSocket, const ENetAddress*);
ENET_API int enet_socket_listen(ENetSocket, int);
ENET_API int enet_socket_send(ENetSocket, const ENetAddress*, const ENetBuffer*, size_t);
ENET_API int enet_socket_receive(ENetSocket, ENetAddress*, ENetBuffer*, size_t);
ENET_API int enet_socket_wait(ENetSocket, enet_uint32*, enet_uint32);
ENET_API int enet_socket_set_option(ENetSocket, ENetSocketOption, int);
ENET_API void enet_socket_destroy(ENetSocket);

//socket address
ENET_API int enet_address_set_host(ENetAddress* address, const char* hostName, enet_uint16 port);
ENET_API int enet_address_get_host_ip(const ENetAddress* address, char* name, size_t nameLength);

//packet interface
ENET_API ENetPacket* enet_packet_create(const void*, size_t, enet_uint32);
ENET_API void enet_packet_destroy(ENetPacket*);

//host interface
ENET_API ENetHost* enet_host_create(const ENetAddress*, size_t, enet_uint32, enet_uint32);
ENET_API void enet_host_destroy(ENetHost*);
ENET_API ENetPeer* enet_host_connect(ENetHost*, const ENetAddress*, size_t);
ENET_API int enet_host_service(ENetHost*, ENetEvent*, enet_uint32);

//peer interface
ENET_API int enet_peer_send(ENetPeer*, enet_uint8, ENetPacket*);
ENET_API ENetPacket* enet_peer_receive(ENetPeer*, enet_uint8);
ENET_API void enet_peer_reset(ENetPeer*);
extern void enet_peer_reset_queues(ENetPeer*);
extern ENetOutgoingCommand* enet_peer_queue_outgoing_command(ENetPeer*, const ENetProtocol*, ENetPacket*, enet_uint32, enet_uint16);

#ifdef __cplusplus 
}
#endif

#endif
