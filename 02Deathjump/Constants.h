#ifndef __constantsH__
#define __constantsH__


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
		KEY_Q = 1,
		KEY_W = 2,
		KEY_E = 4,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	extern float offset;
	extern unsigned char pKeyBuffer[256];
	extern unsigned long CONTROLLS;
	extern unsigned long CONTROLLSHOLD;
}

#endif
