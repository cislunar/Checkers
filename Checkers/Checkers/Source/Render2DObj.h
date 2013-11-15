#pragma once
#include "Math.h"
#include "Render.h"

class Render2DObj
{
public:
	Render2DObj();
	virtual void		Render();
	virtual void		Setup( char* _imageFilePath, glm::vec2 _imageRes );
	virtual void		Cleanup();
	virtual void		SetPos(glm::vec2 _pos){ m_screenPos = _pos; }
	virtual glm::vec2	GetPos(){return m_screenPos;}
	virtual glm::vec2*	GetImageRes(){return &m_imageRes;}

protected:
	GLuint		m_image;
	glm::vec2	m_imageRes;
	glm::vec2	m_screenPos;
private:
};