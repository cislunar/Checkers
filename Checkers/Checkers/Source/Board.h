#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"

class Board : public Render2DObj
{
public:
	Board() : Render2DObj( )
	{
	}
	void		Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes );
protected:
	glm::vec2 m_screenRes;
private:
};