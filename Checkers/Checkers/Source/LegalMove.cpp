#include "LegalMove.h"

LegalMove::LegalMove()
{
	m_movedToCell = -1;
	m_moveType = REG_MOVE;
	m_prevMove = NULL;
	for(int i=0; i<4; ++i)
	{
		m_nextMoves[i] = NULL;
	}

};

void LegalMove::Reset()
{
	for(int i=0; i<4; ++i)
	{
		if(m_nextMoves[i] )
		{
			// Do we need to reset before deleting?
			if(m_nextMoves[i]->HasNextMove())
			{
				m_nextMoves[i]->Reset();
			}
			delete m_nextMoves[i];
			m_nextMoves[i] = NULL;
		}
	}
	m_prevMove = NULL;
}

bool LegalMove::HasNextMove()
{
	bool retval = false;
	for(int i=0; i<4; ++i)
	{
		if( m_nextMoves[i] )
		{
			retval = true;
			break;
		}
	}
	return retval;
}

bool LegalMove::MoveIsUnique( int _desiredCell )
{
	bool retval = true;
	retval = m_movedToCell != _desiredCell;
	if(retval == true)
	{
		for(int i=0; i<4; ++i)
		{
			if (m_nextMoves[i])
			{
				retval = m_nextMoves[i]->MoveIsUnique(_desiredCell);
				if(retval == false)
				{
					break;
				}
			}
		}
	}

	return retval;
}

LegalMove* LegalMove::AddNextMove( LegalMove* _nextMove )
{
	for(int i=0; i<4; ++i)
	{
		if( m_nextMoves[i] == NULL )
		{
			LegalMove* newMove = new LegalMove( *_nextMove );
			newMove->m_prevMove = this;
			m_nextMoves[i] = newMove;
			return m_nextMoves[i];
		}
		if(i == 3)
		{
			// This should never happen
			assert(false);
		}
	}
	return NULL;
}

std::vector<LegalMove> LegalMove::GetVisibleMoves( bool _hasJumps )
{
	std::vector<LegalMove> retval;
	// Only add terminating moves
	if(HasNextMove() == false)
	{
		// If the move list has jumps, we only add jumps to the move list
		if(_hasJumps == false
				&& m_moveType != START_MOVE
			|| _hasJumps == true
				&& m_moveType == JUMP_MOVE )
		{
			retval.push_back( *this );
		}
	}
	else
	{
		for(int i=0; i<4; ++i)
		{
			if( m_nextMoves[i] )
			{
				std::vector<LegalMove> moves =  m_nextMoves[i]->GetVisibleMoves( _hasJumps );
				retval.insert( retval.end(), moves.begin(), moves.end() );
			}
		}
	}
	return retval;
}

bool LegalMove::ContainsJump()
{
	bool retval = false;
	if(m_moveType == JUMP_MOVE)
	{
		retval = true;
	}
	else
	{
		for(int i=0; i<4; ++i)
		{
			if(m_nextMoves[i])
			{
				retval = m_nextMoves[i]->ContainsJump();
				if(retval)
				{
					break;
				}
			}
		}
	}
	return retval;
}

LegalMove*	LegalMove::GetMatchingMove( int _cell )
{
	LegalMove* retval = NULL;
	if( _cell == m_movedToCell)
	{
		retval = this;
	}
	else
	{
		for(int i=0; i<4; ++i)
		{
			if( m_nextMoves[i])
			{
				retval = m_nextMoves[i]->GetMatchingMove( _cell );
				if(retval)
				{
					break;
				}
			}
		}
	}
	return retval;
}
