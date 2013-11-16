
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
	glm::vec2 GetMousePos();
	void UpdatePrevKeys( SDLKey _key, bool _State );
	void UpdateKeys(SDLKey _key, bool _State );
	bool GetKey( SDLKey _key );
	bool GetOnKeyDown( SDLKey _key );
	bool GetCurMouseButton( int _button );
	bool GetPrevMouseButton( int _button );
	bool GetOnMouseButtonDown( int _button );

private:
	Simulation();							// Force use of singleton

	static Simulation	singleton;
	static const int	MAX_SPRITES = 2;
	GLuint				sprites[MAX_SPRITES];
	glm::vec2			m_terrainRes;
	glm::vec2			m_curMouse;
	glm::vec2			m_prevMouse;
	bool				m_KEYS[322]; // Number of SDLK_DOWN events
	bool				m_prevKEYS[322]; // Number of SDLK_DOWN events
	unsigned char		m_prevMouseButts;
	unsigned char		m_curMouseButts;
};
