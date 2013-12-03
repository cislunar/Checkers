#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <minmax.h>

#define MOVE_COUNT				11
#define BITS_PER_MOVE			2
#define BITS_PER_JUMP_STATE		2
#define BITS_PER_MOVE_COUNT		4
#define BITS_PER_CHECKER_IDX	4
#define JUMP_SHIFT				(MOVE_COUNT * BITS_PER_MOVE)
#define MOVE_COUNT_SHIFT		(JUMP_SHIFT + BITS_PER_JUMP_STATE)
#define CHECKER_IDX_SHIFT		(MOVE_COUNT_SHIFT + BITS_PER_MOVE_COUNT)
#define JUMP_BITS				(0x00C00000)
#define MOVE_COUNT_BITS			(0x0F000000)
#define CHECKER_IDX_BITS		(0xF0000000)
#define MOVE_BITS				(0x003FFFFF)
#define MOVE_HORIZ_DIR_BIT		0x2
#define MOVE_VERT_DIR_BIT		0x1


// MoveData bitpack layout:
// 32 bits:
// 00000000000000000000000000000000 : Composed 32 bits
// 0000---------------------------- : checker index 
// ----0000------------------------ : move count
// --------00---------------------- : moves: jumps/noJumps(1/0)
// ----------00-------------------- : move: left/right(1/0), up/down (1/0)
// ----------0000000000000000000000 : moves

//  MOVEDATA BITPACK EXAMPLE:
// 01010100101111100000000000000000 : Composed 32 bits
// 0101---------------------------- : checker index 5
// ----0100------------------------ : 4 moves
// --------01---------------------- : moves: moves are jumps
// ----------111110---------------- : move  upLeft, upLeft, downLeft
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
	void					CheckerIndex( const unsigned char _idx );
	unsigned char			CheckerIndex( void );
	void					MoveCount( const unsigned char _mCnt );
	unsigned char			MoveCount( void );
	void					Moves( const uint32_t _mData );
	uint32_t				Moves( void );	
	void					Jump( const bool _jumpState );
	bool					Jump();
	unsigned char			GetMove( uint16_t _idx );
	uint32_t				GetNSetBits( uint16_t _n );
	static unsigned char	GetHorizBit( unsigned char _move );
	static unsigned char	GetVertBit( unsigned char _move );

	// Variables
	uint32_t		m_moveData;

};