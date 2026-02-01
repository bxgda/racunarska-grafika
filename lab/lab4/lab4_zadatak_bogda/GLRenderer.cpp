#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

// boje
#define COLOR_BG		0.47, 0.0, 0.00 
#define COLOR_GRID		0.50, 0.75, 1.00
#define COLOR_AXIS_X	1.00, 0.00, 0.00
#define COLOR_AXIS_Y	0.00, 1.00, 0.00
#define COLOR_AXIS_Z	0.00, 0.00, 1.00
#define COLOR_POT		0.00, 0.80, 0.00  
#define COLOR_CACTUS	1.00, 0.60, 0.00  
#define COLOR_JOINT		1.00, 1.00, 1.00
#define COLOR_YELLOW	1.00, 1.00, 0.00  

CGLRenderer::CGLRenderer(void)
{
	m_hrc = NULL;

	// inicijalni polozaj kamere
	m_cameraR = 15.0;
	m_cameraAngleXY = 0.0;
	m_cameraAngleXZ = 20.0; // malo podignuta kamera

	m_yellowPartAngle = 0.0; // pocetni ugao rotacije dela figure

	UpdateCameraPosition();
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

	glClearColor(COLOR_BG, 0.0);
	glEnable(GL_DEPTH_TEST);  //z buffer kako bi crtao objekte po redu, a ne obrnuto

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glLoadIdentity();

	// Postavljanje kamere
	gluLookAt(
		m_eyex, m_eyey, m_eyez,	// gde je oko
		0.0, 4.0, 0.0,			// gde gleda (centar figure, malo podignut)
		0.0, 1.0, 0.0			// up vektor
	);

	// crtanje pomocnih elemenata
	DrawAxis(10.0);
	DrawGrid(10, 10, 10, 10);

	// crtanje figure
	DrawFigure(m_yellowPartAngle);

	glFlush();

	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (double)w / (double)h, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	double alphaStep = 180.0 / nSegAlpha;
	double betaStep = 360.0 / nSegBeta;

	for (double beta = 0; beta < 360; beta += betaStep)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (double alpha = -90; alpha <= 90 + alphaStep; alpha += alphaStep)
		{
			// prva tacka (trenutni beta)
			double radAlpha = ToRad(alpha);
			double radBeta = ToRad(beta);

			double x = r * cos(radAlpha) * cos(radBeta);
			double y = r * sin(radAlpha);
			double z = r * cos(radAlpha) * sin(radBeta);

			glNormal3d(x / r, y / r, z / r);
			glVertex3d(x, y, z);

			// druga tacka (sledeci beta)
			radBeta = ToRad(beta + betaStep);

			x = r * cos(radAlpha) * cos(radBeta);
			y = r * sin(radAlpha);
			z = r * cos(radAlpha) * sin(radBeta);

			glNormal3d(x / r, y / r, z / r);
			glVertex3d(x, y, z);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCylinder(double h, double r1, double r2, int nSeg)
{
	double angleStep = 360.0 / nSeg;

	// donja osnova
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0, 0, 0);
		for (double angle = 0; angle <= 360; angle += angleStep)
		{
			double rad = ToRad(angle);
			glVertex3d(r1 * cos(rad), 0, r1 * sin(rad));
		}
	}
	glEnd();

	// gornja osnova
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0, h, 0);
		for (double angle = 0; angle <= 360; angle += angleStep)
		{
			double rad = ToRad(angle);
			glVertex3d(r2 * cos(rad), h, r2 * sin(rad));
		}
	}
	glEnd();

	// omotac
	glBegin(GL_TRIANGLE_STRIP);
	{
		for (double angle = 0; angle <= 360; angle += angleStep)
		{
			double rad = ToRad(angle);

			glVertex3d(r2 * cos(rad), h, r2 * sin(rad));
			glVertex3d(r1 * cos(rad), 0, r1 * sin(rad));
		}
	}
	glEnd();
}

void CGLRenderer::DrawCone(double h, double r, int nSeg)
{
	double angleStep = 360.0 / nSeg;

	// osnova
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0, 0, 0);
		for (double angle = 0; angle <= 360; angle += angleStep)
		{
			double rad = ToRad(angle);
			glVertex3d(r * cos(rad), 0, r * sin(rad));
		}
	}
	glEnd();

	// omotac
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0, h, 0); // vrh kupe
		for (double angle = 0; angle <= 360; angle += angleStep)
		{
			double rad = ToRad(angle);
			glNormal3d(cos(rad), 0, sin(rad));
			glVertex3d(r * cos(rad), 0, r * sin(rad));
		}
	}
	glEnd();
}

void CGLRenderer::DrawAxis(double width)
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{	
		// X osa
		glColor3f(COLOR_AXIS_X);
		glVertex3d(0, 0, 0);
		glVertex3d(width, 0, 0);
		// Y osa
		glColor3f(COLOR_AXIS_Y);
		glVertex3d(0, 0, 0);
		glVertex3d(0, width, 0);
		// Z osa
		glColor3f(COLOR_AXIS_Z);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, width);
	}
	glEnd();
	glLineWidth(1.0);
}

void CGLRenderer::DrawGrid(double width, double height, int nSegW, int nSegH)
{
	double stepW = width / nSegW;
	double stepH = height / nSegH;

	double startW = -width / 2.;
	double startH = -height / 2.;

	glColor3f(COLOR_GRID);
	glBegin(GL_LINES);
	{
		// linije paralelne sa Z osom
		for (int i = 0; i <= nSegW; i++)
		{
			glVertex3d(startW + i * stepW, 0, startH);
			glVertex3d(startW + i * stepW, 0, startH + height);
		}
		// linije paralelne sa X osom
		for (int i = 0; i <= nSegH; i++)
		{
			glVertex3d(startW, 0, startH + i * stepH);
			glVertex3d(startW + width, 0, startH + i * stepH);
		}
	}
	glEnd();
}

void CGLRenderer::DrawFigure(double angle)
{
	// dimenzije
	double potHeight = 1.3;
	double potR1 = 1, potR2 = 1.25;
	double cactusR = 0.45;
	double jointR = 0.3;
	int nSeg = 8;
	int nSegSphere = 50;


	glColor3f(COLOR_POT);
	DrawCylinder(potHeight, potR1, potR2, nSeg);

	glTranslated(0, potHeight, 0);
	DrawCylinder(0.6, potR2 + 0.2, potR2 + 0.2, nSeg);

	glTranslated(0, 0.6, 0);

	glRotated(angle, 1, 0, 0);

	glColor3f(COLOR_YELLOW);
	DrawCylinder(potHeight, cactusR, cactusR, nSeg);

	glTranslated(0, potHeight + jointR, 0);
	glColor3f(COLOR_JOINT);
	DrawSphere(jointR, nSegSphere, nSegSphere); 


	NacrtajValjak(45, potHeight, cactusR, nSeg, jointR, nSegSphere);
	NacrtajKupu(-45, potHeight, cactusR, nSeg, jointR, nSegSphere);

	NacrtajValjak(0, potHeight, cactusR, nSeg, jointR, nSegSphere);

	glTranslated(0, potHeight + 2 * jointR, 0);
	glColor3f(COLOR_JOINT);

	glPushMatrix();

	NacrtajKupu(45, potHeight, cactusR, nSeg, jointR, nSegSphere);

	glRotated(45, 1, 0, 0);
	glTranslated(0, potHeight + 2 * jointR, 0);

	NacrtajKupu(45, potHeight, cactusR, nSeg, jointR, nSegSphere);
	NacrtajKupu(-45, potHeight, cactusR, nSeg, jointR, nSegSphere);

	glPopMatrix();

	NacrtajKupu(-45, potHeight, cactusR, nSeg, jointR, nSegSphere);
	glRotated(-45, 1, 0, 0);
	glTranslated(0, potHeight + 2 * jointR, 0);

	NacrtajKupu(45, potHeight, cactusR, nSeg, jointR, nSegSphere);
	NacrtajKupu(0, potHeight, cactusR, nSeg, jointR, nSegSphere);
	NacrtajValjak(-45, potHeight, cactusR, nSeg, jointR, nSegSphere);
}

void CGLRenderer::NacrtajValjak(double angleDeg, double h, double r, int nSeg, double jointR, int nSegSphere)
{
	glPushMatrix();

	if (angleDeg != 0.0)
		glRotated(angleDeg, 1, 0, 0);

	glTranslated(0, jointR, 0);
	glColor3f(COLOR_CACTUS);
	DrawCylinder(h, r, r, nSeg);
	glTranslated(0, h + jointR, 0);
	glColor3f(COLOR_JOINT);
	DrawSphere(jointR, nSegSphere, nSegSphere);

	glPopMatrix();
}

void CGLRenderer::NacrtajKupu(double angleDeg, double h, double r, int nSeg, double jointR, int nSegSphere)
{
	glPushMatrix();

	if (angleDeg != 0.0)
		glRotated(angleDeg, 1, 0, 0);

	glTranslated(0, jointR, 0);
	glColor3f(COLOR_CACTUS);
	DrawCone(h, r, nSeg);
	glTranslated(0, h + jointR, 0);
	glColor3f(COLOR_JOINT);
	DrawSphere(jointR, nSegSphere, nSegSphere);

	glPopMatrix();
}

void CGLRenderer::RotateView(double dXY, double dXZ)
{
	m_cameraAngleXY += dXY;
	m_cameraAngleXZ += dXZ;

	// ogranicenje elevacije da se kamera ne prevrne
	if (m_cameraAngleXZ > 90) m_cameraAngleXZ = 90;
	if (m_cameraAngleXZ < -90) m_cameraAngleXZ = -90;

	UpdateCameraPosition();
}

void CGLRenderer::ZoomView(double dR)
{
	m_cameraR += dR;
	if (m_cameraR < 2.0) m_cameraR = 2.0;		// minimalni zoom
	if (m_cameraR > 50.0) m_cameraR = 50.0;		// maksimalni zoom
	UpdateCameraPosition();
}

void CGLRenderer::UpdateCameraPosition()
{
	double radXY = ToRad(m_cameraAngleXY);
	double radXZ = ToRad(m_cameraAngleXZ);

	m_eyex = m_cameraR * cos(radXZ) * cos(radXY);
	m_eyey = m_cameraR * sin(radXZ);
	m_eyez = m_cameraR * cos(radXZ) * sin(radXY);
}

void CGLRenderer::RotateYellowPart(double dAngle)
{
	m_yellowPartAngle += dAngle;
}
