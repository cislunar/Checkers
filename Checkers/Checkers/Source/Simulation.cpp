
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
#define DEFAULT_PORT "27017"

Simulation Simulation::singleton;

static RenderObjTest	renderObjTest;
static Obj3D			obj3d;
Board					board;

static float			camDist = -4.f;
static float			camHeight = 0.f;

struct addrinfo			*result = NULL,
						*ptr = NULL;
struct addrinfo			hints;
char					ipaddr[] = "localhost";


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
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	ConnectSocket = INVALID_SOCKET;
	recvbuflen = DEFAULT_BUFLEN;
	sendbuflen = DEFAULT_BUFLEN;
}

Simulation::~Simulation()
{
	glDeleteTextures(1, &m_redWins);
	glDeleteTextures(1, &m_blackWins);

	board.Cleanup();
}


void Simulation::Shutdown()
{
	
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
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
}

void Simulation::CleanupClient()
{
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if ( iResult > 0 )
			printf("Bytes received: %d\n", iResult);
		else if ( iResult == 0 )
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while( iResult > 0 );

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}

void Simulation::SetupServer()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Notify Server user of wait time
	printf("%s\n", "Listening for Client...");

	// Listen for client
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	printf("%s", "Client Connected!\n");
	// No longer need server socket
	closesocket(ListenSocket);
}

void Simulation::SetupClient()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ipaddr, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Notify Client user of wait time
	printf("%s\n","Attempting to connect to Server...");

	// Attempt to connect to an address until one succeeds
	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	//-----------------------
	printf("%s", "Connected to Server!\n\n");
}

void Simulation::SetupNetworkingState()
{
	printf("\nHOST GAME?: ");
	char* msg = (char*)malloc(MAX_INPUT_LEN);
	GetInput(msg, MAX_INPUT_LEN);
	// Wants to be server
	if( msg[0] == 'y' || msg[0] == 'Y' )
	{
		SetupServer();
	}
	// Wants to be client
	else
	{
		SetupClient();
	}
	free( msg );
}

void Simulation::SetupGame()
{
	printf("WELCOME TO CHECKERS!\n");
	SetupNetworkingState();
}

void Simulation::Init()
{
	m_redWins = LoadImage( "REDWINS.png");
	m_blackWins = LoadImage( "BLACKWINS.png");

	board.Setup( "checkerboard.png", glm::vec2(SCREEN_H, SCREEN_H), glm::vec2(SCREEN_W, SCREEN_H));
	//obj3d.Setup();
	//renderObjTest.Setup();
	system("cls");
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


