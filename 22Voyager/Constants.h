#pragma once

#include "engine/MeshObject/Shape.h"
#include "engine/sound/SoundBuffer.h"
#include "engine/sound/MusicBuffer.h"
#include "engine/AssetManger.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include "engine/CharacterSet.h"
#include "engine/Spritesheet.h"

#define WIDTH 1440
#define HEIGHT 900

#define DEBUG 1
#define DEBUG_MENU 0

#define PHYSICS_STEP 0.0166666f

namespace Globals {
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shape> shapeManager;
	extern AssetManager<CharacterSet> fontManager;
	extern AssetManager<Spritesheet> spritesheetManager;
	extern AssetManager<SoundBuffer> soundManager;
	extern AssetManager<MusicBuffer> musicManager;


	extern float soundVolume;
	extern float musicVolume;
}
