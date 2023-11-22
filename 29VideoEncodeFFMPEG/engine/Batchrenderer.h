#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include <GL/glew.h>
#include <array>
#include "Shader.h"
#include "Camera.h"

class Batchrenderer {

public:
	struct Vertex {
		std::array<float, 4> posTex;
		std::array<float, 4> color;
		unsigned int frame;
	};

	Batchrenderer() = default;
	~Batchrenderer();

	void init(size_t size = 400, bool drawSingle = false, bool drawRaw = false);
	void shutdown();
	void drawBuffer();
	void drawBufferRaw();

	void addQuadAA(Vector4f posSize, Vector4f texPosSize = Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);
	void addDiamondAA(Vector4f posSize, Vector4f texPosSize = Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);	
	void addHexagon(Vector4f posSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);
	void addHexagonFlip(Vector4f posSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);
	
	void addRotatedQuadRH(Vector4f posSize, float angle, float rotX, float rotY, Vector4f texPosSize = Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);
	void addRotatedQuadLH(Vector4f posSize, float angle, float rotX, float rotY, Vector4f texPosSize = Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);

	void drawSingleQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), unsigned int frame = 0u);
	void processQuad();
	void processSingleQuad();

	void setShader(const Shader* shader);

	void bindTexture(unsigned int texture, bool isTextureArray = false);
	void unbindTexture(bool isTextureArray = false);

	void bindTexture(unsigned int texture, bool isTextureArray, unsigned int unit);
	void unbindTexture(bool isTextureArray, unsigned int unit);
	void activeTexture(unsigned int unit);

	float(&getQuadPos())[8];
	float(&getTexPos())[8];
	float(&getColor())[4];

	unsigned int& getFrame();

	static void ResetStatistic();
	static void PrintStatistic();
	static Batchrenderer& Get();
	
private:

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
	bool m_drawRaw = false;

	Vertex* buffer;
	Vertex* bufferPtr;
	const Shader *m_shader;
	
	float quadPos[8];
	float texPos[8];
	float color[4];
	unsigned int frame;

	static unsigned int s_drawCallCount;
	static unsigned int s_quadCount;
	static Batchrenderer s_instance;	
};
#endif
