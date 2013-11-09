#pragma once
#include "Render.h"

typedef struct
{
	float Position[4];
	float Color[4];
} Vertex;

class RenderObjBase
{
public:
	RenderObjBase(void){}
	virtual void Setup(void){ CreateShaders(); CreateVBO(); }
	virtual void Cleanup(void){ DestroyShaders(); DestroyVBO();}
	virtual void Render(void) = 0;

protected:
	virtual void CreateVBO(void)		= 0;
	virtual void CreateShaders(void)	= 0;
	virtual void DestroyVBO(void)		= 0;
	virtual void DestroyShaders(void)	= 0;

	ShaderInfo m_renderInfo;

private:
};