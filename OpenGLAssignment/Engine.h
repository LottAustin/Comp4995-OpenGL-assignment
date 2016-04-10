#ifndef ENGINE_H
#define ENGINE_H

#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <math.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <GL\SOIL.h>			// Header File For The SOIL Library
#include "MilkshapeModel.h"		

class Engine {	
public:

	HDC			hDC = NULL;		// Private GDI Device Context
	HGLRC		hRC = NULL;		// Permanent Rendering Context
	HWND		hWnd = NULL;		// Holds Our Window Handle
	HINSTANCE	hInstance;		// Holds The Instance Of The Application
	Model*		pModel = NULL;

	Engine() {}

	// Initializes OpenGL items
	int		InitGL(GLvoid);

	GLvoid KillGLWindow(GLvoid);

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

	int		DrawGLScene(GLvoid);

	bool	GetScreenMode();

	void	SetScreenMode(bool mode);

	GLfloat	GetDepth();

	void	SetDepth(GLfloat z);

	GLfloat GetHorizontal();

	void	SetHorizontal(GLfloat x);

	GLfloat GetXSpeed();

	void	SetXSpeed(GLfloat speed);

	GLfloat GetYSpeed();

	void	SetWalkbias(float wb);

	void	SetYSpeed(GLfloat speed);

	bool	GetActive();
	
	void	SetActive(bool active);

	void	useBuffers(float*, int, float*, int, GLuint, int);

	GLvoid	DrawWater();

private:
	bool		fullscreen_ = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
	bool		active_ = TRUE;		// Window Active Flag Set To TRUE By Default

	GLfloat		LightAmbient_[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat		LightDiffuse_[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat		LightPosition_[4] = { 0.0f, 0.0f, 2.0f, 1.0f };

	GLuint		texture_[3];		// Storage For 3 Textures


	float vertex_buffer[12] = {
		-25.0f, 0.0f, -25.0f,
		-25.0f, 0.0f, -24.0f,
		-24.0f, 0.0f, -25.0f,
		-24.0f, 0.0f, -24.0f
	};

	float indices_buffer[4] = {
		0, 1, 2, 3
	};

	GLfloat		xrot_;				// X Rotation
	GLfloat		yrot_;				// Y Rotation
	GLfloat		xspeed_;			// X Rotation Speed
	GLfloat		yspeed_;			// Y Rotation Speed
	GLfloat		z_ = -5.0f;			// Depth Into The Screen
	GLfloat		wave = 0.0f;
	double eqr[4] = { 0.0f, -1.0f, 0.0f, sin(wave) / 10 };
	GLfloat		x_ = 0;				// Horizontal Position In The Screen
	GLfloat		y_ = 0;				// Verticall Position In The Screen
	int texCount_;
	float walkbias = 0;

	// loads textures into memory
	int			LoadGLTexture(const char* filePath);

	// Loads a basic Box mesh made via OpenGL quads
	void		MakeBox();

	// Loads a basice Floor made via a OpenGL quad
	void		MakeFloor();
};

#endif