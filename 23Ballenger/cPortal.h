#pragma once

#include "Globals.h"
#include "cModel.h"
#include "cShader.h"
#include "cData.h"

#define PORTAL_SIDE  3.0f
#define PORTAL_SPEED 2.0f

class cPortal
{
public:
	cPortal();
	~cPortal();
	void Draw(cData *Data, bool activated, cShader *Shader, cModel *Model);
	void SetPos(float posx, float posy, float posz);
	bool InsidePortal(float px, float py, float pz, float r);
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetReceptorX(int i) const;
	float GetReceptorY(int i) const;

private:
	float x,y,z;
	float ang;
	std::pair<float,float> receptors[5];
};