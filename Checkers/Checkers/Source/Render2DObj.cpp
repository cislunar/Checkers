#include "Render2DObj.h"

Render2DObj::Render2DObj()
{
}

void Render2DObj::Render()
{
	DrawImage(m_image, Color(1,1,1), m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
}

void Render2DObj::Setup( char* _imageFilePath, glm::vec2 _imageRes )
{
	m_image = LoadImage( _imageFilePath );
	m_screenPos = glm::vec2(_imageRes.x/2, _imageRes.y/2);
	m_imageRes = _imageRes;
}

void Render2DObj::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}