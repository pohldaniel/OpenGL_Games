#pragma once

#include "Globals.h"
#include "cShader.h"

#define CIRCLE_RADIUS 2.0f
#define AURA_HEIGHT 3.0f
#define HEIGHT_OFFSET 0.05f

class cRespawnPoint
{
public:
	cRespawnPoint();
	~cRespawnPoint();
	void Draw(int tex_id, bool activated, cShader *Shader);
	void Draw1(int tex_id);
	void Draw2(bool activated, cShader *Shader);


	void  SetPos(float posx, float posy, float posz);
	float GetX();
    float GetY();
    float GetZ();

private:
	float x,y,z;
};