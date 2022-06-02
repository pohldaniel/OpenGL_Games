#ifndef __constantsH__
#define __constantsH__


#include "engine\Vector.h"
#include "engine\AssetManger.h"
#include "engine\Texture.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"

#define DEBUGCOLLISION 1
#define DEBUG 1
#define FIXEDUPDATE 0

#define UPDATE_STEP 0.0166666f
#define PHYSICS_STEP 0.01f
#define WIDTH 1600
#define HEIGHT 900

//#define TILE_WIDTH 64
//#define TILE_HEIGHT 80

#define TILE_WIDTH 96
#define TILE_HEIGHT 48

namespace Globals{
	
	extern Matrix4f projection;
	extern Matrix4f invProjection;

	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Spritesheet> spritesheetManager;
}

#endif
