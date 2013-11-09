#pragma once
#include "RenderObjBase.h"

class RenderObj : public RenderObjBase
{
public:
	RenderObj() : RenderObjBase(){}
	virtual void Render(void);
protected:
	virtual void CreateVBO(void);
	virtual void CreateShaders(void);
	virtual void DestroyVBO(void);
	virtual void DestroyShaders(void);

private:
};