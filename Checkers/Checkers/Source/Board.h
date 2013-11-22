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
		MOVE_TYPE_COUNT
	};
	LegalMove()
	{
		m_movedToCell = -1;
		m_moveType = REG_MOVE;
	};

	int m_movedToCell;
	MOVE_TYPE m_moveType;
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

	void						SetupHighlights( int _selectedCell, std::vector<int>* _possibleMoves  );
	bool						CheckerOnCell( int _cell );
	bool						CheckerOnCell( int _cell, Checker::CHECKER_TYPE _type );
	Checker*					GetCheckerOnCell( int _cell );
	int							GetCell( glm::vec2 _screenPos);
	glm::vec2					GetCellPos( int c );
	std::vector<int>			GetCheckerMoves( Checker* _c, int _cCell );
	void						GetPossibleMoves( Checker* _c, int _cCell, LegalMove* _possibleMoves );
	LegalMove					GetMove( Checker* _movingChecker, glm::vec2 moveDir, int _startCellNum, int desiredCellNum );
	void						HandleCellSelection(int _mousePosCell );
	void						ResetHighlights();


private:
};