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

private:

	void normalizeHeightMap(const float scaleFactor, const float minH = 0.0f);
	void blur(float amount);
	void smooth();


	unsigned int m_width;
	unsigned int m_height;	
	float m_heightScale;
	std::vector<float> m_heights;
};

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
	
	void drawRaw() const;

	void Shutdown();

	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

	float heightAt(float x, float z) const;
	void normalAt(float x, float z, Vector3f &n) const;

	unsigned int getNumberOfTriangles();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();

	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<unsigned int> m_indexBuffer;

private:

	void create(const HeightMap& heightMap);
	void generateIndices();
	void generateIndicesTS();
	void createBuffer();

	void ShutdownHeightMap();
	bool InitializeBuffers();
	void ShutdownBuffers();

private:
	int m_width, m_height;
	HeightMapType* m_data = nullptr;
	int m_vertexCount;
	Vertex* m_vertices;
	float m_gridSpacing;

	HeightMap m_heightMap;





	
	

	unsigned int m_vao;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_drawCount;

	static long floatToLong(float f) {
		// Converts a floating point number into an integer.
		// Fractional values are truncated as in ANSI C.
		// About 5 to 6 times faster than a standard typecast to an integer.

		long fpBits = *reinterpret_cast<const long*>(&f);
		long shift = 23 - (((fpBits & 0x7fffffff) >> 23) - 127);
		long result = ((fpBits & 0x7fffff) | (1 << 23)) >> shift;

		result = (result ^ (fpBits >> 31)) - (fpBits >> 31);
		result &= ~((((fpBits & 0x7fffffff) >> 23) - 127) >> 31);

		return result;
	}

	template <typename T>
	static T bilerp(const T &a, const T &b, const T &c, const T &d, float u, float v) {
		// Performs a bilinear interpolation.
		//  P(u,v) = e + v(f - e)
		//  
		//  where
		//  e = a + u(b - a)
		//  f = c + u(d - c)
		//  u in range [0,1]
		//  v in range [0,1]

		return a * ((1.0f - u) * (1.0f - v))
			+ b * (u * (1.0f - v))
			+ c * (v * (1.0f - u))
			+ d * (u * v);
	}
};