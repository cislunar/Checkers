#include "Checker.h"

void Checker::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}

void Checker::MakeKing( GLuint _kingImg )
{
	m_isKinged = true;
	m_image = _kingImg;
}

bool Checker::IsKinged()
{
	return m_isKinged;
}


void Checker::Init( CHECKER_TYPE _type )
{
	m_type = _type;
}

void Checker::Move( glm::vec2 _newPos )
{
	m_screenPos = _newPos;
}

