#pragma once

#include "cBicho.h"
#include "cCamera.h"

#define PLAYER_SPEED  0.008f
#define PLAYER_JUMP_SPEED  0.4f
#define FRICTION   0.05f
#define ELASTICITY   0.5f //porcentaje de rango [0.0, 1.0]
#define MAX_MOVEMENT 0.8f
#define RADIUS   0.5f

class cPlayer: public cBicho
{
public:
	cPlayer();
	~cPlayer();

	void Draw(cData *Data,cCamera *Camera,cLava *Lava,cShader *Shader);
	void SetFade(bool b);

private:
	bool fade;
};