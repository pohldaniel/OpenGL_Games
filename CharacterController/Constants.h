#ifndef __constantsH__
#define __constantsH__
#include <Box2D\Box2D.h>

#include "engine\Vector.h"

#define DEBUGCOLLISION 1
#define DEBUG 1

#define PHYSICS_STEP 0.01f
#define WIDTH 1600
#define HEIGHT 1500

#define MAX_JUMP_HEIGHT	5.f
#define JUMP_APEX_TIME	0.5f

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

	extern Matrix4f projection;
	extern b2World * world;
}

#endif
