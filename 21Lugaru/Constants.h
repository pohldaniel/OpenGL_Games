#pragma once

#include "engine\AssetManger.h"
#include "engine\Shader.h"
#include "engine\Texture.h"

#define WIDTH 1024
#define HEIGHT 768
#define DEBUG 1

#define PHYSICS_STEP 0.0166666f

namespace Globals {
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern bool drawUi;
}
