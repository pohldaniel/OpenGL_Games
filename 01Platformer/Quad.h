#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Shader.h"

class Quad {

public:

	Quad(float sizeX = 1.0f, float sizeY = 1.0f, float sizeTex = 1.0f);
	Quad(std::string vertex, std::string fragment, float sizeX = 1.0f, float sizeY = 1.0f, float sizeTex = 1.0f);
	~Quad();

	void render(unsigned int texture);
	void render(unsigned int texture, int layer);
	void render2(unsigned int texture);

	Shader* getShader() const;

private:

	void createBuffer();
	unsigned int m_vao;
	float m_sizeX = 1;
	float m_sizeY = 1;
	float m_sizeTex = 1;
	std::vector<float> m_vertex;

	Shader *m_quadShader;
};


#endif // __quadH__