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

#define PHYSICS_STEP	0.01f

#define WIDTH 1600
#define HEIGHT 900

//Level
#define MAP_TILE_WIDTH 50
#define MAP_TILE_HEIGHT 50
#define TILE_WIDTH 50
#define LEVEL_WIDTH 100
#define LEVEL_HEIGHT 12
#define WIDTH2 2 * WIDTH

//Charackter
#define CHARACTER_WIDTH 50
#define CHARACTER_HEIGHT 66
#define CHARACTER_TILE_WIDTH 50
#define CHARACTER_TILE_HEIGHT 100

//Enemy
#define ENEMY_WIDTH 50
#define ENEMY_HEIGHT 50
#define ENEMY_TILE_WIDTH 50
#define ENEMY_TILE_HEIGHT 50

namespace Globals{
	enum Controlls {		
		KEY_W = 1,
		KEY_A = 2,
		KEY_S = 4,
		KEY_D = 8,
		KEY_Q = 16,
		KEY_E = 32,
		KEY_SPACE = 64,
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
	extern AssetManager<CharacterSet> fontManager;


	extern AssetManager<SoundBuffer> soundManager;
	extern AssetManager<MusicBuffer> musicManager;

	extern SoundEffectsPlayer effectsPlayer;
}

#endif
