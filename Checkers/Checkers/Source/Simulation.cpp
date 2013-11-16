#include <sdl.h>
#include <math.h>
#include "Main.h"
#include "Simulation.h"
#include "RenderObj.h"
#include "RenderObjTest.h"
#include "Obj3D.h"
#include "Render2DObj.h"
#include "Board.h"
#include "Checker.h"

static bool			enableGravity = true;
static glm::vec2	grav(0, 175);
static bool			enableRotation = true;
static float		imgRot = 0;

static bool			drawLines = false;

Simulation Simulation::singleton;

static RenderObjTest	renderObjTest;
static Obj3D			obj3d;
Board					board;

static float			camDist = -4.f;
static float			camHeight = 0.f;

Simulation::Simulation()
{
}


void Simulation::Shutdown()
{
	for(int i = 0; i < MAX_SPRITES; ++i)
	{
		glDeleteTextures( 1, &sprites[i] );
	}
	board.Cleanup();
	//renderObjTest.Cleanup();
	//obj3d.Cleanup();
}

// Draw all the sprites
void Simulation::DrawFrame()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	board.Render();
	//obj3d.Render();
	//renderObjTest.Render();
}

void Simulation::SimulateOneFrame(float deltaTime)
{
	// Drag balls with mouse
	int curMx, curMy;
	unsigned char buttons = SDL_GetMouseState( &curMx, &curMy );
	m_curMouse = glm::vec2(curMx, curMy);
	glm::vec2 dragDelta( (float)(m_curMouse.x - m_prevMouse.x), (float)(m_curMouse.y - m_prevMouse.y) );
	m_prevMouse.x = m_curMouse.x;
	m_prevMouse.y = m_curMouse.y;

	RotationWrap();
	board.Update( deltaTime );
}

glm::vec2 Simulation::GetMousePos()
{
	return m_curMouse;
}

glm::mat4 Simulation::GetViewMat()
{
	// Returns an identity matrix that has been pushed backwards
	// This mat keeps track of the camera position, rotation, and scale
	glm::mat4 retval = glm::mat4(1.0f);
	retval = glm::translate( retval, glm::vec3(0,camHeight,camDist) );
	return retval;
}
glm::mat4 Simulation::GetProjMat()
{
	// This matrix keeps track of the camera's lens
	glm::mat4 retval = glm::perspective(60.f, (float)SCREEN_W / SCREEN_H, 1.0f, 1000.f );
	return 	retval;
}

void Simulation::RotationWrap()
{
	// Wrap any value greater than or equal to 180
	imgRot = fmod(imgRot,180.f);
	if(imgRot < 0)
	{
		imgRot += 360;
	}
}

void Log(const char *fmt, ...)
{
#if _DEBUG
	static char buf[1024];
	va_list args;
	va_start(args,fmt);
	vsprintf_s(buf,fmt,args);
	va_end(args);
	OutputDebugStringA(buf);
#endif
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void Simulation::Init()
{
	board.Setup( "checkerboard.png", glm::vec2(SCREEN_H, SCREEN_H), glm::vec2(SCREEN_W, SCREEN_H));
	//obj3d.Setup();
	//renderObjTest.Setup();

#if TWEAK_MENU
#define TWEAK_SEPERATOR()	TwAddSeparator(TwkBar(), "", "")

	TwAddVarRW(TwkBar(), "Cam dist",		TW_TYPE_FLOAT, &camDist,					"min=-500 max=500");
	TwAddVarRW(TwkBar(), "Cam Height",		TW_TYPE_FLOAT, &camHeight,					"min=-1 max=5");
	//TwAddVarRW(TwkBar(), "Gravity On",		TW_TYPE_BOOL8, &enableGravity,			"");
	//TwAddVarRW(TwkBar(), "Gravity",			TW_TYPE_FLOAT, &grav.y,					"min=0 max=200");
	//TwAddVarRW(TwkBar(), "Rotation On",		TW_TYPE_BOOL8, &enableRotation,			"");
	//TwAddVarRW(TwkBar(), "Rotation",		TW_TYPE_FLOAT, &imgRot,					"min=-360 max=360");

	TWEAK_SEPERATOR();
	//TwAddVarRW(TwkBar(), "Draw Lines",		TW_TYPE_BOOL8, &drawLines,				"");
#endif // TWEAK_MENU
}


