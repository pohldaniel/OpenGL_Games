#ifndef __constantsH__
#define __constantsH__

#define WIDTH 800
#define HEIGHT 600

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
		KEY_LEFT = 1,
		KEY_RIGHT = 2,
		KEY_UP = 4,
		KEY_DOWN = 8,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	extern float offset;
	extern unsigned char pKeyBuffer[256];
	extern unsigned long CONTROLLS;
}

#endif
