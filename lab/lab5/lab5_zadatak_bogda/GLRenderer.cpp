#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define TO_RAD 3.14 / 180.0f
#define TO_DEG 180.0f / 3.14

CGLRenderer::CGLRenderer(void)
{
	// boja za tirkizne prstene
	plaviMaterijal.PostaviAmbijent(0, 0, 1, 1);
	plaviMaterijal.PostaviDifuznu(0, 0, 1, 1);
	plaviMaterijal.PostaviSpekularnu(1, 1, 1, 1);
	plaviMaterijal.PostaviShininess(16);

	// boja za ljubicaste prstene
	crveniMaterijal.PostaviAmbijent(1, 0, 0, 1);
	crveniMaterijal.PostaviDifuznu(1, 0, 0, 1);
	crveniMaterijal.PostaviSpekularnu(1, 1, 1, 1);
	crveniMaterijal.PostaviShininess(16);

	// boja osnove
	materijalOsnove.PostaviAmbijent(0.15, 0.15, 0.15, 0);
	materijalOsnove.PostaviDifuznu(0.7, 0.7, 0.7, 0);
	materijalOsnove.PostaviSpekularnu(0.5, 0.9, 0.5, 0);
	materijalOsnove.PostaviShininess(64);

	//boja sobe
	materijalSobe.PostaviAmbijent(0.1, 0.1, 0.1, 1.0);
	materijalSobe.PostaviDifuznu(0.8, 0.8, 0.8, 1.0);
	materijalSobe.PostaviSpekularnu(0.2, 0.2, 0.2, 1.0);
	materijalSobe.PostaviEmisivnu(0.1, 0.1, 0.1, 1.0);
	materijalSobe.PostaviShininess(0);

	// plavo svetlo - loptica
	plavoSvetlo.PostaviAmbijent(0, 0, 1, 1);
	plavoSvetlo.PostaviDifuznu(0, 0, 1, 0);
	plavoSvetlo.PostaviSpekularnu(0, 0, 1, 1);
	plavoSvetlo.PostaviEmisivnu(0, 0, 1, 1);
	plavoSvetlo.PostaviShininess(0);

	// tirkizno svetlo - loptica
	tirkiznoSvetlo.PostaviAmbijent(0, 1, 1, 1);
	tirkiznoSvetlo.PostaviDifuznu(0, 1, 1, 0);
	tirkiznoSvetlo.PostaviSpekularnu(0, 1, 1, 1);
	tirkiznoSvetlo.PostaviEmisivnu(0, 1, 1, 1);
	tirkiznoSvetlo.PostaviShininess(0);

	// zuto svetlo - loptica
	zutoSvetlo.PostaviAmbijent(1, 1, 0, 1);
	zutoSvetlo.PostaviDifuznu(1, 1, 0, 0);
	zutoSvetlo.PostaviSpekularnu(1, 1, 0, 1);
	zutoSvetlo.PostaviEmisivnu(1, 1, 0, 1);
	zutoSvetlo.PostaviShininess(0);
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
	
	glClearColor(0, 0, 0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_NORMALIZE);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Postavljanje kamere
	gluLookAt(
		okoKamereX, okoKamereY, okoKamereZ,	// gdje je oko
		0.0, 6.0, 0.0,						// gdje gleda (centar figure, malo podignut)
		0.0, 1.0, 0.0						// up vektor
	);

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------


	PostaviAmbijentalnoSvetlo();

	glEnable(GL_LIGHTING);

	PlavoSvetlo();
	TirkiznoSvetlo();
	ZutoSvetlo();

	glPushMatrix();

	materijalSobe.IzaberiMaterijal();
	NacrtajSobu();

	materijalOsnove.IzaberiMaterijal();
	NacrtajPostolje();

	NacrtajVazu();

	glPopMatrix();

	// ----------------------------------------------------------

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
	gluPerspective(65, (double)w / (double)h, 1, 100);
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

void CGLRenderer::RotirajPogled(float deltaXY, float deltaXZ)
{
	ugaoKamereXY += deltaXY;
	ugaoKamereXZ += deltaXZ;

	if(ugaoKamereXZ > 90)
		ugaoKamereXZ = 90;

	if (ugaoKamereXZ < -90)
		ugaoKamereXZ = -90;

	PromeniPozicijuKamere();
}

void CGLRenderer::ZumirajPogled(float deltaUdaljenosti)
{
	udaljenostKamere += deltaUdaljenosti;

	if (udaljenostKamere < 2.0f)		// minimalni zoom
		udaljenostKamere = 2.0f;

	if (udaljenostKamere > 50.0f)		// maksimalni zoom
		udaljenostKamere = 50.0f;

	PromeniPozicijuKamere();
}

void CGLRenderer::PromeniPozicijuKamere()
{
	// na osnovu promenjenih parametara izracunavaju se potrebne koordinate za samu kameru

	okoKamereX = udaljenostKamere * cos(ugaoKamereXZ * TO_RAD) * sin(ugaoKamereXY * TO_RAD);
	okoKamereY = udaljenostKamere * sin(ugaoKamereXZ * TO_RAD);
	okoKamereZ = udaljenostKamere * cos(ugaoKamereXZ * TO_RAD) * cos(ugaoKamereXY * TO_RAD);
}

void CGLRenderer::NacrtajSferu(float r)
{
	int step = 3;
	glBegin(GL_QUAD_STRIP);
	for (int alpha = -90; alpha < 90; alpha += step) {
		for (int beta = 0; beta <= 360; beta++) {
			float normX = cos(beta * C_PI_RAD) * cos(alpha * C_PI_RAD);
			float normY = sin(alpha * C_PI_RAD);
			float normZ = sin(beta * C_PI_RAD) * cos(alpha * C_PI_RAD);

			glNormal3f(normX, normY, normZ);

			glVertex3f(r * normX, r * normY, r * normZ);

			float normX1 = cos(beta * C_PI_RAD) * cos((alpha + step) * C_PI_RAD);
			float normY1 = sin((alpha + step) * C_PI_RAD);
			float normZ1 = sin(beta * C_PI_RAD) * cos((alpha + step) * C_PI_RAD);
			glNormal3f(normX1, normY1, normZ1);

			glVertex3f(r * normX1, r * normY1, r * normZ1);
		}
	}
	glEnd();
}

void CGLRenderer::NacrtajValjak(float radius, float height, int sides)
{
	float step = (2 * C_PI) / sides;
	float t_height = height / 2.0;

	//Baza 1
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < sides; i++) {
		glNormal3f(0, 1, 0);
		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));
	}
	glEnd();

	//Baza 2
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < sides; i++) {
		glNormal3f(0, -1, 0);
		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
	}
	glEnd();


	//Omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= sides; i++) {
		glNormal3f(get_x(1, i * step), 0, get_y(1, i * step));
		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));

		glNormal3f(get_x(1, i * step), 0, get_y(1, i * step));
		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
	}
	glEnd();
}

void CGLRenderer::NacrtajPrizmu(float radius, float height, int sides)
{
	float t_height = height / 2.0;

	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	float step = (2 * C_PI) / sides;
	//Baza 1
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < sides; i++) {
		glNormal3f(0, 1, 0);
		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));
	}
	glEnd();

	//Baza 2
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < sides; i++) {
		glNormal3f(0, -1, 0);
		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
	}
	glEnd();

	//Omotac

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= sides; i++) {
		glNormal3f(get_x(1, i * step), 0, get_y(1, i * step));
		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));

		glNormal3f(get_x(1, i * step), 0, get_y(1, i * step));
		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
	}
	glEnd();

	glPopMatrix();
}

void CGLRenderer::NacrtajPrsten(double r1, double r2, double h, int nSeg)
{
	//traka izmedju r1 i r2 sa visinom h
	double step = 2 * C_PI / nSeg;	 //ugao koraka oko kruga

	int v = r2 > r1 ? -1 : 1;
	double r = std::abs(r1 - r2);
	double L = sqrt(r * r + h * h);
	double ny = r / L;
	double nr = h / L;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		glNormal3f(nr * cos(step * i), ny * v, nr * sin(step * i));

		glVertex3f(cos(step * i) * r1, 0, sin(step * i) * r1);												
		glVertex3f(cos(step * i) * r2, h, sin(step * i) * r2);
	}
	glEnd();

	if (normale) {
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);						//uzima verteks po verteks i spaja ih u linije par po par
		glColor3f(0, 1, 0);						//boja normala
		for (int i = 0; i < nSeg + 1; i++) {	//veci broj segmenata daje vise glatku povrsinu

			//tacka dole
			glVertex3f(cos(step * i) * r1, 0, sin(step * i) * r1);
			glVertex3f(cos(step * i) * r1 + cos(step * i) * nr, ny * v, sin(step * i) * r1 + sin(step * i) * nr);
			//tacka gore
			glVertex3f(cos(step * i) * r2, h, sin(step * i) * r2);
			glVertex3f(cos(step * i) * r2 + cos(step * i) * nr, h + ny * v, sin(step * i) * r2 + sin(step * i) * nr);
		}

		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void CGLRenderer::NacrtajSobu()
{
	float a = 40;
	float korak = 1;

	// donji deo sobe
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	for (float i = -a / 2; i < a / 2; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j + korak);
			glVertex3f(i + korak, 0, j + korak);
			glVertex3f(i + korak, 0, j);
		}
	}
	glEnd();

	// gornji deo sobe
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	for (float i = -a / 2; i < a / 2; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(i, a, j);
			glVertex3f(i, a, j + korak);
			glVertex3f(i + korak, a, j + korak);
			glVertex3f(i + korak, a, j);
		}
	}
	glEnd();

	// prednji deo sobe
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	for (float i = 0; i < a; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(a / 2, i, j);
			glVertex3f(a / 2, i, j + korak);
			glVertex3f(a / 2, i + korak, j + korak);
			glVertex3f(a / 2, i + korak, j);
		}
	}
	glEnd();

	// zadnji deo sobe
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	for (float i = 0; i < a; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(-a / 2, i, j);
			glVertex3f(-a / 2, i, j + korak);
			glVertex3f(-a / 2, i + korak, j + korak);
			glVertex3f(-a / 2, i + korak, j);
		}
	}
	glEnd();

	// levi deo sobe
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	for (float i = 0; i < a; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(j, i, -a / 2);
			glVertex3f(j + korak, i, -a / 2);
			glVertex3f(j + korak, i + korak, -a / 2);
			glVertex3f(j, i + korak, -a / 2);
		}
	}
	glEnd();

	// desni deo sobe
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	for (float i = 0; i < a; i += korak)
	{
		for (float j = -a / 2; j < a / 2; j += korak)
		{
			glVertex3f(j, i, a / 2);
			glVertex3f(j + korak, i, a / 2);
			glVertex3f(j + korak, i + korak, a / 2);
			glVertex3f(j, i + korak, a / 2);
		}
	}
	glEnd();
}

void CGLRenderer::NacrtajPostolje()
{
	glClipPlane(GL_CLIP_PLANE0, new double[4] { 0, 1, 0, 0 });
	glEnable(GL_CLIP_PLANE0);
	glColor3f(0.8, 0.8, 0.8);
	NacrtajSferu(3);

	glTranslatef(0, 3, 0);
	glColor3f(0.8, 0.8, 0.8);
	NacrtajValjak(1, 4);

	glTranslatef(0, 2, 0);
	glColor3f(0.8, 0.8, 0.8);
	NacrtajPrizmu(5, 1);
	glDisable(GL_CLIP_PLANE0);
}

void CGLRenderer::NacrtajVazu()
{
	glPushMatrix();
	glTranslatef(0, 0.25, 0);
	glScalef(0.6, 0.6, 0.6);

	float visina = 1;
	float radius4 = 4;
	float radius3 = 3;
	float radius2 = 2;

	bool boja = true;

	int radius[28] = { 4,3,3,2,2,2,2,2,2,3,3,2,2,3,3,4,4,3,3,4,4,3,3,2,2,3,3,4};

	for (int i = 0; i < 28; i += 2) 
	{
		/*if (boja) 
			plaviMaterijal.IzaberiMaterijal();
		else 
			crveniMaterijal.IzaberiMaterijal();*/

		// svaki prsten je stavljen na sivu boju kako bi bolje moglo da se vidi osvetljenje jer ovako se 
		// mnogo preklapaju boje i svetla i ne moze lepo da se vidi efekat svetla
		// inace bi ovaj gore if else trebao da bude koriscen

		materijalOsnove.IzaberiMaterijal();
		
		boja = !boja;
		glTranslatef(0, visina, 0);
		NacrtajPrsten(radius[i], radius[i + 1], visina, 50);
	}

	glPopMatrix();
}

void CGLRenderer::PlavoSvetlo()
{
	float light_ambient[] = { 0.0, 0.0, 1.0, 1.0 };   
	float light_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };   
	float light_specular[] = { 0.0, 0.0, 1.0, 1.0 }; 
	GLfloat light_position[] = { -20, 15, 0, 1 };


	//Boja i intenzitet svetlosti
	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 1.0f);

	glLightfv(GL_LIGHT3, GL_POSITION, light_position);
	GLfloat spot_direction[] = { 1, 0, 0.0 };
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
	//Slabljenje
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0);

	//Usmerenje izvora
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50);
	//glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 0);

	if (this->svetlo1) {
		glPushMatrix();
		plavoSvetlo.IzaberiMaterijal();
		glTranslatef(-18, 15, 0);
		NacrtajSferu(1.5);
		glPopMatrix();
		glEnable(GL_LIGHT3);
	}
	else
		glDisable(GL_LIGHT3);
}

void CGLRenderer::TirkiznoSvetlo()
{
	float light_ambient[] = { 0.0, 1.0, 1.0, 1.0 };
	float light_diffuse[] = { 0.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 0.0, 1.0, 1.0, 1.0 };

	GLfloat light_position[] = { 20, 15, 0,1 };

	// boja i intenzitet svetlosti
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1.0f);

	glLightfv(GL_LIGHT2, GL_POSITION, light_position);

	GLfloat spot_direction[] = { -1, 0, 0 };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	// slabljenje
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);

	// usmerenje izvora
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35);

	if (this->svetlo2) {
		glPushMatrix();
		tirkiznoSvetlo.IzaberiMaterijal();
		glTranslatef(18, 15, 0);
		NacrtajSferu(1);
		glPopMatrix();
		glEnable(GL_LIGHT2);
	}
	else
		glDisable(GL_LIGHT2);
}

void CGLRenderer::ZutoSvetlo()
{
	float light_ambient[] = { 1.0, 1.0, 0.0, 1.0 };   
	float light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };  
	float light_specular[] = { 1.0, 1.0, 0.0, 1.0 }; 

	GLfloat light_position[] = { 0, 40, 0, 1 };

	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient); 
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse); 
	glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular); 

	glLightfv(GL_LIGHT4, GL_POSITION, light_position);			//pozicija svetla

	GLfloat spot_direction[] = { 0, -1, 0.0 };

	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, spot_direction);	//pravac reflektora
	// slabljenje
	glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1);

	//Usmerenje izvora
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 20);
	//glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 7.0);

	if (this->svetlo3) {
		glPushMatrix();
		zutoSvetlo.IzaberiMaterijal();
		glTranslatef(0, 38, 0);
		NacrtajSferu(1.5);
		glPopMatrix();
		glEnable(GL_LIGHT4);
	}
	else
		glDisable(GL_LIGHT4);
}

void CGLRenderer::PostaviAmbijentalnoSvetlo()
{
	GLfloat ambijentalno_svetlo[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambijentalno_svetlo);

	GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };		// Ambijentalna komponenta izvora
	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };		// Jaka bela svetlost
	GLfloat light_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };		// Beli odsjaj

	GLfloat light_position[] = { 13.0f, 39.0f, 13.0f, 1.0f };

	// Podešavanje parametara za LIGHT1
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0f);

	
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(GL_LIGHT1);
}
