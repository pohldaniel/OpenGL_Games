#ifndef				__GAME_CAMERA_H__
#define				__GAME_CAMERA_H__

#include <math.h>
#include <stdio.h>
#include <engine/Vector.h>

//#define	START_ANGLE		-0.628f
#define	START_ANGLE	 -M_PI_4

class CameraNew {
public:
	float camx;
	float camy;
	float camz;

	float pointx;
	float pointy;
	float pointz;

	float lookx;
	float looky;
	float lookz;

	float distance;
	float height;
	float angle;
	float initx;
	float initz;

	float yfactor;

	float hres, vres;
	float breadth, length;

	float m_speed;

public:

	CameraNew(void);
	~CameraNew();

	void Initialize();

	void Move();
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void Resize();

	void Rotate(float a);
	void SetHeight(float d);
	int ConvertCoordinates(int x, int y, float &, float &);
	int ScrollOver(float x, float y);
};

#endif