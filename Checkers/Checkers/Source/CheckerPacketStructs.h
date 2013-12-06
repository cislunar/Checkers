#pragma once

#include "CheckerMovePacket.h"
#include <stdint.h>

#define DEFAULT_BUFLEN		(1024)
#define PACKET_ENUM_BYES	(4)
#define CHAT_MSG_SIZE_BYTES	(2)
#define MAX_INPUT_LEN		(DEFAULT_BUFLEN - PACKET_ENUM_BYES - CHAT_MSG_SIZE_BYTES)

enum CHECKER_PACKET_TYPE
{
	CHECKER_PACKET_MOVE,
	CHECKER_PACKET_CHAT,
	CHECKER_PACKET_TYPE_COUNT
};

struct CheckerPacket_Move
{
	uint32_t packetType;
	CheckerMovePacket cmp;
};

struct CheckerPacket_Chat
{
	uint32_t packetType;
	uint16_t numChars;
	char m_msg[MAX_INPUT_LEN];
};