#ifndef ENGINE_H
#define ENGINE_H

#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
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

	GLuint	GetFilter();

	void	SetFilter(GLuint filter);

	GLfloat	GetDepth();

	void	SetDepth(GLfloat z);

	GLfloat GetXSpeed();

	void	SetXSpeed(GLfloat speed);

	GLfloat GetYSpeed();

	void	SetYSpeed(GLfloat speed);

	bool	GetActive();
	
	void	SetActive(bool active);

private:
	bool		fullscreen_ = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
	bool		active_ = TRUE;		// Window Active Flag Set To TRUE By Default

	GLfloat		LightAmbient_[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat		LightDiffuse_[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat		LightPosition_[4] = { 0.0f, 0.0f, 2.0f, 1.0f };

	GLuint		texture_[3];		// Storage For 3 Textures

	GLfloat		xrot_;				// X Rotation
	GLfloat		yrot_;				// Y Rotation
	GLfloat		xspeed_;			// X Rotation Speed
	GLfloat		yspeed_;			// Y Rotation Speed
	GLfloat		z_ = -5.0f;			// Depth Into The Screen
	int texCount_;

	// loads textures into memory
	int			LoadGLTexture(const char* filePath);

	void		MakeBox();
};

#endif