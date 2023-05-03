#pragma once

#include "Globals.h"

#define FLOW_SPEED 2.0f
#define LAVA_HEIGHT_MAX 4.0f
#define LAVA_HEIGHT_MIN 1.0f

class cLava
{
public:
	cLava();
	~cLava();
	void Load(float terrain_size);
	void Draw(int tex_id);
	float GetHeight();
	float GetHeightMax();
	void Update();

private:
	int id_Lava;
	float height,ang;
	bool up;
};