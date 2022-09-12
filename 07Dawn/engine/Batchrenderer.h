#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include <array>

#include "Extension.h"
#include "Shader.h"
#include "Camera.h"

class Batchrenderer {

public:
	struct Vertex {
		std::array<float, 4> posTex;
		unsigned int frame;
	};

	void init(size_t size = 400);
	void shutdown();
	void drawBuffer();
	void addQuad(Vector4f position, Vector4f texCoord, unsigned int frame);

	void addQuad(std::vector<Vertex> particles);
	void setCamera(const Camera& camera);
	void setShader(Shader* shader);
	static Batchrenderer& get();

private:

	//Batchrenderer(const Camera& camera);
	Batchrenderer() = default;
	~Batchrenderer();

	size_t m_maxQuad = 0;
	size_t m_maxVert = 0;
	size_t m_maxIndex = 0;
	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo= 0;

	uint32_t indexCount = 0;

	Vertex* buffer;
	Vertex* bufferPtr;

	//const Camera& m_cameraR;
	const Camera* m_camera;
	
	Shader *m_shader;
	
	static Batchrenderer s_instance;	
};
#endif
