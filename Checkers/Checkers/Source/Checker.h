#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"

class Checker : public Render2DObj
{
public:
	Checker() : Render2DObj( )
	{
	}
	void		Cleanup();
	glm::vec2*	GetPosPointer(){ return &m_screenPos; }
protected:
private:
};