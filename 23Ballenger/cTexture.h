#pragma once


#include "Globals.h"

class cTexture {

public:
	cTexture(void);
	~cTexture(void);

	bool Load(char *filename,int type = GL_RGBA,int wraps = GL_REPEAT,int wrapt = GL_REPEAT,
			  int magf = GL_LINEAR,int minf = GL_LINEAR,bool mipmap = true);
	int  GetID();
	void GetSize(int *w,int *h);

private:
	GLuint id;
	int width;
	int height;
};
