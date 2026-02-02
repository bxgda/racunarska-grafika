#include "stdafx.h"
#include "CGLMaterial.h"

CGLMaterial::CGLMaterial()
{
	PostaviAmbijent(0.2f, 0.2f, 0.2f, 1.0f);
	PostaviDifuznu(0.2, 0.8, 0.2, 1.0);
	PostaviSpekularnu(1.0, 1.0, 1.0, 1.0);
	PostaviEmisivnu(0.0, 0.0, 0.0, 1.0);
	PostaviShininess(64);
}

CGLMaterial::~CGLMaterial()
{
}

void CGLMaterial::IzaberiMaterijal()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void CGLMaterial::PostaviAmbijent(float r, float g, float b, float a)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
	ambient[3] = a;
}

void CGLMaterial::PostaviDifuznu(float r, float g, float b, float a)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = a;
}

void CGLMaterial::PostaviSpekularnu(float r, float g, float b, float a)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
	specular[3] = a;
}

void CGLMaterial::PostaviEmisivnu(float r, float g, float b, float a)
{
	emissive[0] = r;
	emissive[1] = g;
	emissive[2] = b;
	emissive[3] = a;
}

void CGLMaterial::PostaviShininess(float s)
{
	if (s < 0)
		shininess = 0;
	else if (s > 128)
		shininess = 128;
	else
		shininess = s;
}
