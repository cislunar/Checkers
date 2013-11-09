#include "Main.h"
#include "Render.h"
#include "filereader.h"
#include <assert.h>

Render Render::singleton;



Render::Render()
{
}

Render::~Render()
{
	//CleanUpDrawLine();
}

void Render::init_GL()
{
	// Set the OpenGL state after creating the context with SDL_SetVideoMode
	glClearColor( 0, 0, 0, 0 );
	glEnable( GL_TEXTURE_2D ); // Need this to display a texture
	glViewport( 0, 0, SCREEN_W, SCREEN_H );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, SCREEN_W, SCREEN_H, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	GLenum err = glewInit();
	fprintf(stderr, "Glew Error: %s\n", glewGetErrorString(err));
	if (err != GLEW_OK)
		exit(1); // or handle the error in a nicer way
	if(GLEW_VERSION_2_0)
	{
		printf("GLEW_VERSION_2_0 is supported\n");
	}
	else
	{
		printf("GLEW_VERSION_2_0 is not supported\n");
	}
}

void DrawImage( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height )
{
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;
	// Bind the texture to which subsequent calls refer to
//	glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, sourceTexture );
	glDisable ( GL_ALPHA_TEST ) ;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable ( GL_BLEND ) ;
	glBegin( GL_QUADS );
	// Top-left vertex (corner)
	glTexCoord2i( 0, 0 );
	glVertex3f( xPos-halfW, yPos-halfH, 0 );
	// Bottom-left vertex (corner)
	glTexCoord2i( 1, 0 );
	glVertex3f( xPos+halfW, yPos-halfH, 0 );
	// Bottom-right vertex (corner)
	glTexCoord2i( 1, 1 );
	glVertex3f( xPos+halfW, yPos+halfH, 0 );
	// Top-right vertex (corner)
	glTexCoord2i( 0, 1 );
	glVertex3f( xPos-halfW, yPos+halfH, 0 );
	glEnd();
}

void DrawImgRot( GLuint sourceTexture, Color& color, float xPos, float yPos, float width, float height, float rot )
{
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;
	
	// NOTE: gl functions occur in reverse order from what is listed below
	//Return sprite to original position
	glTranslatef(xPos, yPos, 0);
	glRotatef( rot, 0.0, 0.0, 1.0 );
	// Move sprite to draw window origin (0,0) for rotation
	glTranslatef(-xPos, -yPos, 0);

	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);

	glEnable( GL_TEXTURE_2D );
	// Bind the texture to which subsequent calls refer to
	glBindTexture( GL_TEXTURE_2D, sourceTexture );
	glDisable ( GL_ALPHA_TEST ) ;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable ( GL_BLEND ) ;
	glBegin( GL_QUADS );
	// Top-left vertex (corner)
	glTexCoord2i( 0, 0 );
	glVertex3f( xPos-halfW, yPos-halfH, 0 );
	// Bottom-left vertex (corner)
	glTexCoord2i( 1, 0 );
	glVertex3f( xPos+halfW, yPos-halfH, 0 );
	// Bottom-right vertex (corner)
	glTexCoord2i( 1, 1 );
	glVertex3f( xPos+halfW, yPos+halfH, 0 );
	// Top-right vertex (corner)
	glTexCoord2i( 0, 1 );
	glVertex3f( xPos-halfW, yPos+halfH, 0 );
	glEnd();
}

void DrawLine( glm::vec2& start, glm::vec2& end, Color& color )
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

GLuint LoadImage( char* filename )
{
	GLuint texture=NULL;
	GLenum texture_format=NULL;
	SDL_Surface *surface; // this surface will tell us the details of the image
	GLint nofcolors;

	if ( (surface = IMG_Load(filename)) ) 
	{
		// Check that the image’s width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) 
		{
			printf("warning: image.bmp’s width is not a power of 2\n");
		}
		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) 
		{
			printf("warning: image.bmp’s height is not a power of 2\n");
		}
		//get number of channels in the SDL surface
		nofcolors=surface->format->BytesPerPixel;
		//contains an alpha channel
		if(nofcolors==4)
		{
			if(surface->format->Rmask==0x000000ff)
				texture_format=GL_RGBA;
			else
				texture_format=GL_BGRA;
		}
		else if(nofcolors==3) //no alpha channel
		{
			if(surface->format->Rmask==0x000000ff)
				texture_format=GL_RGB;
			else
				texture_format=GL_BGR;
		}
		else
		{
			printf("warning: the image is not truecolor…this will break ");
		}
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
		// Set the texture’s stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, nofcolors, surface->w, surface->h, 0,
			texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	}
	else 
	{
		char temp[256];
		sprintf_s( temp, "SDL could not load image: %s\n", SDL_GetError() );
		OutputDebugStringA(temp);
		SDL_Quit();
		return 0;
	}
	// Free the SDL_Surface only if it was successfully created
	if ( surface ) 
	{
		SDL_FreeSurface( surface );
	}
	return texture;
}

void LoadShader(ShaderInfo* _shader, ShaderFiles sFiles)
{
	char *vs = NULL,*fs = NULL;

	GLuint vert	= 0;
	vert =	glCreateShader(GL_VERTEX_SHADER);
	GLuint frag	= glCreateShader(GL_FRAGMENT_SHADER);

	// Read in the shader files
	vs = textFileRead(sFiles.vertFile);
	fs = textFileRead(sFiles.fragFile);

	const char * vv = vs;
	const char * ff = fs;

	// Grab source files for vert and frag shaders
	glShaderSource(vert, 1, &vv,NULL);
	glShaderSource(frag, 1, &ff,NULL);

	free((char*)vv);
	free((char*)ff);

	// Compile Shaders--------------------------------------------------------
	// Compile vertex shader, then test if it was successful
	glCompileShader(vert);
	GLQueryCompileStatus(vert, GL_VERTEX_SHADER);

	// Compile fragment shader, then test if it was successful
	glCompileShader(frag);
	GLQueryCompileStatus(frag, GL_FRAGMENT_SHADER);

	// Create shader programs-------------------------------------------------------
	// Create shader program to become an anchor point of shaders to link to.
	// Multiple shaders of each type can be attached here, but only one file per type (vert and frag)
	// can have a main() function
	GLuint prog = glCreateProgram();
	glAttachShader(prog,frag);
	glAttachShader(prog,vert);

	// Link Programs-----------------------------------------------------------------
	// Link program into openGL, then test to see if there were any errors
	glLinkProgram(prog);
	GLQueryLinkStatus(prog);

	// Validate shaders---------------------------------------------------------------
	// Validate that all shaders play well together, see if there are errors
	glValidateProgram(prog);
	GLQueryValidation(prog);

	// Assign values
	_shader->VertexShaderId = vert;
	_shader->FragmentShaderId = frag;
	_shader->ProgramId = prog;
}

void GLQueryCompileStatus(GLuint shader, GLenum type)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", type, InfoLog);
		assert(false);
	}
}

void GLQueryLinkStatus(GLuint prog)
{
	GLint success;
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		assert(false);
	}
}


void GLQueryValidation(GLuint prog)
{
	GLint success;
	// Validate that all shaders play well together

	glGetProgramiv(prog, GL_VALIDATE_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error validating shader program: '%s'\n", ErrorLog);
		assert(false);
	}
}

int printOglError(char *file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}