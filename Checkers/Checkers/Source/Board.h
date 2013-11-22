#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include "Checker.h"
#include <vector>

struct Rect
{
	glm::vec2 upperLeft;
	glm::vec2 botRight;
};

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
	LegalMove()
	{
		m_movedToCell = -1;
		m_moveType = REG_MOVE;
		m_prevMove = NULL;
		m_nextMove = NULL;
	};
	LegalMove( LegalMove* _prevMove)
	{
		m_movedToCell = -1;
		m_moveType = REG_MOVE;
		m_prevMove = _prevMove;
		if(m_prevMove)
		{
			m_prevMove->m_nextMove = this;
		}
	};

	int m_movedToCell;
	MOVE_TYPE m_moveType;
	LegalMove* m_prevMove;
	LegalMove* m_nextMove;
};

class Board : public Render2DObj
{
public:
	Board() : Render2DObj( )
	{
	}
	void		Render();
	void		Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes );
	void		Cleanup();
	void		Update(float _dt );
	void		SetPos(glm::vec2 _pos);

protected:
	glm::vec2					m_screenRes;
	std::vector<Checker>		m_redCheckers;
	std::vector<Checker>		m_blackCheckers;
	std::vector<Render2DObj>	m_cellHighLights;
	int							m_selectedCell;
	Render2DObj					m_mouseHighlight;
	GLuint						m_checkerPlain;
	GLuint						m_checkerKing;
	GLuint						m_cellHighlight;
	float						m_cellSize;
	Rect						m_boardRect;
	std::vector<LegalMove>		m_curPossibleMoves;

	void						SetupHighlights( int _selectedCell, std::vector<LegalMove>* _possibleMoves  );
	bool						CheckerOnCell( int _cell );
	bool						CheckerOnCell( int _cell, Checker::CHECKER_TYPE _type );
	Checker*					GetCheckerOnCell( int _cell );
	int							GetCell( glm::vec2 _screenPos);
	glm::vec2					GetCellPos( int c );
	void						GetCheckerMoves( Checker* _c, int _cCell,  LegalMove* _prevMove, std::vector<LegalMove>* _legalMoves  );
	void						GetPossibleMoves( Checker* _c, int _cCell, LegalMove* _possibleMoves, LegalMove* _prevMove );
	LegalMove					GetMove( Checker* _movingChecker, LegalMove* _prevMove, glm::vec2 moveDir, int _startCellNum, int desiredCellNum );
	void						HandleCellSelection(int _mousePosCell );
	void						ResetHighlights();
	bool						MoveIsUnique( LegalMove* _prevMove, int _desiredCell );
	bool						CellIsMoveable( int _cellNum );
	void						AddPossibleMove( std::vector<LegalMove>* _retMoves, LegalMove* _possibleMove );
	LegalMove*					GetFinalMove( int _cell );
	void						UpdateAfterMove( LegalMove* finalMove );
	std::vector<LegalMove>		GetVisibleMoves( std::vector<LegalMove>* _finalMoves );
	void						RemoveAffectedChecker( int _beginCell, int _endCell);


private:
};