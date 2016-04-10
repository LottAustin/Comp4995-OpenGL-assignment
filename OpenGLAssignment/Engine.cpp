#include "Engine.h"

int Engine::InitGL(GLvoid)								// All Setup For OpenGL Goes Here
{
	pModel = new MilkshapeModel();									// Memory To Hold The Model
	if (pModel->loadModelData("data/House01.ms3d") == false)		// Loads The Model And Checks For Errors
	{
		MessageBox(NULL, "Couldn't load the model data\\model.ms3d", "Error", MB_OK | MB_ICONERROR);
		return 0;													// If Model Didn't Load Quit
	}

	pModel->reloadTextures();

	texCount_ = 0;										// initialize counter for textures

	if (!LoadGLTexture("Data/Crate.bmp"))				// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	if (!LoadGLTexture("Data/Sand.bmp"))				// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	xspeed_ = 0.0f;
	yspeed_ = 0.0f;
	xrot_ = 0.0f;
	yrot_ = 0.0f;

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient_);	// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse_);	// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition_);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One

	SetDepth(-20.0f);

	return TRUE;										// Initialization Went OK
}

int Engine::LoadGLTexture(const char* filePath)			// Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture_[texCount_] = SOIL_load_OGL_texture
		(
			filePath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
			);

	if (texture_[texCount_] == 0)
		return false;


	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture_[texCount_++]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;											// Return Success
}

GLvoid Engine::ReSizeGLScene(GLsizei width, GLsizei height)	// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;											// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix
	glLoadIdentity();										// Reset The Projection Matrix

															// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix
	glLoadIdentity();										// Reset The Modelview Matrix
}

int Engine::DrawGLScene(GLvoid)								// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glLoadIdentity();										// Reset The View

	GLfloat ytrans = walkbias - 5.0f;

	glRotatef(-yspeed_, 0.0f, 1.0f, 0.0f);
	glTranslatef(x_, ytrans, z_);
	glDisable(GL_LIGHTING);
	glTranslatef(0.0f, -10.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_[1]);
	MakeFloor();
	glTranslatef(0.0f, 10.0f, 0.0f);
	glEnable(GL_LIGHTING);

	glColorMask(0, 0, 0, 0);

	double eqr[4] = { 0.0f, -1.0f, 0.0f, sin(wave) / 10 };
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glDisable(GL_DEPTH_TEST);
	DrawWater();
	glEnable(GL_DEPTH_TEST);

	glColorMask(1, 1, 1, 1);
	glStencilFunc(GL_EQUAL, 1, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glEnable(GL_CLIP_PLANE0);

	glClipPlane(GL_CLIP_PLANE0, eqr);
	glPushMatrix();
	glTranslatef(0.0f, sin(wave) / 10, 0.0f);
	glScalef(0.5f, -0.5f, 0.5f);
	pModel->draw();
	glPopMatrix();
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawWater();

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glScalef(0.5f, 0.5f, 0.5f);
	pModel->draw();
	//xspeed_ = yspeed_ = 0;

	xrot_ += xspeed_;
	yrot_ += yspeed_;
	wave += 0.005;
	return TRUE;										// Keep Going
}

GLvoid Engine::KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen_)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}

	
}

bool Engine::GetScreenMode()
{
	return fullscreen_;
}

void Engine::SetScreenMode(bool mode) {
	fullscreen_ = mode;
}

GLfloat Engine::GetDepth() {
	return z_;
}

void Engine::SetDepth(GLfloat z) {
	z_ = z;
}

GLfloat Engine::GetHorizontal() {
	return x_;
}

void Engine::SetHorizontal(GLfloat x) {
	x_ = x;
}

GLfloat Engine::GetXSpeed() {
	return xspeed_;
}

void Engine::SetXSpeed(GLfloat speed) {
	xspeed_ = speed;
}

GLfloat Engine::GetYSpeed() {
	return yspeed_;
}

void Engine::SetActive(bool active) {
	active_ = active;
}

bool Engine::GetActive() {
	return active_;
}

void Engine::SetYSpeed(GLfloat speed) {
	yspeed_ = speed;
}

void Engine::MakeBox() {
	glBegin(GL_QUADS);
	// Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
}

void Engine::useBuffers(float* vertices, int vert_size, float* indices, int ind_size, GLuint type, int number) {
	int finali = 0;
	int first, second, third, forth;
	float waves, prevPos = sin(wave) / 10;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (float x = 0; x < number; x++) {
		for (float z = 0; z < number; z++) {
			for (int i = 0; i < ind_size; i += 4) {
				first = indices[i]; second = indices[i + 1]; third = indices[i + 2]; forth = indices[i + 3];
				waves = sin((wave + z)) / 10;
				glBegin(type);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glColor4f(0.2f, 0.5f, 1.0f, 0.8f);

				glVertex3f(vertices[first * 3] + x, vertices[(first * 3) + 1] + prevPos, vertices[(first * 3) + 2] + z);
				glVertex3f(vertices[second * 3] + x, vertices[(second * 3) + 1] + waves, vertices[(second * 3) + 2] + z);
				glVertex3f(vertices[third * 3] + x, vertices[(third * 3) + 1] + prevPos, vertices[(third * 3) + 2] + z);
				glVertex3f(vertices[forth * 3] + x, vertices[(forth * 3) + 1] + waves, vertices[(forth * 3) + 2] + z);
				glEnd();
				prevPos = waves;
			}
		}
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

GLvoid Engine::DrawWater() {
	int num = 1;
	GLfloat pointx = 1.0f, pointy = -1.0f;
	int indices_size = sizeof(indices_buffer) / sizeof(indices_buffer[0]);
	int vertex_size = sizeof(vertex_buffer) / sizeof(vertex_buffer[0]);

	useBuffers(vertex_buffer, vertex_size, indices_buffer, indices_size, GL_TRIANGLE_STRIP, 50);
}

void Engine::MakeFloor() {
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, 1.0f, -50.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 1.0f, 50.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 1.0f, 50.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 1.0f, -50.0f);
	glEnd();
}

void Engine::SetWalkbias(float wb) {
	walkbias = wb;
}