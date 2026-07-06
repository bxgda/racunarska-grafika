#pragma once

#include "GL\gl.h"

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);

	bool CreateGLContext(CDC* pDC);
	void PrepareScene(CDC* pDC);
	void Reshape(CDC* pDC, int w, int h);
	void DrawScene(CDC* pDC);
	void DestroyScene(CDC* pDC);

	void DrawRing(double r1, double r2, double h, int n);
	void DrawDisk(double R, double h, int n);
	GLuint LoadTexture(char* fileName);
	void DrawKeychain(double angInner, double angRing, double angSwing, GLuint tex, bool useTex);

	void DrawAxes();
	void UpdateCamera();
	void SetMetalMaterial();

	double angInner = 0;
	double angRing = 0;
	double angSwing = 0;

	float cameraAlpha = 0;
	float cameraBeta = 0;
	float cameraDist = 8;

protected:
	HGLRC m_hrc;

	GLuint teksturaLogo;
};
