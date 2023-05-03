#pragma once

#include "Globals.h"
#include "cShader.h"
#include "cModel.h"
#include "cData.h"

#define LEVITATION_SPEED 2.0f
#define BEACON_MIN_RADIUS 0.75f
#define BEACON_HEIGHT 140.0f

class cKey
{
public:
	cKey();
	~cKey();
	void  DrawLevitating(cShader *Shader, cModel *Model, cData *Data, float dist);
	void  DrawPicked(float playerx,float playery, float playerz, float camera_yaw, cModel *Model, cData *Data, cShader *Shader);
	void  DrawDeployed(float holex,float holey, float holez, float yaw, cModel *Model, cData *Data, cShader *Shader);
	void  SetPos(float posx, float posy, float posz);
	float GetX();
    float GetY();
    float GetZ();
	void  Deploy();
	bool  IsDeployed();

private:
	float x,y,z; //posicion inicial
	float ang;
	bool deployed;
};