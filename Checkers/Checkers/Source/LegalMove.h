#pragma once
#include "Math.h"
#include <vector>
#include "Checker.h"
#include "Render.h"

class LegalMove
{
public:
	enum MOVE_TYPE
	{
		JUMP_MOVE,
		REG_MOVE,
		START_MOVE,
		MOVE_TYPE_COUNT
	};

	LegalMove();
	void					Reset();
	bool					HasNextMove();
	bool					MoveIsUnique( int _desiredCell );
	LegalMove*				AddNextMove( LegalMove* _nextMove );
	bool					ContainsJump();
	std::vector<LegalMove>	GetVisibleMoves(  bool _hasJumps  );
	LegalMove*				GetMatchingMove( int _cell );


	int m_movedToCell;
	MOVE_TYPE m_moveType;
	LegalMove* m_prevMove;
private:
	LegalMove* m_nextMoves[4];
};