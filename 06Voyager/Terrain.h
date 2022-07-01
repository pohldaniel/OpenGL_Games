#pragma once

#include <vector>
#include <functional>
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include "engine/Spritesheet.h"

#include "Constants.h"
#include "Camera.h"

class HeightMap{
public:
	HeightMap();
	~HeightMap();

	float getHeightScale() const{
		return m_heightScale;
	}

	int getResolution() const {
		return m_resolution;
	}

	int getWidth() const {
		return m_width;
	}

	float getGridSpacing() const{
		return m_gridSpacing;
	}

	const float *getHeights() const{
		return &m_heights[0];
	}

	void createFromImage(std::string file, int gridSpacing, float scale);
	void createFromTexture(Texture texture, int width, float scale);
	bool create(int size, int width, float scale);
	void destroy();

	void generateDiamondSquareFractal(float roughness);

	float heightAt(float x, float z) const;

	float heightAtPixel(int x, int z) const{
		
		return m_heights[z * m_size + x];
	};

	void normalAt(float x, float z, Vector3f &n) const;
	void normalAtPixel(int x, int z, Vector3f &n) const;

	static float random(float min, float max){
		// Returns a random number in range [min,max].

		return min + (max - min)
			* (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	static long floatToLong(float f){
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
	static T bilerp(const T &a, const T &b, const T &c, const T &d, float u, float v){
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

	float getHeight(unsigned int x, unsigned int z, unsigned int width, unsigned short numCompontents, unsigned char* data);

private:
	void blur(float amount);
	unsigned int heightIndexAt(int x, int z) const;
	void smooth();

	int m_size;
	int m_width;
	int m_resolution;
	float m_gridSpacing;
	float m_heightScale;
	std::vector<float> m_heights;
};


class Terrain{

public:
	enum Mode {
		TRIANGLES,
		TRIANGLE_STRIP,		
	};

	struct TerrainRegion{
		float min;
		float max;
	};

	TerrainRegion m_regions[4];
	Terrain();
	virtual ~Terrain();

	void createProcedural(int resolution, int width, float scale, float roughness);
	void create(int resolution, int width, float scale, float roughness);
	void create(std::string file, int width, float scale);
	void create(Texture texture, int width, float scale);

	void destroy();
	std::function<void(const Camera&)> draw = 0;
	void drawNormal(const Camera& camera);
	void drawInstanced(const Camera& camera);
	bool generateUsingDiamondSquareFractal(float roughness);
	void scaleRegions(const float heighScale);
	void setDisableColorMaps(bool flag) {
		m_disableColorMaps = flag;
	}

	void toggleDisableColorMaps() {
		m_disableColorMaps = !m_disableColorMaps;
	}

	void toggleColorMode() {
		m_colorMode = !m_colorMode;
	}

	const HeightMap &getHeightMap() const{
		return m_heightMap;
	}

	HeightMap &getHeightMap(){
		return m_heightMap;
	}

	Shader* m_terrainShader;
	std::unordered_map<std::string, Texture*> m_textures;
	std::unordered_map<std::string, Spritesheet*> m_spritesheets;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	void setPosition(const float x, const float y, const float z);
	void setPosition(const Vector3f &position);

	void setGridPosition(int x, int z);
	void createInstances();

protected:
	virtual bool terrainCreate();
	virtual bool terrainCreateProcedural();
	virtual void terrainDestroy();

	Vector3f m_position;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	std::vector<Matrix4f> modelMTX;
private:

	struct Vertex{
		float x, y, z;
		float nx, ny, nz;
		float s, t;
	};

	bool generateIndices();
	bool generateIndicesTS();
	bool generateVertices();

	void generateVertices(std::vector<float>& vertexBuffer);
	void generateIndices(std::vector<unsigned int>& indexBuffer);
	void generateIndicesTS(std::vector<unsigned int>& indexBuffer);

	int m_totalVertices;
	int m_totalIndices;
	HeightMap m_heightMap;
	bool m_disableColorMaps;
	bool m_colorMode = true;
	unsigned int m_vbo, m_ibo, m_vao;
	unsigned int m_vboInstances;

	Mode m_mode = Mode::TRIANGLES;
	float m_tilingFactor = 20.0f;
};

