#pragma once

#include "Extension.h"
#include "Vector.h"
#include "Shader.h"
#include "Camera.h"

#include "../ViewPort.h"

class Instancedrenderer {

public:
	struct Vertex {
		float posSize[4];
		float texPosSize[4];
		unsigned int frame;
	};

	void init(size_t size = 400);
	void shutdown();
	void addQuad(Vector4f position, Vector4f texCoord, unsigned int frame);
	void drawBuffer();
	void setShader(Shader* shader);

	static Instancedrenderer& get();

private:

	Instancedrenderer(const Camera& camera);
	~Instancedrenderer();

	size_t m_maxQuad = 0;
	size_t m_maxVert = 0;

	unsigned int m_vao = 0;
	unsigned int m_vboBase = 0;
	unsigned int m_vboInstance = 0;
	unsigned int m_ibo = 0;

	Vertex* buffer;
	Vertex* bufferPtr;
	std::vector<Vertex> instanceBuffer;

	const Camera& m_camera;

	Shader *m_shader;
	static Instancedrenderer s_instance;
	unsigned int m_instanceCount = 0;
};