#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include <vector>

class Checker : public Render2DObj
{
public:
	enum CHECKER_MOVE_DIR
	{
		MOVE_UP,
		MOVE_DOWN,
		MOVE_KING,
		CHECKER_MOVE_DIR_COUNT
	};
	enum CHECKER_TYPE
	{
		BLACK_CHECKER,
		RED_CHECKER,
		CHECKER_TYPE_CNT
	};
	Checker() : Render2DObj( )
	{
	}
	void				Cleanup();
	glm::vec2*			GetPosPointer(){ return &m_screenPos; }
	CHECKER_MOVE_DIR	GetMoveDir(){ return m_moveDir; }
	CHECKER_TYPE		GetCheckerType(){ return m_type;}
	void				Init( CHECKER_TYPE _type );
protected:
	CHECKER_MOVE_DIR m_moveDir;
	CHECKER_TYPE		m_type;
private:
};