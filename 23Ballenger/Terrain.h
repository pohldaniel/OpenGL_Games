#pragma once

#include "engine/Vector.h"
#include "engine/Texture.h"

#define TERRAIN_SIZE    1024
#define TEXTURE_REPEAT	64

class Terrain {

private:

	struct Vertex {
		Vector3f position;
		Vector2f texture;
		Vector3f normal;
	};

	struct HeightMapType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType {
		float x, y, z;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Initialize(const char* filename);
	
	void Shutdown();

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

private:

	HeightMapType* loadFromRAW(const char* filename, unsigned int width, unsigned int height);
	void normalizeHeightMap(const float scaleFactor);
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();

	bool InitializeBuffers();
	void ShutdownBuffers();

private:
	int m_terrainWidth, m_terrainHeight;
	HeightMapType* m_heightMap = nullptr;
	int m_vertexCount;
	Vertex* m_vertices;
	
};