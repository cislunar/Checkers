#include "Checker.h"

void Checker::Render()
{
	DrawImage(m_image, Color(1,1,1), m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
}

void Checker::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}