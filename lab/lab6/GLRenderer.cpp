#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#pragma comment(lib, "GL\\glut32.lib")
#include "CGLMaterial.h"
#include "CGLTexture.h"

CGLRenderer::CGLRenderer(void)
{
	this->angle_v = 0;
	this->angle_h = 0;
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
	glClearColor(0.8, 0.8, 0.8, 0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	CGLTexture::PrepareTexture(false);
	groundTex.LoadFileForm(CString("txt1.jpg"));//256x256
	truckTex.LoadFileForm(CString("txt4.jpeg")); //512x512

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle_h, 0.0, 0, 1.0);
	glRotatef(angle_v, 0, 1.0, 0.0);

	DrawAxis();
	DrawTruck();
	glTranslatef(0, -6, 0);
	DrawGround();

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
	gluPerspective(40, (double)w / (double)h, 1, 200);
	gluLookAt(84, 0, 0, 0, 12, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	truckTex.Relase();
	groundTex.Relase();
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::SetAngles(float angle_h, float angle_v)
{
	this->angle_v = angle_h;
	this->angle_h = angle_v;
}

void CGLRenderer::DrawGround()
{
	CGLTexture tex;
	float x = -768 / 8;
	float z = -768 / 8;
	int step = 256 / 8;

	groundTex.Select();


	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);

	glTexCoord2f(3, 0);
	glVertex3f(x, 0, z);
	glTexCoord2f(3, 3);
	glVertex3f(-x, 0, z);
	glTexCoord2f(0, 3);
	glVertex3f(-x, 0, -z);
	glTexCoord2f(0, 0);
	glVertex3f(x, 0, -z);

	glEnd();
	CGLTexture::BindDefault();
}

void CGLRenderer::DrawAxis()
{
	CGLTexture::BindDefault();
	float size = 64;
	glLineWidth(1.0);

	glBegin(GL_LINES);
	//Crvena X
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(size, 0.0, 0.0);

	//Zelena Y
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, size, 0.0);

	//Plava Z
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, size);
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
}

void CGLRenderer::DrawSide()
{
	float step = 4;
	//1/16=0.0625
	const float texStep = 0.0625;
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);

	glNormal3f(0, 0, 1);

	//prva kolona
	glTexCoord2f(0, 7 * texStep); glVertex3f(0, 0, 0);
	glTexCoord2f(0, 2 * texStep); glVertex3f(0, 5 * step, 0);

	//dijagonala
	glTexCoord2f(texStep, texStep); glVertex3f(step, 6 * step, 0);
	glTexCoord2f(texStep, 7 * texStep); glVertex3f(step, 0, 0);

	//druga kolona
	glTexCoord2f(texStep, 6 * texStep);glVertex3f(step, step, 0);
	glTexCoord2f(texStep, texStep);glVertex3f(step, 6 * step, 0);

	//dijagonala
	glTexCoord2f(2 * texStep, 0);glVertex3f(2 * step, 7 * step, 0);
	glTexCoord2f(2 * texStep, 5 * texStep);glVertex3f(2 * step, 2 * step, 0);

	//treca kolona
	glTexCoord2f(2 * texStep, 5 * texStep);glVertex3f(2 * step, 2 * step, 0);
	glTexCoord2f(2 * texStep, 0);glVertex3f(2 * step, 7 * step, 0);

	glTexCoord2f(3 * texStep, 0);glVertex3f(3 * step, 7 * step, 0);
	glTexCoord2f(3 * texStep, 5 * texStep);glVertex3f(3 * step, 2 * step, 0);


	//cetvrta kolona
	glTexCoord2f(3 * texStep, 5 * texStep);glVertex3f(3 * step, 2 * step, 0);
	glTexCoord2f(3 * texStep, 0);glVertex3f(3 * step, 7 * step, 0);
	//dijagonala
	glTexCoord2f(4 * texStep, 0);glVertex3f(4 * step, 7 * step, 0);
	glTexCoord2f(4 * texStep, 6 * texStep);glVertex3f(4 * step, 1 * step, 0);


	// sanduk
	// prednji deo
	glTexCoord2f(4 * texStep, 7 * texStep);glVertex3f(4 * step, 0, 0);
	glTexCoord2f(4 * texStep, 4 * texStep);glVertex3f(4 * step, 3 * step, 0);

	glTexCoord2f(6 * texStep, 4 * texStep);glVertex3f(6 * step, 3 * step, 0);
	glTexCoord2f(6 * texStep, 7 * texStep);glVertex3f(6 * step, 0, 0);

	
	//gornji deo
	glTexCoord2f(6 * texStep, 5 * texStep);glVertex3f(6 * step, 2 * step, 0);
	glTexCoord2f(6 * texStep, 4 * texStep);glVertex3f(6 * step, 3 * step, 0);

	glTexCoord2f(9 * texStep, 4 * texStep);glVertex3f(9 * step, 3 * step, 0);
	glTexCoord2f(9 * texStep, 5 * texStep);glVertex3f(9 * step, 2 * step, 0);

	//zadnji tocak
	//dijagonala tocak levo 7-6
	glTexCoord2f(5 * texStep, 5 * texStep);glVertex3f(6 * step, 1 * step, 0);
	glTexCoord2f(5 * texStep, 4 * texStep);glVertex3f(6 * step, 2 * step, 0);
	glTexCoord2f(6 * texStep, 4 * texStep);glVertex3f(7 * step, 2 * step, 0);
	glTexCoord2f(5 * texStep, 5 * texStep);glVertex3f(6 * step, 1 * step, 0);
	
	//dijagonala tocak desno
	glTexCoord2f(8 * texStep, 5 * texStep);glVertex3f(9 * step, 1 * step, 0);
	glTexCoord2f(7 * texStep, 4 * texStep);glVertex3f(8 * step, 2 * step, 0);
	glTexCoord2f(8 * texStep, 4 * texStep);glVertex3f(9 * step, 2 * step, 0);
	glTexCoord2f(8 * texStep, 5 * texStep);glVertex3f(9 * step, 1 * step, 0);
	
	//zadnji deo
	glTexCoord2f(9 * texStep, 7 * texStep);glVertex3f(9 * step, 0, 0);
	glTexCoord2f(9 * texStep, 4 * texStep);glVertex3f(9 * step, 3 * step, 0);

	glTexCoord2f(14 * texStep, 4 * texStep);glVertex3f(15 * step, 3 * step, 0);
	glTexCoord2f(14 * texStep, 7 * texStep);glVertex3f(15 * step, 0, 0);

	glEnd();
}
void CGLRenderer::DrawTruck()
{
	float step = 4;
	float width = 16;

	truckTex.Select();

	glPushMatrix();
	glTranslatef(-6.5 * step,0, 0);

	glTranslatef(0, 0, -width);
	DrawSide();
	glTranslatef(0, 0, 2 * width);
	DrawSide();
	glTranslatef(0, 0, -width);

	
	CGLTexture::BindDefault();
	glBegin(GL_QUADS);
	{
		//prednja strana
		glColor3f(0.29, 0.29, 0.18);
		glVertex3f(0, 0, width);
		glVertex3f(0, 0, -width);
		glVertex3f(0, 5 * step, -width);
		glVertex3f(0, 5 * step, width);
	
		// gornja kosina
		glColor3f(97 / 255., 97 / 255. , 61 / 255.);
		glVertex3f(0, 5 * step, -width);
		glVertex3f(0, 5 * step, width);
		glVertex3f(2 * step, 7 * step, width);
		glVertex3f(2 * step, 7 * step, -width);

		//krov
		glColor3f(136. / 255., 136. / 255. , 86. / 255.);
		glVertex3f(2 * step, 7 * step, width);
		glVertex3f(2 * step, 7 * step, -width);
		glVertex3f(4 * step, 7 * step, -width);
		glVertex3f(4 * step, 7 * step, width);

		//iza kabina
		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(4 * step, 7 * step, width);
		glVertex3f(4 * step, 7 * step, -width);
		glVertex3f(4 * step, 3 * step, -width);
		glVertex3f(4 * step, 3 * step, width);

		//sanduk gore
		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(4 * step, 3 * step, -width);
		glVertex3f(4 * step, 3 * step, width);
		glVertex3f(15 * step, 3 * step, width);
		glVertex3f(15 * step, 3 * step, -width);

		//sanduk iza
		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(15 * step, 3 * step, width);
		glVertex3f(15 * step, 3 * step, -width);
		glVertex3f(15 * step, 0, -width);
		glVertex3f(15 * step, 0, width);

		//dole iza sanduk
		glColor3f(0.29, 0.29, 0.18);
		glVertex3f(15 * step, 0, -width);
		glVertex3f(15 * step, 0, width);
		glVertex3f(9 * step, 0, width);
		glVertex3f(9 * step, 0, -width);

		//tocak dole segment 5 ima
		glVertex3f(9 * step, 0, -width);
		glVertex3f(9 * step, 0, width);
		glVertex3f(9 * step, 1 * step, width);
		glVertex3f(9 * step, 1 * step, -width);

		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(9 * step, 1 * step, -width);
		glVertex3f(9 * step, 1 * step, width);
		glVertex3f(8 * step, 2 * step, width);
		glVertex3f(8 * step, 2 * step, -width);

		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(8 * step, 2 * step, -width);
		glVertex3f(8 * step, 2 * step, width);
		glVertex3f(7 * step, 2 * step, width);
		glVertex3f(7 * step, 2 * step, -width);

		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(7 * step, 2 * step, -width);
		glVertex3f(7 * step, 2 * step, width);
		glVertex3f(6 * step, 1 * step, width);
		glVertex3f(6 * step, 1 * step, -width);

		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(6 * step, 1 * step, -width);
		glVertex3f(6 * step, 1 * step, width);
		glVertex3f(6 * step, 0, width);
		glVertex3f(6 * step, 0, -width);

		//ispod
		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(6 * step, 0, -width);
		glVertex3f(6 * step, 0, width);
		glVertex3f(4 * step, 0, width);
		glVertex3f(4 * step, 0, -width);


		//tocak segmenti 
		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(4 * step, 0, -width);
		glVertex3f(4 * step, 0, width);
		glVertex3f(4 * step, 1 * step, width);
		glVertex3f(4 * step, 1 * step, -width);

		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(4 * step, 1 * step, -width);
		glVertex3f(4 * step, 1 * step, width);
		glVertex3f(3 * step, 2 * step, width);
		glVertex3f(3 * step, 2 * step, -width);

		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(3 * step, 2 * step, -width);
		glVertex3f(3 * step, 2 * step, width);
		glVertex3f(2 * step, 2 * step, width);
		glVertex3f(2 * step, 2 * step, -width);

		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(2 * step, 2 * step, -width);
		glVertex3f(2 * step, 2 * step, width);
		glVertex3f(1 * step, 1 * step, width);
		glVertex3f(1 * step, 1 * step, -width);

		glColor3f(136. / 255., 136. / 255., 86. / 255.);
		glVertex3f(1 * step, 1 * step, -width);
		glVertex3f(1 * step, 1 * step, width);
		glVertex3f(1 * step, 0, width);
		glVertex3f(1 * step, 0, -width);

		//ispod napred
		glColor3f(97 / 255., 97 / 255., 61 / 255.);
		glVertex3f(1 * step, 0, -width);
		glVertex3f(1 * step, 0, width);
		glVertex3f(0, 0, width);
		glVertex3f(0, 0, -width);

	}
	glEnd();
	truckTex.Select();

	//tockovi
	glPushMatrix();
	glTranslatef(2.5 * step, 0, -width + 2);
	DrawWheel();

	glTranslatef(5 * step, 0, 0);
	DrawWheel();
	glPopMatrix();

	glTranslatef(2.5 * step, 0, width - 2);
	DrawWheel();
	glTranslatef(5 * step, 0, 0);
	DrawWheel();

	glPopMatrix();
	CGLTexture::BindDefault();
	glPushMatrix();

	//sfera gore
	glTranslatef(11, 4 * 4.5, 0);
	glScalef(1.8f, 0.6f, 1);
	glRotatef(180, 0, 1, 0);
	CGLTexture::BindDefault();  
	glColor3f(1.0f, 1.0f, 1.0f);
	truckTex.Select();

	DrawSphere(12);

	glPopMatrix();


}

void CGLRenderer::DrawWheelSide()
{
	const float RAD = 3.14 / 180;
	const float step = RAD * (360. / 16.);
	const float texStep = 0.0625;
	const float radius = 6;
	const float texRadius = 0.0625 * 1.5;

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	//glTexCoord2f(7 * texStep, 1.5 * texStep);
	glTexCoord2f(7 * texStep, 1.5 * texStep);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= 16; i++) {
		glTexCoord2f(7 * texStep + texRadius * cos(step * i), 1.5 * texStep + texRadius * sin(step * i));
		//glTexCoord2f(6 * texStep + texRadius * cos(step * i), 1.5 * texStep + texRadius * sin(step * i));
		glVertex3f(radius * cos(step * i), radius * sin(step * i), 0);
	}
	glEnd();
}

void CGLRenderer::DrawWheel()
{
	const float RAD = 3.14 / 180;
	const float step = RAD * (360. / 16.);
	const float radius = 6;

	glPushMatrix();
	glTranslatef(0, 0, -2);
	DrawWheelSide();
	glTranslatef(0, 0, 4);
	DrawWheelSide();
	glTranslatef(0, 0, -2);
	glBegin(GL_QUADS);
	for (int i = 0; i <= 16; i++) {
		glVertex3f(radius * cos(step * i), radius * sin(step * i), 2);
		glVertex3f(radius * cos(step * i), radius * sin(step * i), -2);
		glVertex3f(radius * cos(step * (i + 1)), radius * sin(step * (i + 1)), -2);
		glVertex3f(radius * cos(step * (i + 1)), radius * sin(step * (i + 1)), 2);
	}
	glEnd();
	glPopMatrix();

}


void CGLRenderer::DrawSphere(float radius)
{
	float texStep = 0.0625;

	const float RAD = 3.14 / 180;
	float redovi = (180 * RAD) / 8;
	float kolone = (360 * RAD) / 16;

	int k1 = 16;
	bool flag = false;
	glBegin(GL_QUADS);
	for (int i = -4; i < 4; i++)//16
	{
		for (int j = 0; j <= 16; j++)
		{
			glNormal3f(cos(i * redovi) * cos(j * kolone), sin(i * redovi), cos(i * redovi) * sin(j * kolone));
			glTexCoord2f(16-j * texStep, k1 * texStep);
			glVertex3f(radius * cos(i * redovi) * cos(j * kolone), radius * sin(i * redovi), radius * cos(i * redovi) * sin(j * kolone));

			glNormal3f(cos((i + 1) * redovi) * cos(j * kolone), sin((i + 1) * redovi), cos((i + 1) * redovi) * sin(j * kolone));
			glTexCoord2f(16-j * texStep, (k1 - 1) * texStep);
			glVertex3f(radius * cos((i + 1) * redovi) * cos(j * kolone), radius * sin((i + 1) * redovi), radius * cos((i + 1) * redovi) * sin(j * kolone));

			glNormal3f(cos((i + 1) * redovi) * cos((j + 1) * kolone), sin((i + 1) * redovi), cos((i + 1) * redovi) * sin((j + 1) * kolone));
			glTexCoord2f(16-(j + 1) * texStep, (k1 - 1) * texStep);
			glVertex3f(radius * cos((i + 1) * redovi) * cos((j + 1) * kolone), radius * sin((i + 1) * redovi), radius * cos((i + 1) * redovi) * sin((j + 1) * kolone));

			glNormal3f(cos(i * redovi) * cos((j + 1) * kolone), sin(i * redovi), cos(i * redovi) * sin((j + 1) * kolone));
			glTexCoord2f(16-(j+ 1) * texStep, k1 * texStep);
			glVertex3f(radius * cos(i * redovi) * cos((j + 1) * kolone), radius * sin(i * redovi), radius * cos(i * redovi) * sin((j + 1) * kolone));
		}
		k1 -= 1;
	}

	glEnd();



}
