#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Shader.h"

class Quad {

public:

	Quad(bool flippable = false, float sizeX = 1.0f, float sizeY = 1.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f);
	~Quad();

	void render(unsigned int texture, bool array = false);
	void setFlipped(bool flipped);

private:

	void createBuffer(unsigned int& vao, bool flippable, float sizeX = 1, float sizeY = 1, float sizeTexX = 1, float sizeTexY = 1, short x = 0, short y = 0);
	unsigned int m_vao = 0;
	unsigned int m_vaoFlipped = 0;
	unsigned int m_quadVBO = 0;

	bool m_flippable = false;
	bool m_flipped = false;
};
#endif // __quadH__