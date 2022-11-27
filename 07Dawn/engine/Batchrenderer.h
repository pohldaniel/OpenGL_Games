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
		unsigned int sampler;
	};

	void init(size_t size = 400, bool drawSingle = false);
	void shutdown();
	void drawBuffer(bool updateView = true);
	void addQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u, unsigned int sampler = 0u, bool updateView = true);
	void drawSingleQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u, unsigned int sampler = 0u, bool updateView = true);
	void processQuad(bool updateView);
	void processSingleQuad(bool updateView);

	void setCamera(const Camera& camera);
	void setShader(Shader* shader);
	void updateModelMtx(const Matrix4f& mtx);

	void bindTexture(unsigned int texture, bool isTextureArray = false);
	void unbindTexture(bool isTextureArray = false);

	void bindTexture(unsigned int texture, bool isTextureArray, unsigned int unit);
	void unbindTexture(bool isTextureArray, unsigned int unit);
	void activeTexture(unsigned int unit);

	float(&getQuadPos())[8];
	float(&getTexPos())[8];
	float(&getColor())[4];
	unsigned int& getFrame();
	unsigned int& getSampler();

	static Batchrenderer& Get();
	
private:

	Batchrenderer() = default;
	Batchrenderer(Batchrenderer const& rhs) = delete;
	Batchrenderer& operator=(const Batchrenderer& rhs) = delete;
	~Batchrenderer();

	size_t m_maxQuad = 0;
	size_t m_maxVert = 0;
	size_t m_maxIndex = 0;
	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo= 0;

	unsigned int m_vaoSingle = 0;
	unsigned int m_vboSingle = 0;
	unsigned int m_iboSingle = 0;

	uint32_t indexCount = 0;

	Vertex* buffer;
	Vertex* bufferPtr;
	const Camera* m_camera;
	Shader *m_shader;
	float quadPos[8];
	float texPos[8];
	float color[4];
	unsigned int frame;
	unsigned int sampler;

	static Batchrenderer s_instance;	
};
#endif
