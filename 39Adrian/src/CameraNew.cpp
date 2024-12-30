#include <iostream>
#include "CameraNew.h"
#include "Application.h"

CameraNew::CameraNew(void){
		Initialize();
}

CameraNew::~CameraNew(){
}

void CameraNew::Initialize(void){

	height = 30.0f;
	distance = height * M_SQRT3;
	//distance = 50.0f;
	angle = START_ANGLE;

	pointx = 0.0f;
	pointy = 0.0f;
	pointz = 0.0f;

	lookx = 0.0f;
	looky = 1.0f;
	lookz = 0.0f;
	  
	initx = -780.0f;
	initz = 780.0f;

	breadth = 2000.0f;
	length = 2000.0f;

	hres = static_cast<float>(Application::Width);
	vres = static_cast<float>(Application::Height);

	camy = height;
	camx = distance * sin(angle) + initx;
	camz = distance * cos(angle) + initz;

	yfactor = -sqrt(1 + (distance * distance) / (height * height));

	m_speed = 5.0f;

	Move();
}

void CameraNew::Rotate(float a){
	angle = a;
	camx = distance * sin(angle) + initx;
	camz = distance * cos(angle) + initz;

	pointx = initx;
	pointz = initz;
}

void CameraNew::Move(void){
	camx = distance * sin(angle) + initx;
	camz = distance * cos(angle) + initz;

	pointx = initx;
	pointz = initz;
}

void CameraNew::MoveUp(void){
	float tmpx, tmpy;
	ConvertCoordinates((hres / 2.0), (vres / 2.0), tmpx, tmpy);
	if (tmpy > breadth / 2)
		return;

	initx += sin(angle) * m_speed;
	initz += cos(angle) * m_speed;
	Move();
}

void CameraNew::MoveDown(void){
	float tmpx, tmpy;
	ConvertCoordinates((hres / 2.0), (vres / 2.0), tmpx, tmpy);
	if (tmpy < -breadth / 2)
		return;

	initx -= sin(angle) * m_speed;
	initz -= cos(angle) * m_speed;
	Move();
}

void CameraNew::MoveLeft(void){
	float tmpx, tmpy;
	ConvertCoordinates((hres / 2.0), (vres / 2.0), tmpx, tmpy);
	if (tmpx > length / 2)
		return;

	initx += cos(angle) * m_speed;
	initz -= sin(angle) * m_speed;
	Move();
}

void CameraNew::MoveRight(void){
	float tmpx, tmpy;
	ConvertCoordinates((hres / 2.0), (vres / 2.0), tmpx, tmpy);
	if (tmpx < -length / 2)
		return;

	initx -= cos(angle) * m_speed;
	initz += sin(angle) * m_speed;
	Move();
}

void CameraNew::SetHeight(float d){
	height = d;

	camy = height;
	camx = distance * sin(angle) + initx;
	camz = distance * cos(angle) + initz;

	yfactor = -sqrt(1 + (distance * distance) / (height * height));
}

int CameraNew::ConvertCoordinates(int x, int y, float &x3, float &y3){
	float newx = x - (hres / 2.0);
	float newy = yfactor * ((vres / 2.0) - y);

	x3 = newx * cos(angle) + newy * sin(angle);
	y3 = newy * cos(angle) - newx * sin(angle);

	x3 += pointx;
	y3 += pointz;

	return 0;
}

int CameraNew::ScrollOver(float x, float y){
	initx = x;
	initz = y;
	Move();
	return 0;
}

void CameraNew::Resize() {
	hres = static_cast<float>(Application::Width);
	vres = static_cast<float>(Application::Height);
}