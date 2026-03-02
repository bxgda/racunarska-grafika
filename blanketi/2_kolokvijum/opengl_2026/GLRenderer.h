#pragma once
#include "DImage.h"

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);

	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

protected:
	HGLRC	 m_hrc;							//OpenGL Rendering Context 

public:
	void UpdateCamera();

	double cameraDist = 10;
	double cameraBeta = 7;
	double cameraAlpha = 0;

	UINT LoadTexture(char* fileName);

	UINT prostorija;
	UINT metal;

	void SetMaterial(float diffuse[]);
	void DrawRoller(float r, float h, int nR, bool bHalf);

	void DrawCarouselPart(float r, float h, int n, float alpha);

	float ugaoKorpa = 0;
	float ugao1 = 0;
	float ugao2 = 0;
	float ugao3 = 0;
	float slide = 0;
	int brojDelova = 5;

	void DrawCarouselPartWithArm(float r, float h, int n, float alpha, float beta, float gamma, float slide);
	void DrawCourousel(float r, float h, int n, float alpha, float beta, float gamma, float slide, float delta, int nParts);
};
