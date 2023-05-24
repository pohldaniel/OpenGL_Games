#ifndef _LINE_H
#define _LINE_H

#include<vector>
#include "Vector.h"

class Line {

public:

	Line();
	~Line();

	void addToBuffer(const Vector3f& begin, const Vector3f& end);
	void drawRaw() const;
	void cleanup();
	void addVec4Attribute(const std::vector<Vector4f>& values);

private:

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	unsigned int m_drawCount = 0;
	unsigned int m_vboAdd1 = 0;

	std::vector<Vector3f> m_positions;
	std::vector<unsigned int> m_indexBuffer;
};

#endif