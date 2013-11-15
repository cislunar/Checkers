#include "Render2DObj.h"

Render2DObj::Render2DObj()
{
	m_canRender = true;
}

void Render2DObj::Render()
{
	if(m_canRender)
	{
		DrawImage(m_image, m_color, m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
	}
}

void Render2DObj::Render_IgnoreBehind()
{
	if(m_canRender)
	{
		DrawImage(m_image, m_color, m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y, GL_SRC_ALPHA, GL_ZERO);
	}
}

void Render2DObj::Setup( char* _imageFilePath, glm::vec2 _imageRes )
{
	m_image = LoadImage( _imageFilePath );
	m_screenPos = glm::vec2(_imageRes.x/2, _imageRes.y/2);
	m_imageRes = _imageRes;
}

void Render2DObj::Setup( GLuint _img, glm::vec2 _imageRes )
{
	m_image = _img;
	m_screenPos = glm::vec2(_imageRes.x/2, _imageRes.y/2);
	m_imageRes = _imageRes;
}

void Render2DObj::Setup( char* _imageFilePath, glm::vec2 _imageRes, Color _color  )
{
	m_image = LoadImage( _imageFilePath );
	m_screenPos = glm::vec2(_imageRes.x/2, _imageRes.y/2);
	m_imageRes = _imageRes;
	m_color = _color;
}

void Render2DObj::Setup( GLuint _img, glm::vec2 _imageRes, Color _color  )
{
	m_image = _img;
	m_screenPos = glm::vec2(_imageRes.x/2, _imageRes.y/2);
	m_imageRes = _imageRes;
	m_color = _color;
}

void Render2DObj::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}