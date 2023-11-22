#ifndef __constantsH__
#define __constantsH__


#include "engine\Vector.h"
#include "engine\AssetManger.h"
#include "engine\Texture.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"

#define DEBUGCOLLISION 1
#define DEBUG 1
#define FIXEDUPDATE 0

#define UPDATE_STEP 0.0166666f
#define PHYSICS_STEP 0.01f
#define WIDTH 1024
#define HEIGHT 768

#define TILE_WIDTH 96
#define TILE_HEIGHT 48
namespace Globals{
	enum Controlls {		
		KEY_W = 1,
		KEY_A = 2,
		KEY_S = 4,
		KEY_D = 8,		
		KEY_SPACE = 16,
		KEY_ESCAPE = 32,
		KEY_Q = 64,
		KEY_RETURN = 128,
		KEY_UP = 256,
		KEY_DOWN = 512,
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

	extern AssetManager<Texture> textureManager;
	extern AssetManager<Shader> shaderManager;
	extern AssetManager<Spritesheet> spritesheetManager;
}

#endif
