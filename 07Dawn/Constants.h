#pragma once

#include <map>
#include <vector>

#include "engine\Vector.h"
#include "engine\AssetManger.h"
#include "engine\Shader.h"
#include "engine\Texture.h"
#include "engine\Spritesheet.h"
#include "engine\CharacterSet.h"
#include "engine\Clock.h"

#include "Zone.h"


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
	extern AssetManager<CharacterSet> fontManager;

	extern bool enableWireframe;

	extern Clock clock;
	extern std::vector<TextWindow*> allTextWindows;
}