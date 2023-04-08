#pragma once
#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <vector>
#include "CameraVo.h"
#include <GL/glew.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include "PerlinNoiseVo.h"
#include "ShaderVo.h"
#include "TextureVo.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Terrain
{
public:
	Terrain();
	~Terrain();

	void LoadHeightmapImage(const char* FileName);
	float GetHeightOfTerrain(float _X, float _Z);
	float BarryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
	void InitTerrain(char* vs, char* fs);
	void CreateTerrainWithPerlinNoise();
	glm::vec3 CalculateNormal(unsigned int x, unsigned int z);
	void SetFog(bool fogState) { m_fog = fogState; }

	void Draw(CameraVo& cam, DirectionalLight* directionLight, PointLight* lamp, SpotLight* spotlight);

private:
	ShaderVo m_terrainShader;
	TextureVo m_terrainTexture;
	GLuint m_terrainTextures[5];
	glm::mat4 m_model;

private:
	enum { VERTEX_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, ELEMENT_BUFFER, TOTAL_BUFFERS };

	GLuint m_VAO;
	GLuint m_VBO[TOTAL_BUFFERS];

	float m_cellSpacing, m_fTerrainHeight;
	float m_terrainLength;
	float m_terrainWidth;
	float m_terrainXPos;
	float m_terrainZPos;
	bool m_fog;

	std::vector<std::vector<float> > m_vHeights;
	std::vector<unsigned int> m_indices;

private:
	std::uint32_t seed;
	PerlinNoiseVo noise;
};

#endif // !__TERRAIN_H__