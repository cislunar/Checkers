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
	for(int i = 0; i < 322; i++) { // init them all to false
		m_KEYS[i] = false;
		m_prevKEYS[i] = false;
	}
	SDL_EnableKeyRepeat(0,0); 
	m_gameIsOver = false;
	m_canDrawGameOver = false;
	m_userType = USER_TYPE_COUNT;
}


void Simulation::Shutdown()
{
	for(int i = 0; i < MAX_SPRITES; ++i)
	{
		glDeleteTextures( 1, &sprites[i] );
	}

	glDeleteTextures(1, &m_redWins);
	glDeleteTextures(1, &m_blackWins);

	board.Cleanup();
}

// Draw all the sprites
void Simulation::DrawFrame()
{
	board.Render();
	if(m_gameIsOver
		&& m_canDrawGameOver )
	{
		DrawImage(
			*m_winner,
			Color(1,1,1,1),
			SCREEN_W/2.f,
			SCREEN_H/2.f,
			1024.f,
			512.f);
	}
}

void Simulation::SimulateOneFrame(float deltaTime)
{
	// Drag balls with mouse
	int curMx, curMy;
	m_curMouseButts = SDL_GetMouseState( &curMx, &curMy );
	m_curMouse = glm::vec2(curMx, curMy);
	glm::vec2 dragDelta( (float)(m_curMouse.x - m_prevMouse.x), (float)(m_curMouse.y - m_prevMouse.y) );
	m_prevMouse.x = m_curMouse.x;
	m_prevMouse.y = m_curMouse.y;

	RotationWrap();
	if(board.GameIsOver() == false)
	{
		board.Update( deltaTime );
	}
	else
	{
		if(m_gameIsOver == false)
		{
			m_gameIsOver = true;
			if(board.HasPiecesLeft(Checker::BLACK_CHECKER))
			{
				m_winner = &m_blackWins;
			}
			else
			{
				m_winner = &m_redWins;
			}
		}
		m_canDrawGameOver = sin( (double)SDL_GetTicks()/200.f ) > 0;
	}

	memcpy(m_prevKEYS, m_KEYS, sizeof(bool) * 322);
	m_prevMouseButts = m_curMouseButts;
}

bool Simulation::GetCurMouseButton( int _button )
{
	return _button > 0 && _button < SDL_BUTTON_WHEELDOWN 
				&& m_curMouseButts & SDL_BUTTON(_button);
}

bool Simulation::GetPrevMouseButton( int _button )
{
	return _button > 0 && _button < SDL_BUTTON_WHEELDOWN 
				&& m_prevMouseButts & SDL_BUTTON(_button);
}


bool Simulation::GetOnMouseButtonDown( int _button )
{
	return GetCurMouseButton( _button ) == true
		&& GetPrevMouseButton( _button ) == false;
}

bool Simulation::GetOnKeyDown( SDLKey _key )
{
	bool retval = (m_KEYS[_key] == true && m_prevKEYS[_key] == false);
	return retval;
}

bool Simulation::GetKey( SDLKey _key )
{
	return m_KEYS[_key];
}

void Simulation::UpdatePrevKeys( SDLKey _key, bool _State )
{
	m_prevKEYS[_key] = _State;
}

void Simulation::UpdateKeys(SDLKey _key, bool _State )
{
	m_KEYS[_key] = _State;
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

void Simulation::GetInput( char* _inputBuf, const int _maxMsgLen )
{
	// Get input
	memset(_inputBuf, 0, sizeof(char) * _maxMsgLen);
	if(fgets(_inputBuf, _maxMsgLen - 1, stdin))
	{
		// If we didn't get a newline, 
		// eat the rest of the characters until we get a newline
		if (NULL == strchr(_inputBuf, '\n'))
		{
			EatExtraInput(); 
			_inputBuf[_maxMsgLen-2] = '\n';
		}
		printf("%s", "\n");
	}
}

void Simulation::EatExtraInput(void) 
{
	int ch;
	// Eat characters until we get the newline
	while ((ch = getchar()) != '\n') {
		if (ch < 0)
			exit(EXIT_FAILURE); // EOF!
	}
}

void Simulation::SetupNetworkingState()
{
	
}

void Simulation::SetupGame()
{
	printf("WELCOME TO CHECKERS!\n");
	printf("PLAY GAME?: ");
}

void Simulation::Init()
{
	m_redWins = LoadImage( "REDWINS.png");
	m_blackWins = LoadImage( "BLACKWINS.png");

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


