#pragma once

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


	void DrawSphere(double r, int nSegAlpha, int nSegBeta);
	void DrawCylinder(double h, double r1, double r2, int nSeg);
	void DrawCone(double h, double r, int nSeg);
	void DrawAxis(double width);
	void DrawGrid(double width, double height, int nSegW, int nSegH);

	void DrawFigure(double angle);

	// pomocne funkcije za crtanje segmenata (push/pop unutar)
	void NacrtajValjak(double angleDeg, double h, double r, int nSeg, double jointR, int nSegSphere);
	void NacrtajKupu(double angleDeg, double h, double r, int nSeg, double jointR, int nSegSphere);

	// kontrola kamere i interakcija
	void RotateView(double dXY, double dXZ);
	void ZoomView(double dR);
	void UpdateCameraPosition();

	// kontrola zutog elementa
	void RotateYellowPart(double dAngle);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	// parametri kamere (sferne koordinate)
	double m_cameraR;
	double m_cameraAngleXY; // ugao u horizontalnoj ravni
	double m_cameraAngleXZ; // ugao elevacije

	// izracunate koordinate oka
	double m_eyex, m_eyey, m_eyez;

	// parametar za rotaciju dela figure
	double m_yellowPartAngle;

	// konstante za konverziju
	const double PI = 3.14159265358979323846;
	inline double ToRad(double angle) { return angle * PI / 180.0; }
};
