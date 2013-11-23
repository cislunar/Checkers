#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include "Checker.h"
#include <vector>
#include <algorithm>    // std::find
#include <string>

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
		m_prevMove;
		m_nextMove;
	};
	LegalMove( std::vector<LegalMove>::iterator _prevMove)
	{
		m_movedToCell = -1;
		m_moveType = REG_MOVE;
		m_prevMove = _prevMove;
	};

	int m_movedToCell;
	MOVE_TYPE m_moveType;
	std::vector<LegalMove>::iterator m_prevMove;
	std::vector<LegalMove>::iterator m_nextMove;
};

inline bool operator== (const LegalMove &lm1, const LegalMove &lm2)
{
	return (lm1.m_movedToCell == lm2.m_movedToCell 
		&& lm1.m_moveType == lm2.m_moveType 
		&& lm1.m_nextMove == lm2.m_nextMove 
		&& lm1.m_prevMove == lm2.m_prevMove );
}

inline bool operator!= (const LegalMove &lm1, const LegalMove &lm2)
{
	return !(lm1 == lm2);
}

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

	void								SetupHighlights( int _selectedCell, std::vector<LegalMove>* _possibleMoves  );
	bool								CheckerOnCell( int _cell );
	bool								CheckerOnCell( int _cell, Checker::CHECKER_TYPE _type );
	Checker*							GetCheckerOnCell( int _cell );
	int									GetCell( glm::vec2 _screenPos);
	glm::vec2							GetCellPos( int c );
	void								GetCheckerMoves( Checker* _c, int _cCell,  std::vector<LegalMove>::iterator _prevMove, std::vector<LegalMove>* _legalMoves  );
	void								GetPossibleMoves( Checker* _c, int _cCell, LegalMove* _possibleMoves, std::vector<LegalMove>::iterator _prevMove );
	LegalMove							GetMove( Checker* _movingChecker, std::vector<LegalMove>::iterator _prevMove, glm::vec2 moveDir, int _startCellNum, int desiredCellNum );
	void								HandleCellSelection(int _mousePosCell );
	void								ResetHighlights();
	bool								MoveIsUnique( std::vector<LegalMove>::iterator _prevMove, int _desiredCell );
	bool								CellIsMoveable( int _cellNum );
	bool								TryAddMove( std::vector<LegalMove>* _retMoves, LegalMove* _possibleMove );
	std::vector<LegalMove>::iterator	GetFinalMove( int _cell );
	void								UpdateAfterMove( std::vector<LegalMove>::iterator finalMove );
	std::vector<LegalMove>				GetVisibleMoves( std::vector<LegalMove>* _finalMoves );
	void								RemoveAffectedChecker( int _beginCell, int _endCell);


};