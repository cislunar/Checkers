
#include <stdlib.h>	// For random tint
#include "Math.h"
#include "Render.h"

void Log(const char *fmt, ...);


class Simulation
{
public:
	static Simulation* GetSimulation() {return &singleton;}

	void Init();
	void Shutdown();

	void SimulateOneFrame(float deltaTime);	// A single logic step
	void DrawFrame();						// Draw all the sprites
	void RotationWrap();					// Wrap rotation of sprites
	glm::mat4 GetViewMat();
	glm::mat4 GetProjMat();


private:
	Simulation();							// Force use of singleton

	static Simulation	singleton;
	static const int	MAX_SPRITES = 2;
	GLuint				sprites[MAX_SPRITES];
	glm::vec2			m_terrainRes;
};
