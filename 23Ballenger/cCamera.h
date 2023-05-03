#pragma once

#include "cScene.h"

#define STATE_FPS         0
#define STATE_TPS         1
#define STATE_TPS_FREE	  2

#define CAMERA_ZFAR   (TERRAIN_SIZE/sin(PI/4))
#define CAMERA_ZNEAR  0.01f
#define CAMERA_MAX_DISTANCE   10.0f
#define CAMERA_SPEED  (PI/180*0.2)
#define CAMERA_SMOOTHING_SPEED  0.01f

class cCamera
{
public:
	cCamera();
	~cCamera();
	
    void Refresh();
	void Update(cTerrain *Terrain,cLava *Lava,float player_x,float player_y,float player_z);
	void SetState(int s);
	int  GetState();
	void SetPos(float posx, float posy, float posz);

	float GetDistance();
    void GetDirectionVector(float &x, float &y, float &z);

    void  RotateYaw(float angle);
	void  SetYaw(float angle);
	float GetYaw();

	void  SetLastYaw(float angle);
	float GetLastYaw();

    void  RotatePitch(float angle);
	void  SetPitch(float angle);
	float GetPitch();

	float GetX();
	float GetY();
	float GetZ();

private:
	float x,y,z;   // Position
    float vx,vy,vz; // Direction vector of where we are looking at (van de 0 a 1)
	float yaw,last_yaw,pitch; // Various rotation angles en radianes
	float distance;
	float lambda;
	int state;

	float GetLavaLambda(float Py,float Qy,float height);
};
