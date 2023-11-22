#pragma once
#include "engine/Vector.h"
#include "engine/Texture.h"
const int TEXTURE_REPEAT = 8;

class TerrainClass {

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
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(const char* filename, Texture* texture);
	void Shutdown();

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

private:
	bool LoadHeightMap(const char* filename);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();

	bool InitializeBuffers();
	void ShutdownBuffers();

private:
	int m_terrainWidth, m_terrainHeight;
	HeightMapType* m_heightMap;
	Texture* m_Texture;
	int m_vertexCount;
	Vertex* m_vertices;
};