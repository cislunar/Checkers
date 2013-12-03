#pragma once

#include "CheckerMovePacket.h"
#include <stdint.h>

enum CHECKER_PACKET_TYPE
{
	CHECKER_PACKET_MOVE,
	CHECKER_PACKET_TYPE_COUNT
};

struct CheckerPacket_Move
{
	uint32_t packetType;
	CheckerMovePacket cmp;
};