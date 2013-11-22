#include "Checker.h"

void Checker::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}
}

void Checker::Init( CHECKER_TYPE _type )
{
	m_type = _type;
	if(m_type == BLACK_CHECKER)
	{
		m_moveDir = MOVE_UP;
	}
	else
	{
		m_moveDir = MOVE_DOWN;
	}
}

void Checker::Move( glm::vec2 _newPos )
{
	m_screenPos = _newPos;
}

