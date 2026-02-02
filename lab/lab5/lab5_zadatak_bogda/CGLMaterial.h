#pragma once

#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"

class CGLMaterial
{
public:
	CGLMaterial();
	~CGLMaterial();

	void IzaberiMaterijal();

	void PostaviAmbijent(float r, float g, float b, float a);
	void PostaviDifuznu(float r, float g, float b, float a);
	void PostaviSpekularnu(float r, float g, float b, float a);
	void PostaviEmisivnu(float r, float g, float b, float a);
	void PostaviShininess(float s);

protected:
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;
};

