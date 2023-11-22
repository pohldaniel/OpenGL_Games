#pragma once

#include <GL/glew.h>
#include <vector>
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/Camera.h"
#include "engine/Framebuffer.h"

#include "Constants.h"

class Terrain;

class Water {

public:

	Water(const float& dt, const float& fdt);
	virtual ~Water();

	void create(unsigned int resolution, unsigned int width, float waterLevel);
	void createBuffer();
	void generateVertices(std::vector<float>& vertexBuffer);
	void generateIndices(std::vector<unsigned int>& indexBuffer);

	void setGridPosition(int x, int z);

	void render(const Camera& camera, const unsigned int &reflectionTexture, const unsigned int &refractionTexture, const unsigned int &refractionDepthTexture);
	float getWaterLevel();

	Shader* m_waterShader;
	unsigned int m_width;
	unsigned int m_resolution;
	unsigned int m_totalVertices;
	unsigned int m_totalIndices;
	float m_gridSpacing;
	const float WAVE_SPEED = 0.03f;
	float move = 0.0f;
	float m_waterLevel;

	std::unordered_map<std::string, Texture*> m_textures;

	unsigned int m_vbo, m_ibo, m_vao;
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	Matrix4f m_model;
	std::vector<Matrix4f> modelMTX;

	const float& m_fdt;
	const float& m_dt;

	Framebuffer m_reflectionBuffer;
	Framebuffer m_refractionBuffer;

	void bindReflectionBuffer();
	void bindRefractionBuffer();

	Framebuffer getReflectionBuffer();
	Framebuffer getRefractionBuffer();
};