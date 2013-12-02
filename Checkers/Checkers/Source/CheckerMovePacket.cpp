#include "CheckerMovePacket.h"
#include <string.h>

void CheckerMovePacket::CheckerIndex( const unsigned char _idx )
{
	// Clear out the idx data
	m_moveData = m_moveData & 0x0fffffff;
	// Or the new data into place, but make sure to clear the MSBs (left 4 bits)
	m_moveData = m_moveData | ((uint32_t(_idx) & 0x0f) << 28);
}

unsigned char CheckerMovePacket::CheckerIndex( void )
{
	// 0000---------------------------- : checker index
	unsigned char retval = ( m_moveData & 0xf0000000 ) >> 28;
	return retval;
}

void CheckerMovePacket::MoveCount( const unsigned char _mCnt )
{
	// clear out the moveCnt data
	m_moveData = m_moveData & 0xf0ffffff;
	// Or the new data into place, but make sure to clear the MSBs (left 4 bits)
	m_moveData = m_moveData |  ((uint32_t(_mCnt) & 0x0f) << 24);

}

unsigned char CheckerMovePacket::MoveCount( void )
{
	// ----0000------------------------ : move count
	unsigned char retval = ( m_moveData & 0x0f000000) >> 24;
	return retval;
}

void CheckerMovePacket::Moves( const uint32_t _mData )
{
	uint32_t data = _mData & 0x00ffffff;
	m_moveData = (m_moveData & 0xff000000) | data;
}

uint32_t	CheckerMovePacket::Moves( void )
{
	uint32_t retval = 0;
	retval = (m_moveData & 0x00ffffff);
	return retval;
}