#pragma once
#include <vector>

class SlicedCube {

public:

	SlicedCube();
	SlicedCube(int width, int height, int depth);
	~SlicedCube();

	void drawRaw();


	int getWidth();
	int getHeight();
	int getDepth();
	void create(int width, int height, int depth);
	unsigned int& getVao();
	void cleanup();

private:

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	int m_width, m_height, m_depth;

};