#ifndef __pixelbufferH__
#define __pixelbufferH__

#include "Texture.h"

class Pixelbuffer {

public:

	Pixelbuffer();
	~Pixelbuffer();

	void create(unsigned int size, bool doubleBuffer = true);
	void mapData(const Texture& texture, unsigned char* data);
	void cleanup();

private:

	unsigned int m_pbo[2] = { 0 };

	bool read = true;
	bool write = false;
	bool m_doubleBuffer = true;
};

#endif 