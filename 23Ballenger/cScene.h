#pragma once

#include "cData.h"
#include "cTerrain.h"
#include "cSkybox.h"
#include "cLava.h"
#include "cShader.h"

class cScene
{
public:
	cScene(void);
	virtual ~cScene(void);

	void LoadLevel(int level, cTerrain *Terrain, cLava *Lava, float zfar);
	void Draw(cData *Data, cTerrain *Terrain, cLava *Lava, cShader *Shader, Coord playerPos);

private:
	cSkybox Skybox;
};
