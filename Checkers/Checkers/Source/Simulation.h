#pragma once

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>	// For random tint
#include <stdio.h>
#include "Math.h"
#include "Render.h"
#include "CheckerPacketStructs.h"


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
	static Simulation* GetSimulation() {return &m_singleton;}


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
	void ReceiveCheckerMovePacket( CheckerMovePacket _cmp );


private:
	Simulation();							// Force use of singleton
	~Simulation();
	void				ShowGameOver();
	bool				SetupNetworkingState();
	void				SetupGame();
	void				EatExtraInput(void);
	void				GetInput( char* _inputBuf, const int _maxMsgLen );
	bool				SetupServer();
	bool				SetupClient();
	void				CleanupServer();
	void				CleanupClient();
	void				EndOfTurn();
	void				SendMovePacket();
	void				HandleMovePacket( CheckerPacket_Move* _cpm );
	void				HandleChatPacket( CheckerPacket_Chat* _cpc );
	void				HandleCheckerPacket( char* _data );
	void				SetupReceivePacketThread();
	void				SignalAnnouncement();
	int					ReceiveCheckerPackets( );
	int					GetChatData();

	static int			ReadChat_ThreadProxy( void* pParam )
	{
		return ((Simulation*)pParam)->GetChatData();
	}

	static int			ReceivePackets_ThreadProxy( void* pParam )
	{
		return ((Simulation*)pParam)->ReceiveCheckerPackets();
	}

	// Statics and consts
	
	static const int	MAX_SPRITES = 2;
	static const int	SDL_KEY_CNT = 322;
	static Simulation	m_singleton;
	
	glm::vec2			m_terrainRes;
	glm::vec2			m_curMouse;
	glm::vec2			m_prevMouse;
	bool				m_KEYS[SDL_KEY_CNT]; // Number of SDLK_DOWN events
	bool				m_prevKEYS[SDL_KEY_CNT]; // Number of SDLK_DOWN events
	unsigned char		m_prevMouseButts;
	unsigned char		m_curMouseButts;
	GLuint				m_redWins;
	GLuint				m_blackWins;
	GLuint*				m_winner;
	bool				m_gameIsOver;
	bool				m_canDrawGameOver;
	USER_TYPE			m_userType;
	WSADATA				m_wsaData;
	int					m_iResult;
	SOCKET				m_listenSocket;
	SOCKET				m_clientSocket;
	SOCKET				m_connectSocket;
	CheckerMovePacket	m_outCmp;
	bool				m_haveLocalCmp;
	bool				m_receivePackets;
	bool				m_readChatData;
	SDL_Thread			*thread;
	bool				m_otherPlayerQuit;

	int					m_iSendResult;
	int					m_sendRecvBufLen;
	char				m_sendbuf[DEFAULT_BUFLEN];
	char				m_recvbuf[DEFAULT_BUFLEN];
};
