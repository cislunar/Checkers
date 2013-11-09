#include "RenderObjBase.h"

class Obj3D : public RenderObjBase
{
public:
	Obj3D() : RenderObjBase()
	{ 
		position = glm::vec4(0,0,0,1);
		m_scale = 1.0f;
	}
	virtual void Render(void);
	virtual glm::mat4 GetModelMat();

protected:
	virtual void CreateVBO(void);
	virtual void CreateShaders(void);
	virtual void DestroyVBO(void);
	virtual void DestroyShaders(void);
	void BindForRender();
	void UnbindForRender();
private:
	glm::vec4 position;
	float m_scale;
	GLuint
		ProjectionMatrixUniformLocation,
		ViewMatrixUniformLocation,
		ModelMatrixUniformLocation;
};