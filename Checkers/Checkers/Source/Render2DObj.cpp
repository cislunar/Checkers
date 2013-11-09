#include "Render2DObj.h"

Render2DObj::Render2DObj()
{
}

void Render2DObj::Render()
{
	DrawImage(m_image, Color(1,1,1), m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
}

void Render2DObj::Setup( char* _imageFilePath )
{
	m_image = LoadImage( _imageFilePath );
}

void Render2DObj::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}