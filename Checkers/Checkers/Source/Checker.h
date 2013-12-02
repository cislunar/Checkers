#pragma once
#include "Math.h"
#include "Render.h"
#include "Render2DObj.h"
#include <vector>

class Checker : public Render2DObj
{
public:
	friend bool operator== ( const Checker &a, const Checker &b );
	friend bool operator!= ( const Checker &a, const Checker &b );

	enum CHECKER_TYPE
	{
		BLACK_CHECKER,
		RED_CHECKER,
		CHECKER_TYPE_CNT
	};
	Checker() : Render2DObj( )
	{
		m_isKinged = false;
	}
	void				Cleanup();
	glm::vec2*			GetPosPointer(){ return &m_screenPos; }
	CHECKER_TYPE		GetCheckerType()const { return m_type;}
	void				Init( CHECKER_TYPE _type );
	void				Move( glm::vec2 _newPos );
	bool				IsKinged();
	void				MakeKing( GLuint _kingImg );
	void				Deactivate( glm::vec2 _newPos );
	bool				Active()const {return m_isActive;}
protected:
	CHECKER_TYPE		m_type;
	bool				m_isKinged;
	bool				m_isActive;
private:
};