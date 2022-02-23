#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Shader.h"

class Quad {

public:

	Quad(float sizeX = 1.0f, float sizeY = 1.0f, float sizeTex = 1.0f);
	~Quad();

	void render(unsigned int texture);

private:

	void createBuffer();
	unsigned int m_vao;
	float m_sizeX = 1;
	float m_sizeY = 1;
	float m_sizeTex = 1;
};
#endif // __quadH__