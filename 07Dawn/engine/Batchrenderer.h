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
		std::array<unsigned int, 4> drawState;
	};

	void init(size_t size = 400, bool drawSingle = false);
	void shutdown();
	void drawBuffer();
	void addQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = true, unsigned int sampler = 0u, unsigned int frame = 0u);
	void drawSingleQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = true, unsigned int sampler = 0u, unsigned int frame = 0u);
	void processQuad();
	void processSingleQuad();

	void setCamera(const Camera& camera);
	void setShader(Shader* shader);

	void bindTexture(unsigned int texture, bool isTextureArray = false);
	void unbindTexture(bool isTextureArray = false);

	void bindTexture(unsigned int texture, bool isTextureArray, unsigned int unit);
	void unbindTexture(bool isTextureArray, unsigned int unit);
	void activeTexture(unsigned int unit);

	float(&getQuadPos())[8];
	float(&getTexPos())[8];
	float(&getColor())[4];

	bool& getUpdateView();
	unsigned int& getSampler();
	unsigned int& getFrame();

	static void ResetStatistic();
	static void PrintStatistic();
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
	bool updateView;

	static unsigned int s_drawCallCount;
	static unsigned int s_quadCount;
	static Batchrenderer s_instance;	
};
#endif
