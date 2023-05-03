#pragma once

#include "cTexture.h"

//Image array size
#define NUM_TEX		 15

//Image identifiers
enum {
	IMG_GRASS,
	IMG_ROCK,
	IMG_LAVA,
	IMG_SKYBOX,
	IMG_PLAYER,
	IMG_PLAYER_NMAP,
	IMG_CIRCLE_ON,
	IMG_CIRCLE_OFF,
	IMG_VORTEX,
	IMG_KEY,
	IMG_KEY_NMAP,
	IMG_PORTAL,
	IMG_PORTAL_NMAP,
	IMG_COLUMN,
	IMG_COLUMN_NMAP
};

class cData
{
public:
	cData(void);
	~cData(void);

	int  GetID(int img);
	void GetSize(int img, int *w, int *h);
	bool Load();

private:
	cTexture textures[NUM_TEX];
	bool LoadImage(int img,char *filename,int type = GL_RGBA);
};
