#pragma once

#include <engine/MeshObject/Shape.h>
//#include <engine/sound/SoundBuffer.h>
//#include <engine/sound/MusicBuffer.h>
#include <engine/AssetManger.h>
#include <engine/Shader.h>
#include <engine/Texture.h>
#include <engine/CharacterSet.h>
#include <engine/Spritesheet.h>
#include <Physics/Physics.h>

#define WIDTH 1600
#define HEIGHT 900

#define DEBUG 1
#define DEVBUILD 1
#define DEBUGCOLLISION 0

#define PHYSICS_STEP 0.0166666666666667f
//#define PHYSICS_STEP 0.0083333333333333f

#define TILE_WIDTH 96
#define TILE_HEIGHT 48

namespace Globals {
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shape> shapeManager;
	extern AssetManager<CharacterSet> fontManager;
	extern AssetManager<Spritesheet> spritesheetManager;
	//extern AssetManager<SoundBuffer> soundManager;
	//extern AssetManager<MusicBuffer> musicManager;
	extern Physics *physics;

	extern float soundVolume;
	extern float musicVolume;
}
