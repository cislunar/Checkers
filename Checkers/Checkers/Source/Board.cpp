#include "Board.h"
#include <assert.h>
#include "Simulation.h"

static Simulation* m_sim = NULL;
void Board::Update(float _dt )
{
	if(m_isPlayersTurn)
	{
		// Update highlight 
		int mouseCellPos = GetCell( m_sim->GetMousePos() );
		HandleCellSelection( mouseCellPos );
	}
}

void Board::HandleCellSelection(int _mousePosCell )
{
	if( CheckerOnCell(_mousePosCell, m_playerType ) )
	{
		m_mouseHighlight.SetPos( GetCellPos(  _mousePosCell) );
		m_mouseHighlight.SetRenderState(true);

		if( m_sim->GetOnMouseButtonDown( SDL_BUTTON_LEFT ) )
		{
			// If player is clicking on a cell that hasn't been clicked
			// we highlight it and check for possible moves and highlight those.
			if( _mousePosCell != m_selectedCell )
			{
				m_selectedCell = _mousePosCell;	
				m_movesRoot.Reset();
				Checker* c = GetCheckerOnCell( m_selectedCell );

				LegalMove startMove;
				startMove.m_movedToCell = m_selectedCell;
				startMove.m_moveType = LegalMove::START_MOVE;
				m_movesRoot = startMove;

				GetCheckerMoves(c, m_selectedCell, &m_movesRoot );
				SetupHighlights( _mousePosCell, &m_movesRoot );
			}
			else
			{
				m_selectedCell = -1;
				m_movesRoot.Reset();
				ResetHighlights();
			}
		}
	}
	else
	{
		if( _mousePosCell != m_selectedCell
			&& m_sim->GetOnMouseButtonDown( SDL_BUTTON_LEFT ) )
		{
			if(CanMoveToCell( _mousePosCell ) )
			{
				// Move checker
				Checker* selected = GetCheckerOnCell( m_selectedCell );
				LegalMove* finalMove = m_movesRoot.GetMatchingMove( _mousePosCell );
				HandleFinalMove( finalMove, selected);
				selected->Move( GetCellPos( _mousePosCell) );
				ResetHighlights();
				// Decompose final move into CheckerMovePacket
				DecomposeFinalMove( finalMove, selected );
				m_movesRoot.Reset();
				m_visibleMoves.clear();
				m_selectedCell = -1;
			}
		}
		m_mouseHighlight.SetRenderState(false);
	}
}

void Board::RemoveAffectedChecker( int _beginCell, int _endCell )
{
	int maxCell = max(_beginCell, _endCell);
	int minCell = min(_beginCell, _endCell);
	int cellDiff = maxCell - minCell;
	// Diff between cells will be:
	// 14 (bot left)
	// 18 (bot right)
	int affectedCell = -1;
	switch( cellDiff )
	{
	case 14:
		affectedCell = minCell + 7;
		break;
	case 18:
		affectedCell = minCell + 9;
		break;
	default:
		// We should never ever have this happen.
		assert(false);
	}

	Checker* affected = GetCheckerOnCell(affectedCell);
	affected->Deactivate( GetCellPos(-1) );
}

void Board::DecomposeFinalMove( LegalMove* finalMove, Checker* _c )
{
	CheckerMovePacket cmp;
	unsigned char	idx = -1;
	unsigned char	moveCnt = 0;
	uint32_t		moves;

	// Get idx
	for(int i = 0; i<12; ++i)
	{
		switch (m_playerType)
		{
		case Checker::BLACK_CHECKER:
			if(m_blackCheckers[i] == *_c)
			{
				idx = i;
			}
			break;
		case Checker::RED_CHECKER:
			if(m_redCheckers[i] == *_c)
			{
				idx = i;
			}
			break;
		default:
		case Checker::CHECKER_TYPE_CNT:
			break;
		}
	}

	// Get move count
	LegalMove* ptr = finalMove;
	while( ptr->m_prevMove != NULL )
	{
		moveCnt++;
		ptr = ptr->m_prevMove;
	}

	// Get moves
	moves = DecomposeFinalMove_GetMoves( finalMove, moveCnt );

	// Get jumpState
	bool jumpState = finalMove->ContainsJump();

	// Compose cmp
	cmp.CheckerIndex( idx );
	cmp.MoveCount( moveCnt );
	cmp.Moves( moves );
	cmp.Jump( jumpState );

	// Notify Simulation
	m_sim->ReceiveCheckerMovePacket(cmp);
}

uint8_t	Board::HorizMoveType( int _start, int _end )
{
	// move: left/right(1/0)
	int sMod = _start % 8;
	int eMod = _end % 8;
	// Cells numbers are 2D array.
	// They increase going to the right and down.
	// Cells on the left have smaller numbers than cells to the right.
	uint8_t retval = eMod < sMod ? 1 : 0;
	return retval;
}

uint8_t	Board::VertMoveType(  int _start, int _end )
{
	// up/down (1/0)
	// Cells numbers are 2D array.
	// They increase going to the right and down.
	// Cells on the above have lower values than cells above
	uint8_t retval = _end < _start ? 1 : 0;
	return retval;
}

int Board::GetCellFromDir( bool _jump, int _start, int _horiz, int _vert )
{
	// move: up/down (1/0)
	// move: left/right(1/0)

	int retval = -1;
	if( _start >= 0
		&& _start < (8*8) )
	{
		//retval = _horiz == 0 : 
		int offset = 0;
		if( _horiz == 0 )
		{
			offset = 9;
		}
		else
		{
			offset = 7;
		}

		if( _vert == 1 )
		{
			// flip cell to the row above _start's cell
			offset -= 16;
		}

		if( _jump )
		{
			// If we have a jump, we multiply by 2 to get the offset at
			// start's row +/- 2 rows
			offset *= 2;
		}
		retval = _start + offset;
	}
	return retval;
}

void Board::HandleOtherPlayerMoves( CheckerMovePacket _cmp )
{
	// Get the vector of the other checker type
	// So we can handle the moves that were made.
	std::vector<Checker>	*othercheckers	= NULL;
	othercheckers = m_playerType == Checker::RED_CHECKER ? &m_blackCheckers : &m_redCheckers;

	Checker* checker = &(*othercheckers)[_cmp.CheckerIndex()];
	// Get the moves that were made so we can run through them.
	// Get total count of moves made so we can run through them.
	unsigned char movesCnt = _cmp.MoveCount();
	if( movesCnt > 0 )
	{
		//####################################################################################
		// Handle jump and regular moves---------------------------------------------------
		unsigned char i = 0;
		// We store the cell the checker starts on because it is not included in the moves data.
		int startCellNum = 0;
		startCellNum = GetCell( checker->GetPos() );
		unsigned char move =  _cmp.GetMove( i );
		int horiz = CheckerMovePacket::GetHorizBit( move );
		int vert = CheckerMovePacket::GetVertBit( move );
		int endCellNum = 0;
		endCellNum = GetCellFromDir( _cmp.Jump(), startCellNum, horiz, vert );
		do 
		{
			// If this is a jump move, remove the checker we jumped
			if( _cmp.Jump() )
			{
				RemoveAffectedChecker(startCellNum, endCellNum );
			}
			++i;
			// Safety conditional so we don't accidentally access data we don't have
			if( i < movesCnt)
			{
				startCellNum = endCellNum;
				move =  _cmp.GetMove( i );
				horiz = CheckerMovePacket::GetHorizBit( move );
				vert = CheckerMovePacket::GetVertBit( move );
				endCellNum = GetCellFromDir( _cmp.Jump(), startCellNum, horiz, vert );
			}
			else
			{
				//####################################################################################
				// Handle KingMaker move-------------------------------------------------------------
				// King maker move will ALWAYS be the last move a piece makes
				if( checker->IsKinged() == false
					&& IsKingMove( endCellNum ) )
				{
					checker->MakeKing( m_checkerKing_texHandle );
				}

				//####################################################################################
				// Move checker to final spot
				checker->Move( GetCellPos( endCellNum ) );
			}
		} while ( i < movesCnt);
	}
}

uint32_t Board::DecomposeFinalMove_GetMoves( LegalMove* _finalMove, int _moveCnt )
{
	LegalMove* move = _finalMove;
	uint32_t retval = 0;
	for(int i=_moveCnt-1; i >= 0; --i)
	{
		// Determine what dirs of the move
		uint32_t horizMove = HorizMoveType( move->m_prevMove->m_movedToCell, move->m_movedToCell );
		uint32_t vertMove = VertMoveType( move->m_prevMove->m_movedToCell, move->m_movedToCell );
		// Each moves consists of 2 bits. Multiply by 2 to get the idx.
		int idx = i * BITS_PER_MOVE;
		// Store the up/down dir (1/0)
		retval = retval | ( vertMove << idx );
		// Store the leftRight dir (1/0)
		++idx;
		retval = retval | ( horizMove << idx );
		move = move->m_prevMove;
	}
	return retval;
}

void Board::HandleFinalMove( LegalMove* finalMove, Checker* _c )
{
	if( finalMove->m_moveType == LegalMove::JUMP_MOVE )
	{
		LegalMove* move = finalMove;
		while( move->m_prevMove != NULL)
		{
			RemoveAffectedChecker(move->m_movedToCell, move->m_prevMove->m_movedToCell  );
			move = move->m_prevMove;
		}
	}

	if( _c->IsKinged() == false
		&& IsKingMove(finalMove->m_movedToCell) )
	{
		_c->MakeKing( m_checkerKing_texHandle );
	}
}

bool Board::CanMoveToCell( int _cellNum )
{
	bool retval = false;

	std::vector<LegalMove>::iterator first;
	for( first=m_visibleMoves.begin(); first != m_visibleMoves.end(); ++first)
	{
		if( first->m_movedToCell == _cellNum )
		{
			retval = true;
			break;
		}
	}
	return retval;
}

void Board::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_checkerPlain);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_checkerKing_texHandle);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_cellHighlight);
	}
}

void Board::ResetHighlights()
{
	for(uint32_t i=0; i<12; ++i)
	{
		m_cellHighLights[i].SetPos( GetCellPos( -1 ) );
		m_cellHighLights[i].SetRenderState( false );

	}
}

void Board::SetupHighlights( int _selectedCell, LegalMove* _rootMove  )
{
	ResetHighlights();
	m_cellHighLights[0].SetPos( GetCellPos( _selectedCell ) );
	m_cellHighLights[0].SetRenderState( true );

	m_visibleMoves = _rootMove->GetVisibleMoves( _rootMove->ContainsJump() );
	std::vector<LegalMove>::iterator iter;
	int i=0;
	for( iter=m_visibleMoves.begin(); iter != m_visibleMoves.end(); ++iter, ++i)
	{
		m_cellHighLights[i+1].SetPos( GetCellPos( iter->m_movedToCell ) );
		m_cellHighLights[i+1].SetRenderState( true );
	}
}

void Board::Render()
{
	if(m_canRender)
	{
		// Draw board
		DrawImage(m_image, Color(1,1,1,1), m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
		
		// Drawhighlight of cell that has mouse and checker on it
		m_mouseHighlight.Render_IgnoreBehind();

		// Draw checkers and highlights
		for(int i=0; i<12; ++i)
		{
			m_cellHighLights[i].Render_IgnoreBehind();
			m_redCheckers[i].Render();
			m_blackCheckers[i].Render();
		}
	}
}

bool Board::CheckerOnCell( int _cell, Checker::CHECKER_TYPE _type )
{
	if( _cell >= 0 && _cell < 8 * 8)
	{
		int cell = -1;
		std::vector<Checker>* checkerVec = NULL;
		switch( _type )
		{
		case Checker::BLACK_CHECKER:
			checkerVec = &m_blackCheckers;
			break;
		case Checker::RED_CHECKER:
			checkerVec = &m_redCheckers;
			break;
		default:
		case Checker::CHECKER_TYPE_CNT:
				break;
		}

		for(int i=0; i<12; ++i)
		{
			cell = GetCell( (*checkerVec)[i].GetPos() );
			if( cell != -1 && cell == _cell)
			{
				return true;
			}
		}
	}
	return false;
}

bool Board::CheckerOnCell( int _cell )
{
	if( _cell >= 0 && _cell < 8 * 8)
	{
		int cell = -1;
		for(int i=0; i<12; ++i)
		{
			cell = GetCell( m_blackCheckers[i].GetPos() );
			if( cell != -1 && cell == _cell)
			{
				return true;
			}
			cell = GetCell( m_redCheckers[i].GetPos() );
			if( cell != -1 && cell == _cell)
			{
				return true;
			}
		}
	}
	return false;
}


int	Board::GetCell( glm::vec2 _screenPos)
{
	int retval = -1;
	if(_screenPos.x >= m_boardRect.upperLeft.x
		&& _screenPos.x <= m_boardRect.botRight.x
		&& _screenPos.y >= m_boardRect.upperLeft.y
		&& _screenPos.y <= m_boardRect.botRight.y)
	{
		//Within AABB, get cell pos
		glm::vec2 modifiedBoardPos = m_screenPos - m_boardRect.upperLeft;
		glm::vec2 modified_ScreenPos = _screenPos - m_boardRect.upperLeft;
		int x = modified_ScreenPos.x / m_cellSize;
		int y = modified_ScreenPos.y / m_cellSize;
		retval = y * 8 + x;
	}
	return retval;
}

void Board::SetPos(glm::vec2 _pos)
{
	Render2DObj::SetPos(_pos);
	m_boardRect.botRight = _pos + (m_imageRes / 2.f);
	m_boardRect.upperLeft = _pos - (m_imageRes / 2.f);
}

void Board::Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes )
{
	Render2DObj::Setup(_imageFilePath, _imageRes);
	SetPos(glm::vec2(_screenRes.x/2, _screenRes.y/2));
	m_screenRes = _screenRes;

	m_cellHighlight = LoadImage("cellHighlight.png");
	m_checkerPlain = LoadImage("checkerPlain.png");
	m_checkerKing_texHandle = LoadImage("checkerKing.png");

	m_mouseHighlight.Setup(m_cellHighlight, m_imageRes / 8.f, Color(1.f,.7f, 0,1));
	m_mouseHighlight.SetPos( GetCellPos( -1 ) );
	m_mouseHighlight.SetRenderState(false);

	Checker obj;
	Render2DObj highlight;
	glm::vec2 startPos = m_screenPos;
	m_cellSize = m_imageRes.x / 8.f;
	startPos -= glm::vec2(3.5f, 3.5f) * m_cellSize;
	int cellPos = -1;
	for(int i=0; i<12; ++i)
	{
		// Offsets checker by 2 horizontally but offset resets after 4 in a row (for new row)
		obj.Setup(m_checkerPlain, m_imageRes / 8.f, Color(1,0,0,1) );
		obj.Init( Checker::RED_CHECKER );
		// checkers begin spaced out by 2 in their rows.
		// The middle row is the same but has an offset of 1
		cellPos = i * 2 + (i >= 4 && i < 8 ? 1 : 0);
		obj.SetPos( GetCellPos( cellPos ) );
		m_redCheckers.push_back(obj);

		obj.Setup(m_checkerPlain, m_imageRes / 8.f, Color(0.12f,0.12f,0.12f,1) );
		obj.Init( Checker::BLACK_CHECKER );
		// Follows same rules as above cell calculation
		// but in reverse because these checkers are in mirror position
		cellPos = i * -2 + (i >= 4 && i < 8 ? -1 : 0) + 63;
		obj.SetPos( GetCellPos(cellPos) );
		m_blackCheckers.push_back(obj);

		// Setup Highlights
		highlight.Setup(m_cellHighlight, m_imageRes / 8.f, Color(0,0, 1,.7f));
		highlight.SetPos( GetCellPos( -1 ) );
		highlight.SetRenderState(false);
		m_cellHighLights.push_back(highlight);
	}

	m_sim = Simulation::GetSimulation();
}

glm::vec2 Board::GetCellPos( int c )
{
	assert( c >= -1 && c < 64);
	if( c == -1)
	{
		// give bogus position so that the object is off screen
		return glm::vec2(-m_screenPos);
	}
	// Get beginning offset
	glm::vec2 retval =	m_screenPos 
						- glm::vec2(m_imageRes.x/2.f, m_imageRes.y/2.f) 
						+ (glm::vec2(1,1) * (m_cellSize/2.f) );
	retval += glm::vec2(c % 8, floor(c/8.f)) * m_cellSize;
	return retval;
}

Checker* Board::GetCheckerOnCell( int _cell )
{
	Checker* retval = NULL;
	int cell = -1;
	for(int i=0; i<12; ++i)
	{
		cell = GetCell( m_redCheckers[i].GetPos() );
		if( cell != -1 && cell == _cell)
		{
			retval = &m_redCheckers[i];
			break;
		}
		cell = GetCell( m_blackCheckers[i].GetPos() );
		if( cell != -1 && cell == _cell)
		{
			retval = &m_blackCheckers[i];
			break;
		}
	}
	return retval;
}

bool Board::IsKingMove( int _cell )
{
	bool retval = (_cell >= 0 && _cell <= 7)
					|| (_cell >= 56 &&_cell <= 63);
	return retval;
}

LegalMove Board::GetMove( Checker* _movingChecker, LegalMove* const _prevMove, glm::vec2 moveDir, int _startCellNum, int desiredCellNum )
{
	LegalMove retVal;
	if( m_movesRoot.MoveIsUnique( desiredCellNum ) )
	{
		// We cannot land on a cell that is occupied
		if(CheckerOnCell(desiredCellNum) )
		{
			// if the other checker is an opponent checker we can jump it
			if(CheckerOnCell( desiredCellNum, _movingChecker->GetCheckerType()) == false)
			{
				// If we can jump over it
				int jumpCell = GetCell( GetCellPos(_startCellNum) + glm::vec2(moveDir.x * 2, moveDir.y * 2));
				if( jumpCell != -1
					&& CheckerOnCell(jumpCell) == false
					&& m_movesRoot.MoveIsUnique(jumpCell ) )
				{
					retVal.m_prevMove = _prevMove;
					retVal.m_movedToCell = jumpCell;
					retVal.m_moveType = LegalMove::JUMP_MOVE;
				}
			}
		}
		// We can land on this cell
		// If we haven't previously jumped.
		// Once a checker jumps, the only moves it can continue to make
		// are jump moves.
		else if (_prevMove == NULL
					|| _prevMove->m_moveType != LegalMove::JUMP_MOVE)
		{
			retVal.m_movedToCell = desiredCellNum;
			if(IsKingMove( retVal.m_movedToCell ) )
			retVal.m_moveType = LegalMove::REG_MOVE;
		}
	}
	return retVal;
}

void Board::GetPossibleMoves( Checker* _c, int _cCell, LegalMove* _possibleMoves,  LegalMove* const _prevMove )
{
	_possibleMoves[0] = _possibleMoves[1] = _possibleMoves[2] = _possibleMoves[3] = LegalMove();
	glm::vec2 cellPos = GetCellPos( _cCell );

	if(	_c->GetCheckerType() == Checker::BLACK_CHECKER
		|| _c->IsKinged() )
	{
		// up left
		int desiredCell = GetCell( cellPos + glm::vec2(-m_cellSize, -m_cellSize));
		_possibleMoves[0] = GetMove(_c, 
									_prevMove, 
									glm::vec2(-m_cellSize, -m_cellSize), 
									_cCell, 
									desiredCell);
		
		// up right
		desiredCell = GetCell( cellPos + glm::vec2(m_cellSize, -m_cellSize));

		_possibleMoves[1] = GetMove(_c, 
									_prevMove, 
									glm::vec2(m_cellSize, -m_cellSize), 
									_cCell, 
									desiredCell);
	}

	if( _c->GetCheckerType() == Checker::RED_CHECKER
		|| _c->IsKinged() )
	{
		// down left
		int desiredCell = GetCell( cellPos + glm::vec2(-m_cellSize, m_cellSize));
		_possibleMoves[2] = GetMove(_c, 
									_prevMove, 
									glm::vec2(-m_cellSize, m_cellSize), 
									_cCell, 
									desiredCell);

		// down right
		desiredCell = GetCell( cellPos + glm::vec2(m_cellSize, m_cellSize));
		_possibleMoves[3] = GetMove(_c, 
									_prevMove, 
									glm::vec2(m_cellSize, m_cellSize), 
									_cCell, 
									desiredCell);
	}
}

void Board::GetCheckerMoves( Checker* _c, int _cCell, LegalMove* _prevMove )
{
	LegalMove* possibleMoves = new LegalMove[4];
	GetPossibleMoves( _c, _cCell, possibleMoves, _prevMove);

	for(int i=0; i<4; ++i)
	{
		if(possibleMoves[i].m_movedToCell != -1)
		{
			LegalMove* newMove = _prevMove->AddNextMove( &possibleMoves[i] );
			if( possibleMoves[i].m_moveType == LegalMove::JUMP_MOVE )
			{
				GetCheckerMoves( _c, newMove->m_movedToCell, newMove);
			}
		}
	}
	
	delete [] possibleMoves;
}

bool Board::GameIsOver()
{
	return HasPiecesLeft( Checker::BLACK_CHECKER) == false || HasPiecesLeft(Checker::RED_CHECKER) == false;
}


bool Board::HasPiecesLeft( Checker::CHECKER_TYPE _ct )
{
	std::vector<Checker>* checkers;
	if( _ct == Checker::BLACK_CHECKER)
	{
		checkers = &m_blackCheckers;
	}
	else
	{
		checkers = &m_redCheckers;
	}
	std::vector<Checker>::iterator it;
	int count = 0;
	for( it=checkers->begin(); it != checkers->end(); ++it)
	{
		if(it->Active())
		{
			return true;
		}
	}
	return false;
}

void Board::SetPlayerType( Checker::CHECKER_TYPE _type )
{
	m_playerType = _type;
}

void Board::SetPlayersTurnState( bool _state )
{
	m_isPlayersTurn = _state;
}
