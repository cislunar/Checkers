
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

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "27014"

Simulation Simulation::m_singleton;

static RenderObjTest	renderObjTest;
static Obj3D			obj3d;
Board					board;

static float			camDist = -4.f;
static float			camHeight = 0.f;

struct addrinfo			*result = NULL,
						*ptr = NULL;
struct addrinfo			hints;
char					ipaddr[] = "127.0.0.1";


Simulation::Simulation()
{
	for(int i = 0; i < 322; i++) { // init them all to false
		m_KEYS[i] = false;
		m_prevKEYS[i] = false;
	}
	SDL_EnableKeyRepeat(0,0); 
	m_gameIsOver = false;
	m_haveLocalCmp = false;
	m_canDrawGameOver = false;
	m_userType = USER_TYPE_COUNT;
	m_listenSocket = INVALID_SOCKET;
	m_clientSocket = INVALID_SOCKET;
	m_connectSocket = INVALID_SOCKET;
	m_sendRecvBufLen = DEFAULT_BUFLEN;
}

Simulation::~Simulation()
{
	glDeleteTextures(1, &m_redWins);
	glDeleteTextures(1, &m_blackWins);

	board.Cleanup();
}


void Simulation::Shutdown()
{
	if(m_userType == USER_SERVER)
	{
		CleanupServer();
	}
	else
	{
		CleanupClient();
	}
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
	int curMx, curMy;
	m_curMouseButts = SDL_GetMouseState( &curMx, &curMy );
	m_curMouse = glm::vec2(curMx, curMy);
	glm::vec2 dragDelta( (float)(m_curMouse.x - m_prevMouse.x), (float)(m_curMouse.y - m_prevMouse.y) );
	m_prevMouse.x = m_curMouse.x;
	m_prevMouse.y = m_curMouse.y;

	if(board.GameIsOver() == false)
	{
		// If it's our turn, handle things normally
		if(board.IsPlayersTurn())
		{
			board.Update( deltaTime );
			// Handle end of turn if condition is met
			EndOfTurn();
		}
		else
		{
			// Wait for data, then handle it when it comes in
			BeginTurn();
		}
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

void Simulation::BeginTurn()
{
	CheckerPacket_Move cpm;
	if( ReceiveMovePacket( &cpm ) )
	{
		// handle the move
		board.HandleOtherPlayerMoves( cpm.cmp );
		// Allow the player to play
		board.SetPlayersTurnState( true );
	}
}

void Simulation::EndOfTurn() 
{
	// We have the final checkerMovePacket for this turn
	// We can send it
	if(m_haveLocalCmp)
	{
		// Send data
		m_haveLocalCmp = false;
		SendMovePacket();
		board.SetPlayersTurnState( false );
	}
}

bool Simulation::ReceiveMovePacket( CheckerPacket_Move* _cpm )
{
	bool retval = false;
	// Receive response---------------------------------------------------
	memset(m_recvbuf, 0, DEFAULT_BUFLEN);
	SOCKET recvSocket = m_userType == USER_CLIENT ? m_connectSocket : m_clientSocket;
	struct timeval tv;

	tv.tv_sec = 1000;  /* 1 Secs Timeout */

	setsockopt(recvSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval));
	m_iResult = recv(recvSocket, m_recvbuf, m_sendRecvBufLen, 0);
	if(m_iResult > 0)
	{
		uint32_t packetType = -1;
		memcpy(&packetType, m_recvbuf, sizeof(uint32_t));

		// Make sure this is the correct packet type before playing with it
		if( packetType == CHECKER_PACKET_MOVE )
		{
			memcpy(_cpm, m_recvbuf, sizeof(CheckerPacket_Move));
			retval = true;
		}
	}
	else if (m_iResult == 0)
	{
		printf("Connection closing...\n");
	}

	tv.tv_sec = 10000;  /* 10 Secs Timeout */
	setsockopt(recvSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval));
	return retval;
};

void Simulation::SendMovePacket()
{
	// Package up the CheckerMovePacket into a CheckerPacket_Move
	CheckerPacket_Move cpm;
	cpm.cmp = m_outCmp;
	cpm.packetType = CHECKER_PACKET_MOVE;

	SOCKET sendSocket = m_userType == USER_CLIENT ? m_connectSocket : m_clientSocket;

	// Clear the memory and copy  over the data
	memset(m_sendbuf, 0, sizeof(char) * DEFAULT_BUFLEN);
	size_t cpmSize = sizeof(CheckerPacket_Move);
	memcpy(m_sendbuf,  &cpm, cpmSize);

	// send MovePAcket--------------------------------------------------------
	m_iSendResult = send(sendSocket , m_sendbuf, m_sendRecvBufLen, 0 );
	if (m_iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(m_connectSocket);
		WSACleanup();
		return;
	}
}

void Simulation::ReceiveCheckerMovePacket( CheckerMovePacket _cmp )
{
	m_outCmp = _cmp;
	m_haveLocalCmp = true;
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

void Simulation::CleanupServer()
{
	// shutdown the connection since we're done
	m_iResult = shutdown(m_clientSocket, SD_SEND);
	if (m_iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(m_clientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(m_clientSocket);
	WSACleanup();
}

void Simulation::CleanupClient()
{
	// shutdown the connection since no more data will be sent
	m_iResult = shutdown(m_connectSocket, SD_SEND);
	if (m_iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(m_connectSocket);
		WSACleanup();
		return;
	}

	// Receive until the peer closes the connection
	do {

		m_iResult = recv(m_connectSocket, m_recvbuf, m_sendRecvBufLen, 0);
		if ( m_iResult > 0 )
		{

		}
		else if ( m_iResult == 0 )
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while( m_iResult > 0 );

	// cleanup
	closesocket(m_connectSocket);
	WSACleanup();
}

bool Simulation::SetupServer()
{
	bool retval = false;
	// Initialize Winsock
	m_iResult = WSAStartup(MAKEWORD(2,2), &m_wsaData);
	if (m_iResult != 0) {
		printf("WSAStartup failed with error: %d\n", m_iResult);
		return retval;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	m_iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if ( m_iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", m_iResult);
		WSACleanup();
		return retval;
	}

	// Create a SOCKET for connecting to server
	m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return retval;
	}

	// Setup the TCP listening socket
	m_iResult = bind( m_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (m_iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(m_listenSocket);
		WSACleanup();
		return retval;
	}

	// Notify Server user of wait time
	printf("%s\n", "Listening for Client...");

	// Listen for client
	m_iResult = listen(m_listenSocket, SOMAXCONN);
	if (m_iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return retval;
	}

	// Accept a client socket
	m_clientSocket = accept(m_listenSocket, NULL, NULL);
	if (m_clientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return retval;
	}

	printf("%s", "Client Connected!\n");
	// No longer need server socket
	closesocket(m_listenSocket);
	retval = true;
	return retval;
}

bool Simulation::SetupClient()
{
	bool retval = false;
	// Initialize Winsock
	m_iResult = WSAStartup(MAKEWORD(2,2), &m_wsaData);
	if (m_iResult != 0) {
		printf("WSAStartup failed with error: %d\n", m_iResult);
		return retval;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	m_iResult = getaddrinfo(ipaddr, DEFAULT_PORT, &hints, &result);
	if ( m_iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", m_iResult);
		WSACleanup();
		return retval;
	}

	// Notify Client user of wait time
	printf("%s\n","Attempting to connect to Server...");

	// Attempt to connect to an address until one succeeds
	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_connectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
			ptr->ai_protocol);
		if (m_connectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return retval;
		}

		// Connect to server.
		m_iResult = connect( m_connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (m_iResult == SOCKET_ERROR) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			closesocket(m_connectSocket);
			m_connectSocket = INVALID_SOCKET;
			return retval;
		}
		break;
	}

	if (m_connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return retval;
	}

	//-----------------------
	printf("%s", "Connected to Server!\n\n");
	retval = true;
	return retval;
}

bool Simulation::SetupNetworkingState()
{
	bool retval = false;
	printf("\nHOST GAME?: ");
	char* msg = (char*)malloc(MAX_INPUT_LEN);
	GetInput(msg, MAX_INPUT_LEN);
	// Wants to be server
	if( msg[0] == 'y' || msg[0] == 'Y' )
	{
		retval = SetupServer();
		m_userType = USER_SERVER;
	}
	// Wants to be client
	else
	{
		retval = SetupClient();
		m_userType = USER_CLIENT;
	}
	free( msg );
	return retval;
}

void Simulation::SetupGame()
{
	system("cls");
	printf("WELCOME TO CHECKERS!\n");
	while( SetupNetworkingState() == false );
	switch (m_userType)
	{
	case USER_CLIENT:
		board.SetPlayerType( Checker::RED_CHECKER );
		board.SetPlayersTurnState( false );
		break;
	case USER_SERVER:
		board.SetPlayerType( Checker::BLACK_CHECKER );
		board.SetPlayersTurnState( true );
		break;
	default:
	case USER_TYPE_COUNT:
		break;
	}
}

void Simulation::Init()
{
	m_redWins = LoadImage( "REDWINS.png");
	m_blackWins = LoadImage( "BLACKWINS.png");

	board.Setup( "checkerboard.png", glm::vec2(SCREEN_H, SCREEN_H), glm::vec2(SCREEN_W, SCREEN_H));
	SetupGame();

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


