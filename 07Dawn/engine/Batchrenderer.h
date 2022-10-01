#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include <array>

#include "Extension.h"
#include "Shader.h"
#include "Camera.h"

class Batchrenderer {
	friend class Fontrenderer;

public:
	struct Vertex {
		std::array<float, 4> posTex;
		std::array<float, 4> color;
		unsigned int frame;
	};

	void init(size_t size = 400);
	void shutdown();
	void drawBuffer(bool updateView = true);
	void addQuad(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0, bool updateView = true);

	void setCamera(const Camera& camera);
	void setShader(Shader* shader);

	void bindTexture(unsigned int texture, bool isTextureArray = false);
	void unbindTexture(bool isTextureArray = false);
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

	//const Camera& m_camera;
	const Camera* m_camera;
	
	Shader *m_shader;
	
	static Batchrenderer s_instance;	
};
#endif
