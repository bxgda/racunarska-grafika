#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")
#define _USE_MATH_DEFINES
#include <math.h>

#define TO_RAD 3.14 / 180.0f
#define TO_DEG 180.0f / 3.14

CGLRenderer::CGLRenderer(void)
{
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	m_hrc = wglCreateContext(pDC->m_hDC);

	if (!m_hrc) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	prostorija = LoadTexture("prostorija.png");
	metal = LoadTexture("metal.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// -----------------------------------

	UpdateCamera();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	float pozicijaLight1[] = { 0.0f, 3.9, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, pozicijaLight1);

	float ambiLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambiLight1);

	float difLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difLight1);

	float specLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_SPECULAR, specLight1);

	float spotDir[] = { 0.0f, -1.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0f);     
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 8.0f);    

	// -----------------------------------------

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, prostorija);
	glEnable(GL_CULL_FACE);

	float difMat[] = { 0.7f, 1.0f, 0.7f, 1.0f };
	SetMaterial(difMat);

	// -----------------------------------------

	DrawRoller(5, 4, 20, false);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	float difMat1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(difMat1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, metal);

	glTranslatef(0, 0, -0.1 * 5);

	DrawCourousel(0.8, 0.2, 20, ugaoKorpa, ugao2, ugao1, slide, ugao3, brojDelova);

	glDisable(GL_TEXTURE_2D);

	// -----------------------------------

	glFlush();
	SwapBuffers(pDC->m_hDC);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glDeleteTextures(1, &metal);
	glDeleteTextures(1, &prostorija);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT idTeksture;
	DImage tekstura;
	tekstura.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &idTeksture);

	glBindTexture(GL_TEXTURE_2D, idTeksture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tekstura.Width(), tekstura.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, tekstura.GetDIBBits());
	return idTeksture;
}

void CGLRenderer::SetMaterial(float diffuse[])
{
	float amb[] = { diffuse[0] / 2, diffuse[1] / 2, diffuse[2] / 2, diffuse[3] / 2 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);

	float dif[] = { diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);

	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

	float sjaj = 64.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, sjaj);
}

void CGLRenderer::DrawRoller(float r, float h, int nR, bool bHalf)
{
	glColor3f(1, 1, 1);

	double korak = 2 * M_PI / nR;

	double krug;
	if (bHalf)
		krug = nR / 2;
	else
		krug = nR;

	// omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= krug; i++)
	{
		double ugao = i * korak;
		double x1 = r * cos(ugao);
		double z1 = r * sin(ugao);

		double x2 = r * cos(ugao);
		double z2 = r * sin(ugao);

		double nx = cos(ugao);
		double nz = sin(ugao);

		double u = (double)i / nR;

		glNormal3f(nx, 0, nz);

		glTexCoord2f(u, 1.0);
		glVertex3f(x1, h, z1);

		glTexCoord2f(u, 0.0);
		glVertex3f(x2, 0, z2);
	}
	glEnd();

	// donja baza
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3d(0, 0, 0);
	for (int i = krug; i >= 0; i--)
	{
		double ugao = i * korak;
		double x = r * cos(ugao);
		double z = r * sin(ugao);
		float u = 0.5f + 0.5 * x;
		float v = 0.5f + 0.5 * z;
		glTexCoord2f(u, v);
		glVertex3d(x, 0, z);
	}
	glEnd();

	// gornja baza
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3d(0, h, 0);
	for (int i = 0; i <= krug; i++)
	{
		double ugao = i * korak;
		double x = r * cos(ugao);
		double z = r * sin(ugao);
		float u = 0.5f + 0.5 * x;
		float v = 0.5f + 0.5 * z;
		glTexCoord2f(u, v);
		glVertex3d(x, h, z);
	}
	glEnd();

	if (bHalf)
	{
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= nR; i++) 
		{
			float x = -r + 2 * r * i / nR;
			float u = (float)i / nR;
			glNormal3f(0, 0, -1);  
			glTexCoord2f(u, 1.0f); glVertex3f(x, h, 0);
			glTexCoord2f(u, 0.0f); glVertex3f(x, 0, 0);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCarouselPart(float r, float h, int n, float alpha)
{
	glPushMatrix();

	DrawRoller(0.1 * r, h, n, true);

	glTranslatef(0, h, 0);

	DrawRoller(0.1 * r, h, n, false);

	glTranslatef(0, h, 0);

	glRotatef(alpha, 0, 1, 0);

	DrawRoller(r, h, n, false);

	glTranslatef(0, h, 0);


	for (int i = 0; i < 5; i++)
	{
		glPushMatrix();

		glRotatef(i * (360 / 5), 0, 1, 0);
		glTranslatef(0.8 * r, 0, 0);

		DrawRoller(0.15 * r, 0.5 * h, n, false);

		glPopMatrix();
	}

	glPopMatrix();
}

void CGLRenderer::DrawCarouselPartWithArm(float r, float h, int n, float alpha, float beta, float gamma, float slide)
{
	glPushMatrix();

	DrawRoller(0.1 * r, 2 * h, n, false);

	glTranslated(0, 2 * h, 0);

	DrawRoller(0.1 * r, h, n, true);

	glRotatef(90, 0, 0, 1);
	glRotatef(180, 0, 1, 0);
	glTranslatef(-h / 2, -h / 2, 0);

	glTranslatef(0, h / 2, 0);
	glRotatef(gamma, 0, 0, 1);
	glTranslatef(0, -h / 2, 0);

	DrawRoller(0.1 * r, h, n, true);

	glTranslatef(0, h, 0);

	DrawRoller(0.1 * r, 3 * h, n, false);

	glTranslatef(0, 3 * h - 0.15 * h + slide * h, 0);

	DrawRoller(0.1 * r, 3 * h, n, false);

	glTranslatef(0, 3 * h, 0);
	glRotatef(180, 0, 1, 0);

	DrawRoller(0.1 * r, h, n, true);

	glRotatef(-90, 0, 0, 1);
	glTranslatef(-h / 2, 0, 0);
	glRotatef(180, 0, 1, 0);
	glTranslatef(0, -0.1 * r, 0);

	glTranslatef(0, h / 2, 0);
	glRotatef(beta, 0, 0, 1);
	glTranslatef(0, -h / 2, 0);

	DrawCarouselPart(r, h, n, alpha);

	glPopMatrix();
}

void CGLRenderer::DrawCourousel(float r, float h, int n, float alpha, float beta, float gamma, float slide, float delta, int nParts)
{
	glPushMatrix();

	DrawRoller(0.5 * r, h, 20, false);

	glTranslatef(0, h, 0);

	glRotatef(delta, 0, 1, 0);

	DrawRoller(r, h, 20, false);

	for (int i = 0; i < nParts; i++)
	{
		glPushMatrix();

		glRotatef(i * (360 / nParts), 0, 1, 0);
		glTranslatef(0.8 * r, 0, 0);
		glRotatef(180, 0, 1, 0);

		DrawCarouselPartWithArm(r, h, 20, alpha, beta, gamma, slide);

		glPopMatrix();
	}

	glPopMatrix();
}
