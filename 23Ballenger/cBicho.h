#pragma once

#include "cScene.h"

class cBicho
{
public:
	cBicho(void);
	~cBicho(void);

	void  SetPos(float posx, float posy, float posz);
	void  SetX(float posx);
	void  SetY(float posy);
	void  SetZ(float posz);
    float GetX();
    float GetY();
    float GetZ();

	void  SetVel(float velx, float vely, float velz);
	void  SetVX(float velx);
	void  SetVY(float vely);
	void  SetVZ(float velz);
    float GetVX();
    float GetVY();
    float GetVZ();

	void  SetYaw(float ang);
	void  SetPitch(float ang);
    float GetYaw();
    float GetPitch();

	void SetState(int s);
	int  GetState();

	void SetMaxHealth(int max_h);
	void SetHealth(int h);
	int  GetMaxHealth();
	int  GetHealth();

private:
	float x, y, z;   // Position
	float vx, vy, vz;   // Velocity
	float yaw, pitch;   // rotation angles (by movement) in degrees
	int max_health,health,state;
};
