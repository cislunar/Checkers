#include "Board.h"

void Board::Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes )
{
	Render2DObj::Setup(_imageFilePath, _imageRes);
	m_screenPos = glm::vec2(_screenRes.x/2, _screenRes.y/2);
	m_screenRes = _screenRes;
}
