#pragma once

#include "engine\Vector.h"
#include "engine\AssetManger.h"
#include "engine\Shader.h"
#include "engine\Texture.h"

#define WIDTH 800
#define HEIGHT 600
#define DEBUG 1

#define PHYSICS_STEP 0.0166666f

namespace Globals {

	extern Matrix4f projection;
	extern Matrix4f invProjection;

	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
}