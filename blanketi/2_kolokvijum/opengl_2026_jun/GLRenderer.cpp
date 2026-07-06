#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")
#define _USE_MATH_DEFINES
#include <math.h>

#include "DImage.h"

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
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1;
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
   	pfd.iPixelType = PFD_TYPE_RGBA;
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24;
   	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

   	m_hrc = wglCreateContext(pDC->m_hDC);

	if (!m_hrc) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClearColor(0.75, 0.75, 0.75, 1);
	glEnable(GL_DEPTH_TEST);

	teksturaLogo = LoadTexture("logo.bmp");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	UpdateCamera();

	// osvetljenje - pozicioni izvor svetlosti
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float pozicijaLight0[] = { 3.0f, 5.0f, 3.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, pozicijaLight0);

	float ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	// iscrtavanje scene
	DrawAxes();

	SetMetalMaterial();

	DrawKeychain(angInner, angRing, angSwing, teksturaLogo, true);

	glFlush();
	SwapBuffers(pDC->m_hDC);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
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

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glDeleteTextures(1, &teksturaLogo);

	//---------------------------------
	wglMakeCurrent(NULL,NULL);
	if(m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);

	glEnd();

	glEnable(GL_LIGHTING);
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}

GLuint CGLRenderer::LoadTexture(char* fileName)
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

void CGLRenderer::SetMetalMaterial()
{
	float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);

	float dif[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);

	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

	float sjaj = 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, sjaj);
}

void CGLRenderer::DrawRing(double r1, double r2, double h, int n)
{
	double korak = 2 * M_PI / n;

	// gornja strana
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);

		glNormal3f(0, 1, 0);
		glVertex3d(r2 * x, h / 2, r2 * z);
		glVertex3d(r1 * x, h / 2, r1 * z);
	}
	glEnd();

	// donja strana
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);

		glNormal3f(0, -1, 0);
		glVertex3d(r1 * x, -h / 2, r1 * z);
		glVertex3d(r2 * x, -h / 2, r2 * z);
	}
	glEnd();

	// spoljasnji omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);

		glNormal3f(x, 0, z);
		glVertex3d(r2 * x, h / 2, r2 * z);
		glVertex3d(r2 * x, -h / 2, r2 * z);
	}
	glEnd();

	// unutrasnji omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);

		glNormal3f(-x, 0, -z);
		glVertex3d(r1 * x, -h / 2, r1 * z);
		glVertex3d(r1 * x, h / 2, r1 * z);
	}
	glEnd();
}

void CGLRenderer::DrawDisk(double R, double h, int n)
{
	double korak = 2 * M_PI / n;

	// gornja baza
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3d(0, h / 2, 0);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		float u = 0.5f + 0.5f * (float)x;
		float v = 0.5f + 0.5f * (float)z;
		glTexCoord2f(u, v);
		glVertex3d(R * x, h / 2, R * z);
	}
	glEnd();

	// donja baza
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3d(0, -h / 2, 0);
	for (int i = n; i >= 0; i--)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		float u = 0.5f + 0.5f * (float)x;
		float v = 0.5f + 0.5f * (float)z;
		glTexCoord2f(u, v);
		glVertex3d(R * x, -h / 2, R * z);
	}
	glEnd();

	// omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);

		glNormal3f(x, 0, z);
		glTexCoord2f((float)i / n, 0);
		glVertex3d(R * x, h / 2, R * z);
		glTexCoord2f((float)i / n, 1);
		glVertex3d(R * x, -h / 2, R * z);
	}
	glEnd();
}

void CGLRenderer::DrawKeychain(double angInner, double angRing, double angSwing, GLuint tex, bool useTex)
{
	glDisable(GL_TEXTURE_2D);

	// nepomicna alka
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	DrawRing(0.44, 0.56, 0.08, 30);
	glPopMatrix();

	glPushMatrix();

	// usica
	double alkaR = (0.44 + 0.56) / 2.0;
	glTranslated(0, -alkaR, 0);
	glRotated(angSwing, 0, 0, 1);

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	DrawRing(0.10, 0.17, 0.06, 20);
	glPopMatrix();

	// vertikalni nosac
	double usicaR = (0.10 + 0.17) / 2.0;
	glTranslated(0, -usicaR - 0.26 / 2.0, 0);
	DrawDisk(0.07, 0.26, 10);

	// pomeramo se na centar prstenova
	double spoljniR2 = 1.12;
	glTranslated(0, -0.26 / 2.0 - spoljniR2, 0);

	glRotated(90, 0, 1, 0);

	// rotacija oba prstena oko vertikalne ose
	glRotated(angRing, 0, 1, 0);

	// spoljni prsten
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	DrawRing(0.86, 1.12, 0.12, 30);
	glPopMatrix();

	glRotated(angInner, 0, 1, 0);

	// unutrasnji prsten
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	DrawRing(0.64, 0.80, 0.10, 30);
	glPopMatrix();

	// centralni disk sa teksturom
	if (useTex)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	DrawDisk(0.66, 0.06, 30);
	glPopMatrix();

	if (useTex)
		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}
