#pragma once
#include <memory>

#include <engine/sound/SoundBuffer.h>
#include <engine/sound/MusicBuffer.h>
#include <engine/Clock.h>

#define WIDTH 1280
#define HEIGHT 720

#define DEBUG 1
#define DEVBUILD 1

#define PHYSICS_STEP 0.0166666666666667f
//#define PHYSICS_STEP 0.0125f
//#define PHYSICS_STEP 0.0083333333333333f

//#define UPDATE_STEP 0.0166666666666667f
#define UPDATE_STEP 0.0083333333333333f

namespace Globals {
	extern Clock clock;

	extern unsigned int lightUbo;
	extern const unsigned int lightBinding;

	extern float soundVolume;
	extern float musicVolume;
}