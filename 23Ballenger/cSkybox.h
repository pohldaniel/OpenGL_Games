#pragma once

#include "Globals.h"

class cSkybox
{
public:
	cSkybox();
	~cSkybox();
	void Load(float zfar);
	void Draw(int tex_id,float player_x,float player_y,float player_z);

private:
	int id_Skybox;
};