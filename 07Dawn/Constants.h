#pragma once

#include <map>
#include <vector>

#include "engine\Vector.h"
#include "engine\AssetManger.h"
#include "engine\Shader.h"
#include "engine\Texture.h"
#include "engine\Spritesheet.h"

#include "Zone.h"
#include "Framesbase.h"

#define WIDTH 1600
#define HEIGHT 900
#define DEBUG 1

#define PHYSICS_STEP 0.0166666f

namespace Globals {

	extern Matrix4f projection;
	extern Matrix4f invProjection;
	extern Matrix4f orthographic;
	extern Matrix4f invOrthographic;

	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern AssetManager<Spritesheet> spritesheetManager;

	extern bool enableWireframe;

	extern std::map<std::string,Zone*> allZones;
	extern Zone* currentZone;

	extern void setCurrentZone(Zone* newCurZone);
	extern Zone* getCurrentZone();
	extern bool initPhase;
	extern std::vector<FramesBase*> activeFrames;

	extern unsigned int textureAtlas;
}