#ifndef _PROTOCOL_H
#define  _PROTOCOL_H

enum {
	ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS = 32,
	ENET_PROTOCOL_MAXIMUM_PEER_ID = 0X7FFF,
	ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT = 1,
	ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT = 255,
	ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE = 32768,
	ENET_PROTOCOL_MINIMUM_WINDOW_SIZE = 4096
};

typedef enum {
	ENET_PROTOCOL_COMMAND_CONNECT = 2,
	ENET_PROTOCOL_COMMAND_COUNT = 12,
	ENET_PROTOCOL_COMMAND_MASK = 0x0f
} ENetProtocolCommand;

typedef enum {
	ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE = (1<<7)
} ENetProtocolFlag;

typedef struct _ENetProtocolCommandHeader {
	enet_uint8 command;
	enet_uint8 channelID;
	enet_uint16 reliableSequenceNumber;
} ENetProtocolCommandHeader;

typedef struct _ENetProtocolConnect {
	ENetProtocolCommandHeader header;
	enet_uint32 windowSize;
	enet_uint32 channelCount;
	enet_uint32 sessionID;
} ENetProtocolConnect;

typedef union {
	ENetProtocolCommandHeader header;
	ENetProtocolConnect connect;
} ENetProtocol;

#endif
