#include "Render.h"
#include "SDL.h"
#include "SDL_image.h"

#include "Main.h"
#include "Simulation.h"

#include <stdio.h> 

#if TWEAK_MENU
TwBar *myBar;
TwBar *TwkBar()
{
	return myBar;
}
#endif // TWEAK_MENU

SDL_Event event;
float x=100,y=100;


void StartFrame()
{
	// Clear the screen before drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glTranslatef(SCREEN_W/2,SCREEN_HEIGHT/2,0.0);	// Moves world origin to center of window
//	glRotatef( spin, 0.0, 0.0, 1.0 );				// Rotate camera
}

void EndFrame()
{
	glLoadIdentity();
	SDL_GL_SwapBuffers();
}

int init()
{
	SDL_Surface *screen;
	// Slightly different SDL initialization
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	screen = SDL_SetVideoMode( SCREEN_W, SCREEN_H, 0, SDL_OPENGL | SDL_OPENGL);//SDL_FULLSCREEN );	
	if ( !screen ) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

void clean_up()
{
#if TWEAK_MENU
	TwTerminate();
#endif // TWEAK_MENU
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	//Make sure the program waits for a quit
	bool running = true;
	bool step = false;
	bool quit = false;
	init();
	Render *pRender = Render::GetInstance();
	pRender->init_GL();

#if TWEAK_MENU
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(SCREEN_W, SCREEN_H);
	myBar = TwNewBar("Tweak");
#endif // TWEAK_MENU

	// Initialize the FPS tracking variables
	Uint32 nextFPSCount = SDL_GetTicks() + 1000;
	int frames = 0;
	int prevTime = SDL_GetTicks();

	Simulation *pSimulation = Simulation::GetSimulation();
	pSimulation->Init();
	while( quit == false )
	{
		while( SDL_PollEvent( &event ) )
		{
#if TWEAK_MENU
			int handled = TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
			if( handled )
				continue;
#endif // TWEAK_MENU

			switch( event.type )
			{
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_KEYDOWN:
				pSimulation->UpdateKeys(event.key.keysym.sym, true);
				switch( event.key.keysym.sym )
				{
				case SDLK_1:
					break;
				case SDLK_RIGHT:
					step = true;
					break;
				case SDLK_ESCAPE:
					quit=true;
					break;
				}
				break;
			case SDL_KEYUP:
				pSimulation->UpdateKeys(event.key.keysym.sym, false);
				switch( event.key.keysym.sym )
				{
				case SDLK_RIGHT:
					break;
				}
				break;
			}
		}

		if( running || step )
		{
			step = false;
			StartFrame();

			int newTime = SDL_GetTicks();
			float elapsed = (float)(newTime - prevTime) / 1000.0f;
			if( elapsed > 0.05f )
				elapsed = 0.05f;	// Clamp long dt's down
			if( elapsed < 0.001f)
				continue;			// dt too short. Skip to next frame (without updating prevTime so dt grows).
			prevTime = newTime;

			pSimulation->DrawFrame();				// Render all sprites

			const int subSteps = 1;
			for( int i=0; i<subSteps; i++ )
			{
				pSimulation->SimulateOneFrame(elapsed / subSteps);	// Logic update for Simulation
			}

#if TWEAK_MENU
			TwDraw();
#endif // TWEAK_MENU

			EndFrame();

			// Once per second, show the FPS
			frames++;
			if( SDL_GetTicks() >  nextFPSCount )
			{
				char buffer[256];
				sprintf_s(buffer, "FPS: %d", frames );
				SDL_WM_SetCaption( buffer, NULL );
				nextFPSCount = SDL_GetTicks() + 1000;
				frames = 0;
			}
		}
	}
	pSimulation->Shutdown();
	clean_up();
	return 0;
}

