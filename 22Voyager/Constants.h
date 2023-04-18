#pragma once

#include "engine/AssetManger.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Cubemap.h"

#define WIDTH 1024
#define HEIGHT 768
#define DEBUG 1

#define PHYSICS_STEP 0.0166666f

namespace Globals {
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shape> shapeManager;
	extern AssetManager<Cubemap> cubemapManager;
	extern bool drawUi;
}
