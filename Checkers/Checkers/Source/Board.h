#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include "Checker.h"
#include <vector>

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
protected:
	glm::vec2 m_screenRes;
	std::vector<Checker> m_redCheckers;
	std::vector<Checker> m_blackCheckers;
	GLuint m_checkerPlain;
	GLuint m_checkerKing;
	float m_checkerSize;
private:
};