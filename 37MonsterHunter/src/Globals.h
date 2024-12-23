#pragma once
#include <memory>
#include <engine/Clock.h>
#include <engine/MeshObject/Shape.h>
#include <engine/sound/SoundBuffer.h>
#include <engine/sound/MusicBuffer.h>
#include <engine/AssetManger.h>
#include <engine/Shader.h>
#include <engine/Texture.h>
#include <engine/CharacterSet.h>
#include <engine/Spritesheet.h>
#include <Physics/Physics.h>

#include <engine/animationNew/Animation.h>
#include <engine/animation/AssimpAnimation.h>

#define WIDTH 1280
#define HEIGHT 720

#define DEBUG 1
#define DEVBUILD 1

#define PHYSICS_STEP 0.0166666666666667f
//#define PHYSICS_STEP 0.0125f
//#define PHYSICS_STEP 0.0083333333333333f

#define UPDATE_STEP 0.0166666666666667f
//#define UPDATE_STEP 0.0083333333333333f

namespace Globals {
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shape> shapeManager;
	extern AssetManager<CharacterSet> fontManager;
	extern AssetManager<Spritesheet> spritesheetManager;
	extern AssetManager<SoundBuffer> soundManager;
	extern AssetManager<MusicBuffer> musicManager;
	extern AssetManager<AssimpAnimation> animationManager;
	extern AssetManager<Animation> animationManagerNew;
	extern std::unique_ptr<Physics> physics;
	extern Clock clock;

	extern unsigned int lightUbo;
	extern const unsigned int lightBinding;

	extern float soundVolume;
	extern float musicVolume;
}