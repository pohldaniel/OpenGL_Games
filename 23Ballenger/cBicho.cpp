#include "cBicho.h"

cBicho::cBicho(void)
{
	SetVel(0.0,0.0,0.0);
	yaw = 0.0f; pitch = 0.0f;
}
cBicho::~cBicho(void){}

void cBicho::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;
}
void cBicho::SetX(float posx)
{
	x = posx;
}
void cBicho::SetY(float posy)
{
	y = posy;
}
void cBicho::SetZ(float posz)
{
	z = posz;
}
float cBicho::GetX()
{
    return x;
}
float cBicho::GetY()
{
    return y;
}
float cBicho::GetZ()
{
    return z;
}

void cBicho::SetVel(float velx, float vely, float velz)
{
	vx = velx;
	vy = vely;
	vz = velz;
}
void cBicho::SetVX(float velx)
{
	vx = velx;
}
void cBicho::SetVY(float vely)
{
	vy = vely;
}
void cBicho::SetVZ(float velz)
{
	vz = velz;
}
float cBicho::GetVX()
{
    return vx;
}
float cBicho::GetVY()
{
    return vy;
}
float cBicho::GetVZ()
{
    return vz;
}

void cBicho::SetYaw(float ang)
{
	yaw = ang;
}
void cBicho::SetPitch(float ang)
{
	pitch = ang;
}
float cBicho::GetYaw()
{
	return yaw;
}
float cBicho::GetPitch()
{
	return pitch;
}

void cBicho::SetState(int s)
{
	state = s;
}
int cBicho::GetState()
{
	return state;
}

void cBicho::SetMaxHealth(int max_h)
{
	max_health = max_h;
}
int cBicho::GetMaxHealth()
{
	return max_health;
}
void cBicho::SetHealth(int h)
{
	health = h;
}
int cBicho::GetHealth()
{
	return health;
}