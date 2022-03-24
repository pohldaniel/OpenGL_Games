#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Shader.h"

class Quad {

public:
	Quad() = default;
	Quad(bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f, short x = 0, short y = 0);
	~Quad();

	void render(unsigned int texture, bool array = false);
	void render();
	void setFlipped(bool flipped);

	const Vector2f &getScale() const;

private:

	void createBuffer(unsigned int& vao, bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f, short x = 0, short y = 0);
	
	unsigned int m_vao = 0;
	unsigned int m_vaoFlipped = 0;
	unsigned int m_quadVBO = 0;

	bool m_flippable = false;
	bool m_flipped = false;

	Vector2f m_scale;
};
#endif // __quadH__