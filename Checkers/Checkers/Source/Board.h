#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include "Checker.h"
#include <vector>
#include "LegalMove.h"
#include <algorithm>

struct Rect
{
	glm::vec2 upperLeft;
	glm::vec2 botRight;
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
	bool		GameIsOver();
	bool		HasPiecesLeft( Checker::CHECKER_TYPE _ct );
	void		SetPlayerType(Checker::CHECKER_TYPE _type);
	void		SetUsersTurn( bool _state );

private:
	// Board information
	glm::vec2					m_screenRes;
	std::vector<Checker>		m_redCheckers;
	std::vector<Checker>		m_blackCheckers;
	std::vector<Render2DObj>	m_cellHighLights;
	Render2DObj					m_mouseHighlight;
	// Textures
	GLuint						m_checkerPlain,
								m_checkerKing,
								m_cellHighlight;
	// Misc
	float						m_cellSize;
	Rect						m_boardRect;
	LegalMove					m_movesRoot;
	std::vector<LegalMove>		m_visibleMoves;
	int							m_selectedCell;
	Checker::CHECKER_TYPE		m_playerType;
	bool						m_isPlayersTurn;

	// Functions
	void						SetupHighlights( int _selectedCell, LegalMove* _rootMove  );
	bool						CheckerOnCell( int _cell );
	bool						CheckerOnCell( int _cell, Checker::CHECKER_TYPE _type );
	Checker*					GetCheckerOnCell( int _cell );
	int							GetCell( glm::vec2 _screenPos);
	glm::vec2					GetCellPos( int c );
	void						GetCheckerMoves( Checker* _c, int _cCell,  LegalMove* _prevMove );
	void						GetPossibleMoves( Checker* _c, int _cCell, LegalMove* _possibleMoves, LegalMove* const _prevMove );
	LegalMove					GetMove( Checker* _movingChecker, LegalMove* const _prevMove, glm::vec2 moveDir, int _startCellNum, int desiredCellNum );
	void						HandleCellSelection(int _mousePosCell );
	void						ResetHighlights();
	bool						CanMoveToCell( int _cellNum );
	void						HandleFinalMove( LegalMove* finalMove, Checker* _c );
	void						RemoveAffectedChecker( int _beginCell, int _endCell);
	bool						IsKingMove( int _cell );
	void						DecomposeFinalMove( LegalMove* finalMove, Checker* _c );
	uint32_t					DecomposeFinalMove_GetMoves( LegalMove* _finalMove, int _moveCnt );
	int							HorizMoveType(  int _start, int _end );
	int							VertMoveType(  int _start, int _end );
};