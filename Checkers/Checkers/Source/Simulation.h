#pragma once

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>	// For random tint
#include <stdio.h>
#include "Math.h"
#include "Render.h"


void Log(const char *fmt, ...);


class Simulation
{
	enum USER_TYPE
	{
		USER_CLIENT,
		USER_SERVER,
		USER_TYPE_COUNT
	};

public:
	static Simulation* GetSimulation() {return &singleton;}


	void Init();
	void Shutdown();

	void SimulateOneFrame(float deltaTime);	// A single logic step
	void DrawFrame();						// Draw all the sprites
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
	~Simulation();

	// Statics and consts
	static const int	DEFAULT_BUFLEN = 1024;
	static const int	MAX_INPUT_LEN = 256;
	static const int	MAX_SPRITES = 2;
	static Simulation	singleton;

	void				ShowGameOver();
	void				SetupNetworkingState();
	void				SetupGame();
	void				EatExtraInput(void);
	void				GetInput( char* _inputBuf, const int _maxMsgLen );
	void				SetupServer();
	void				SetupClient();
	void				CleanupServer();
	void				CleanupClient();

	glm::vec2			m_terrainRes;
	glm::vec2			m_curMouse;
	glm::vec2			m_prevMouse;
	bool				m_KEYS[322]; // Number of SDLK_DOWN events
	bool				m_prevKEYS[322]; // Number of SDLK_DOWN events
	unsigned char		m_prevMouseButts;
	unsigned char		m_curMouseButts;
	GLuint				m_redWins;
	GLuint				m_blackWins;
	GLuint*				m_winner;
	bool				m_gameIsOver;
	bool				m_canDrawGameOver;
	USER_TYPE			m_userType;
	WSADATA				wsaData;
	int					iResult;
	SOCKET				ListenSocket;
	SOCKET				ClientSocket;
	SOCKET				ConnectSocket;

	int					iSendResult;
	char				sendbuf[DEFAULT_BUFLEN];
	int					sendbuflen;
	char				recvbuf[DEFAULT_BUFLEN];
	int					recvbuflen;
};
