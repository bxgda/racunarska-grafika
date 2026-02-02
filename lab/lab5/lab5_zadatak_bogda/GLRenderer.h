#pragma once
#include "CGLMaterial.h"

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

public:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	// moje promenljive i funkcije ----------------------------------

	const double C_PI = 3.14159265358979323846;
	const double C_PI_RAD = 3.14159265358979323846 / 180.0;

	CGLMaterial materijalSobe;
	CGLMaterial materijalOsnove;
	CGLMaterial plaviMaterijal;
	CGLMaterial crveniMaterijal;
	CGLMaterial plavoSvetlo;
	CGLMaterial tirkiznoSvetlo;
	CGLMaterial zutoSvetlo;

	bool svetlo1 = true;
	bool svetlo2 = true;
	bool svetlo3 = true;

	bool normale = true;

	float udaljenostKamere = 15.0f;		// udaljenost kamere od koordinatnog pocetka
	float ugaoKamereXY = 0.0f;			// ugao rotacije kamere oko Y ose
	float ugaoKamereXZ = 0.0f;			// ugao rotacije kamere oko X ose

	double okoKamereX = 0.0;			// koordinate tacke sa koje kamera gleda
	double okoKamereY = 0.0;
	double okoKamereZ = 0.0;

	void RotirajPogled(float deltaXY, float deltaXZ);
	void ZumirajPogled(float deltaUdaljenosti);
	void PromeniPozicijuKamere();

	void NacrtajSferu(float radius);
	void NacrtajValjak(float radius, float height = 0.3, int sides = 18);
	void NacrtajPrizmu(float radius, float height = 0.3, int sides = 4);
	void NacrtajPrsten(double r1, double r2, double h, int nSeg);

	void NacrtajSobu();
	void NacrtajPostolje();
	void NacrtajVazu();

	void PlavoSvetlo();
	void TirkiznoSvetlo();
	void ZutoSvetlo();

	void PostaviAmbijentalnoSvetlo();

	inline float get_x(float radius, float angle) { return radius * cosf(angle); }
	inline float get_y(float radius, float angle) { return radius * sinf(angle); }

};
