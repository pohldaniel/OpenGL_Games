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

//#define WIDTH 1600
//#define HEIGHT 900

#define WIDTH 1024
#define HEIGHT 768

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
	extern bool savingAllowed;
	extern Clock clock;


	extern unsigned int viewUbo;
	extern const unsigned int viewBinding;
}