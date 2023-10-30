#ifndef __cubeH__
#define __cubeH__

#include <vector>
#include "Vector.h"

class Cube {

public:
	Cube();
	Cube(Vector3f& position, Vector3f size);
	~Cube();

	void draw(unsigned int texture);
	void drawRaw();

	void setPosition(const Vector3f &position);
	void setSize(const Vector3f &size);


	const Vector3f &getPosition() const;
	const Vector3f &getSize() const;


private:

	void createBuffer();
	void createBufferTexNormal();
	void createBufferTex();
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	Vector3f m_position;
	Vector3f m_size;

};
#endif // __cubeH__