#include "CheckerMovePacket.h"

uint32_t CheckerMovePacket::GetNSetBits( uint16_t _n )
{
	// Clamp _n so that it doesn't have a value of more than 32
	uint16_t n = min(_n, 32);
	uint32_t retval = 0;
	for(uint16_t i = 0; i < n; ++i)
	{
		retval = retval | (1 << i);
	}
	return retval;
}

unsigned char	CheckerMovePacket::GetHorizBit( unsigned char _move )
{
	return _move & MOVE_HORIZ_DIR_BIT;
}

unsigned char	CheckerMovePacket::GetVertBit( unsigned char _move )
{
	return _move & MOVE_VERT_DIR_BIT;
}

void CheckerMovePacket::CheckerIndex( const unsigned char _idx )
{
	// Clear out the idx data
	m_moveData = m_moveData & (~CHECKER_IDX_BITS);
	// clamp the idx data, shift them into place, them or them into place
	m_moveData = m_moveData | ((uint32_t(_idx) & GetNSetBits(BITS_PER_CHECKER_IDX)) << CHECKER_IDX_SHIFT);
}

unsigned char CheckerMovePacket::CheckerIndex( void )
{
	// 0000---------------------------- : checker index
	unsigned char retval = ( m_moveData & CHECKER_IDX_BITS ) >> CHECKER_IDX_SHIFT;
	return retval;
}

void CheckerMovePacket::MoveCount( const unsigned char _mCnt )
{
	// clear out the moveCnt data
	m_moveData = m_moveData & (~MOVE_COUNT_BITS);
	// clamp the _mCnt, move them into place, then or into place
	m_moveData = m_moveData |  ((uint32_t(_mCnt) & GetNSetBits(BITS_PER_MOVE_COUNT)) << MOVE_COUNT_SHIFT);

}

unsigned char CheckerMovePacket::MoveCount( void )
{
	// ----0000------------------------ : move count
	unsigned char retval = ( m_moveData & MOVE_COUNT_BITS) >> MOVE_COUNT_SHIFT;
	return retval;
}

void CheckerMovePacket::Moves( const uint32_t _mData )
{
	// clamp the data
	uint32_t data = _mData & MOVE_BITS;
	// clear out moveData movesBits, then set them
	m_moveData = (m_moveData & (~MOVE_BITS)) | data;
}

uint32_t	CheckerMovePacket::Moves( void )
{
	uint32_t retval = 0;
	retval = (m_moveData & MOVE_BITS);
	return retval;
}

unsigned char CheckerMovePacket::GetMove( uint16_t _idx )
{
	unsigned char retval = -1;
	// We need to make sure that the user is requesting a move index value
	// that is in range
	if( _idx < MOVE_COUNT )
	{
		uint32_t moves = Moves();
		// We shift the m_moveData over by 2 for each index 
		// and only copy the least significant 2 bits
		retval = ( m_moveData >> (_idx * BITS_PER_MOVE) ) & (GetNSetBits(BITS_PER_MOVE));
	}
	return retval;
}

void CheckerMovePacket::Jump( const bool _jumpState )
{
	// Clear out jump state in move
	m_moveData = m_moveData & (~JUMP_BITS);
	// --------00---------------------- : moves: jumps/noJumps(1/0)
	uint8_t jumpState = _jumpState ? 1 : 0;
	m_moveData = m_moveData | (jumpState << JUMP_SHIFT );
}

bool CheckerMovePacket::Jump()
{
	// --------00---------------------- : moves: jumps/noJumps(1/0)
	bool retval = false;
	uint8_t jumpState = (m_moveData & JUMP_BITS) >> JUMP_SHIFT;
	if(jumpState > 0)
	{
		retval = true;
	}
	return retval;
}