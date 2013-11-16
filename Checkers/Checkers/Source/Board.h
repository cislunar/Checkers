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

class Board : public Render2DObj
{
public:
	Board() : Render2DObj( )
	{
	}
	void		Render();
	void		Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes );
	void		Cleanup();
	glm::vec2	GetCellPos( int c );
	void		Update(float _dt );
	int			GetCell( glm::vec2 _screenPos);
	void		SetPos(glm::vec2 _pos);
	bool		CheckerOnCell( int _cell );
	void		SetupHighlights( int _selectedCell );

protected:
	glm::vec2 m_screenRes;
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
private:
};