#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Vector.h"

class Quad {

public:

	Quad();
	Quad(const Vector3f& position, const Vector3f& size);
	Quad(bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f,  short x = 0, short y = 0);
	~Quad();

	void draw(unsigned int texture, bool array = false);
	void drawRaw();
	void drawRawInstanced(unsigned int instanceCount);
	void setFlipped(bool flipped);

	void setPosition(const Vector3f& position);
	void setSize(const Vector3f& size);

	const Vector3f& getPosition() const;
	const Vector3f& getSize() const;

	void createBuffer(const Vector3f& position, const Vector3f& size);

private:

	void createBuffer(unsigned int& vao, bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f, short x = 0, short y = 0);
	void createBuffer();
	
	void mapBuffer();

	unsigned int m_vao = 0;
	unsigned int m_vaoFlipped = 0;
	unsigned int m_vbo = 0;

	bool m_flippable = false;
	bool m_flipped = false;

	Vector3f m_position;
	Vector3f m_size;

};
#endif // __quadH__