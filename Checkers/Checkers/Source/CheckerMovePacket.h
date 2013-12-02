#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// MoveData bitpack layout:
// 32 bits:
// 00000000000000000000000000000000 : Composed 32 bits
// 0000---------------------------- : checker index 
// ----0000------------------------ : move count
// --------00---------------------- : move: left/right(1/0), up/down (1/0)
// --------000000000000000000000000 : moves

//  MOVEDATA BITPACK EXAMPLE:
// 01010100101111100000000000000000 : Composed 32 bits
// 0101---------------------------- : checker index 5
// ----0100------------------------ : 4 moves
// --------10111110---------------- : move downLeft, upLeft, upLeft, downLeft
// ----------------0000000000000000 : unused move space

// The reason we don't need a Checker type specifier (black or red)
// is that the moves happen consecutively, as well as the fact that
// the only moves a game will receive will be from the other connected player.
// Thus, we can assume that the data we receive is ALWAYS about the other player.

class CheckerMovePacket
{
public:
	CheckerMovePacket()
	{
		m_moveData = 0;
	}

	// Functions
	void			CheckerIndex( const unsigned char _idx );
	unsigned char	CheckerIndex( void );
	void			MoveCount( const unsigned char _mCnt );
	unsigned char	MoveCount( void );
	void			Moves( const uint32_t _mData );
	uint32_t		Moves( void );	

	// Variables
	uint32_t		m_moveData;

};