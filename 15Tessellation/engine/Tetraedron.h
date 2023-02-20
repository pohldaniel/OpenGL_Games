#pragma once
#ifndef __tetraedronH__
#define __tetraedronH__

#include <vector>
#include "Shader.h"

class Tetraedron {

public:
	Tetraedron();
	Tetraedron(Vector3f& position, Vector3f size);
	~Tetraedron();

	void draw(unsigned int texture);
	void drawRaw();

	void setPosition(const Vector3f &position);
	void setSize(const Vector3f &size);


	const Vector3f &getPosition() const;
	const Vector3f &getSize() const;

private:

	void createBuffer();
	void createBufferNormal();
	void createBufferTex();
	void createBufferTexNormal();
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	Vector3f m_position;
	Vector3f m_size;

};
#endif // __cubeH__