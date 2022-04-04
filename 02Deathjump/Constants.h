#ifndef __constantsH__
#define __constantsH__
#include "Vector.h"
#include "AssetManger.h"
#include "Texture.h"
#include "Shader.h"
#include "CharacterSet.h"
#include "SoundDevice.h"
#include "SoundBuffer.h"
#include "MusicBuffer.h"
#include "SoundEffectsPlayer.h"
#include "Spritesheet.h"

#define DEBUGCOLLISION 0
#define DEBUG 0

#define PHYSICS_STEP 0.01f
#define WIDTH 1600
#define HEIGHT 900

namespace Globals{
	enum Controlls {		
		KEY_W = 1,
		KEY_A = 2,
		KEY_S = 4,
		KEY_D = 8,		
		KEY_SPACE = 16,
		KEY_ESCAPE = 32,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	extern float offset;
	extern unsigned char pKeyBuffer[256];
	extern unsigned long CONTROLLS;
	extern unsigned long CONTROLLSHOLD;

	typedef struct { long x; long y;} POINT;
	extern POINT cursorPosScreen;

	typedef struct { float x; float y; float z; } POINTF;

	extern POINTF cursorPosNDC;
	extern POINTF cursorPosEye;

	extern bool lMouseButton;

	extern Matrix4f projection;
	extern Matrix4f invProjection;

	extern AssetManagerStatic<Texture> textureManager;
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Spritesheet> spritesheetManager;
	//extern AssetManager<CharacterSetOld> fontManagerOld;
	extern AssetManager<CharacterSet> fontManager;

	extern AssetManager<SoundBuffer> soundManager;
	extern AssetManager<MusicBuffer> musicManager;

	extern float bestTime;
	extern float musicVolume;
	extern float soundVolume;
}

#endif
