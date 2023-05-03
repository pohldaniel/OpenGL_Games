#include "cCamera.h"

cCamera::cCamera()
{
	yaw = 0.0f;
	last_yaw = 0.0f;
	pitch = -PI/2;
	distance = 0.4f;
	lambda = 1.0f;
	state = STATE_TPS;
}
cCamera::~cCamera(){}

void cCamera::Refresh()
{
	vx = cos(yaw) * cos(pitch);
	vy = sin(pitch);
	vz = sin(yaw) * cos(pitch);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(x, y, z,     x + vx, y + vy, z + vz,    0,1,0);
}

float cCamera::GetLavaLambda(float Py,float Qy,float height)
{
	float Vy = Qy - Py;
	float D = -height;
	if(Vy == 0.0f) return 1.0f;
	float lambda = -(Py + D) / Vy;
	if(lambda < 0.0f || lambda > 1.0f) return 1.0f;
	return lambda;
}

void cCamera::Update(cTerrain *Terrain, cLava *Lava, float player_x,float player_y,float player_z)
{
	vx = cos(yaw) * cos(pitch);
	vy = sin(pitch);
	vz = sin(yaw) * cos(pitch);

	//camera sigue a player
	if(GetState() == STATE_FPS) SetPos(player_x, player_y + 0.5, player_z);
	else
	{
		float newLambda = Terrain->GetSegmentIntersectionLambda(player_x,player_y,player_z, vx,vy,vz, CAMERA_MAX_DISTANCE);
		newLambda = std::min( newLambda , GetLavaLambda(player_y,player_y - CAMERA_MAX_DISTANCE*vy,Lava->GetHeight()) );

		if(lambda < newLambda)
		{
			lambda += CAMERA_SMOOTHING_SPEED;
			if(lambda > newLambda) lambda = newLambda;
		}
		else lambda = newLambda;

		distance = CAMERA_MAX_DISTANCE*lambda*0.85;

		SetPos(player_x - distance*vx,player_y - distance*vy,player_z - distance*vz);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(x, y, z,     x + vx, y + vy, z + vz,    0,1,0);
}

void cCamera::SetState(int s)
{
	state = s;
}
int  cCamera::GetState()
{
	return state;
}

void cCamera::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;

	Refresh();
}

float cCamera::GetDistance()
{
	return distance;
}

void cCamera::GetDirectionVector(float &x, float &y, float &z)
{
    x = vx;
    y = vy;
    z = vz;
}

void cCamera::RotateYaw(float angle)
{
	yaw += angle;

	Refresh();
}
void cCamera::SetYaw(float angle)
{
	yaw = angle;
}
float cCamera::GetYaw()
{
	return yaw;
}

void cCamera::SetLastYaw(float angle)
{
	last_yaw = angle;
}
float cCamera::GetLastYaw()
{
	return last_yaw;
}

void cCamera::RotatePitch(float angle)
{
    const float limit = 89.0 * PI / 180.0;

	pitch += angle;

    if(pitch < -limit)
        pitch = -limit;

    if(pitch > limit)
        pitch = limit;

	Refresh();
}
void cCamera::SetPitch(float angle)
{
	pitch = angle;
}
float cCamera::GetPitch()
{
	return pitch;
}

float cCamera::GetX()
{
	return x;
}
float cCamera::GetY()
{
	return y;
}
float cCamera::GetZ()
{
	return z;
}