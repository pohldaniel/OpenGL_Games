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
#include "engine\sound\SoundBuffer.h"
#include "engine\sound\MusicBuffer.h"
#include "engine\sound\SoundEffectsPlayer.h"

//#define WIDTH 1600
//#define HEIGHT 900

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
	extern AssetManager<SoundBuffer> soundManager;
	extern AssetManager<MusicBuffer> musicManager;
	extern AssetManager<SoundEffectsPlayer> soundEffectsPlayer;
	extern bool enableWireframe;
	extern bool savingAllowed;
	extern bool isPaused;
	extern Clock clock;


	extern unsigned int viewUbo;
	extern const unsigned int viewBinding;

	extern float soundVolume;
	extern float musicVolume;
	extern bool applyDisplaymode;
	extern unsigned int width;
	extern unsigned int height;

	extern void setResolution(unsigned int width, unsigned int height);
	extern void useDisplaymode(bool flag);
	extern void setSoundVolume(float soundVolume);
	extern void setMusicVolume(float musicVolume);
}

