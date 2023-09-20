#ifndef __pixelbufferH__
#define __pixelbufferH__

#include "Texture.h"
#include "Spritesheet.h"
#include "Framebuffer.h"
#include "Shader.h"

class Pixelbuffer {

public:

	Pixelbuffer();
	~Pixelbuffer();

	void create();
	void create(unsigned int width, unsigned int height);
	void cleanup();

private:

	unsigned int m_pbo = 0;
};

#endif 