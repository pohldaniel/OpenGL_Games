#pragma once

#include <vector>
#include <GL/glew.h>
#include "engine/Camera.h"
#include "engine/Texture.h"

#include "PerlinNoiseVo.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Terrain {

	enum { VERTEX_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, ELEMENT_BUFFER, TOTAL_BUFFERS };

public:
	Terrain();
	~Terrain();

	void init();	
	void draw(const Camera& camera, DirectionalLight* directionLight, PointLight* lamp, SpotLight* spotlight);
	float getHeightOfTerrain(float x, float z) const;
	void createTerrainWithPerlinNoise();
	void setFog(bool fogState) { m_fog = fogState; }

private:

	void loadHeightmapImage(const char* fileName);
	
	float barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const;	
	Vector3f calculateNormal(unsigned int x, unsigned int z);

	GLuint m_VAO;
	GLuint m_VBO[TOTAL_BUFFERS];

	std::vector<Texture> m_textures;
	Matrix4f m_model;

	float m_cellSpacing, m_fTerrainHeight;
	float m_terrainLength;
	float m_terrainWidth;
	float m_terrainXPos;
	float m_terrainZPos;
	bool m_fog;

	std::vector<std::vector<float>> m_vHeights;
	std::vector<unsigned int> m_indices;

	std::uint32_t seed;
	PerlinNoiseVo noise;
};