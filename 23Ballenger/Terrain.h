#pragma once

#include "engine/Vector.h"
#include "engine/Texture.h"

#define TERRAIN_SIZE    1024
#define TEXTURE_REPEAT	64

class HeightMap {

public:

	HeightMap();
	~HeightMap();

	void loadFromRAW(const char* filename, unsigned int width, unsigned int height);

	void destroy();
	float heightAtPixel(int x, int z) const;
	void normalAtPixel(int x, int z, Vector3f &n) const;

	unsigned int heightIndexAt(int x, int z) const;

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	float getHeightScale() const;
	const float* getHeights() const;
	float getMinHeight() const;
	float getMaxHeight() const;

private:

	void normalizeHeightMap(const float scaleFactor, const float minH = 0.0f);
	void blur(float amount);
	void smooth();

	unsigned int m_width;
	unsigned int m_height;	
	float m_heightScale;
	std::vector<float> m_heights;
	float m_minH;
	float m_maxH;
};

class Terrain {

	friend class QuadTree;

public:
	Terrain();
	~Terrain();

	bool init(const char* filename);
	
	void drawRaw() const;

	float heightAt(float x, float z) const;
	void normalAt(float x, float z, Vector3f &n) const;

	const HeightMap& getHeightMap() const;
	unsigned int getNumberOfTriangles();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();

	const Vector3f& getMin() const;
	const Vector3f& getMax() const;

	void bindVAO() const;
	void unbindVAO() const;

private:

	void create(const HeightMap& heightMap);
	void createBuffer();
	void generateIndices();
	void generateIndicesTS();
	

	int m_width, m_height;
	int m_vertexCount;
	float m_gridSpacing;
	HeightMap m_heightMap;

	unsigned int m_vao;
	unsigned int m_ibo;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_drawCount;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	Vector3f m_min;
	Vector3f m_max;
};