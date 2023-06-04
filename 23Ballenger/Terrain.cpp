#include <GL/glew.h>
#include <iostream>
#include "Terrain.h"
#include "Utils.h"

HeightMap::HeightMap() : m_width(0), m_height(0), m_heightScale(256.0f / 64.0f), m_minH(FLT_MAX), m_maxH(-FLT_MAX){

}

HeightMap::~HeightMap() {
	destroy();
}

void HeightMap::destroy() {
	m_width = 0;
	m_height = 0;
	m_heightScale = 1.0f;
	m_heights.clear();
}

void HeightMap::loadFromRAW(const char* filename, unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;
	

	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	FILE *pFile = fopen(filename, "rb");
	fread(data, width * height, 1, pFile);
	fclose(pFile);

	m_heights.resize(m_width * m_height);

	unsigned int index = 0;
	
	for (int z = 0; z < m_height; z++) {
		for (int x = 0; x < m_width; x++) {
			index = z * m_width + x;
			m_heights[index] = data[index];

			m_minH = std::min(m_minH, m_heights[index]);
			m_maxH = std::max(m_maxH, m_heights[index]);
		}
	}

	free(data);

	//smooth();
	normalizeHeightMap(m_heightScale, m_minH);
}

void HeightMap::normalizeHeightMap(const float scaleFactor, const float minH) {
	
	for (int i = 0; i < m_width * m_height; ++i) {
		m_heights[i] = (m_heights[i] - minH) / scaleFactor;
	}	

	m_minH = (m_minH - minH) / scaleFactor;
	m_maxH = (m_maxH - minH) / scaleFactor;
}

void HeightMap::smooth() {
	// Applies a box filter to the height map to smooth it out.
	std::vector<float> source(m_heights);
	float value = 0.0f;
	float cellAverage = 0.0f;
	int i = 0;
	int bounds = m_width * m_height;

	for (int z = 0; z < m_height; ++z) {

		for (int x = 0; x < m_width; ++x) {

			value = 0.0f;
			cellAverage = 0.0f;

			i = (z - 1) * m_width + (x - 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (z - 1) * m_width + x;
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (z - 1) * m_width + (x + 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = z * m_width + (x - 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = z * m_width + x;
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = z * m_width + (x + 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (z + 1) * m_width + (x - 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (z + 1) * m_width + x;
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (z + 1) * m_width + (x + 1);
			if (i >= 0 && i < bounds) {
				value += source[i];
				cellAverage += 1.0f;
			}

			m_heights[z * m_width + x] = value / cellAverage;
		}
	}
}

void HeightMap::blur(float amount) {
	// Applies a simple FIR (Finite Impulse Response) filter across the height
	// map to blur it. 'amount' is in range [0,1]. 0 is no blurring, and 1 is
	// very strong blurring.

	float *pPixel = 0;
	float *pPrevPixel = 0;
	int width = m_width;
	int height = m_height;
	int pitch = m_width;

	// Blur horizontally. Both left-to-right, and right-to-left.
	for (int z = 0; z < height; ++z) {
		pPrevPixel = &m_heights[z * pitch];

		// Left-to-right.
		for (int x = 1; x < width; ++x) {
			pPixel = &m_heights[(z * pitch) + x];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}

		pPrevPixel = &m_heights[(z * pitch) + (width - 1)];

		// Right-to-left.
		for (int x = width - 2; x >= 0; --x) {
			pPixel = &m_heights[(z * pitch) + x];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}
	}

	// Blur vertically. Both top-to-bottom, and bottom-to-top.
	for (int z = 0; z < width; ++z) {
		pPrevPixel = &m_heights[z];

		// Top-to-bottom.
		for (int x = 1; x < height; ++x) {
			pPixel = &m_heights[(x * pitch) + z];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}

		pPrevPixel = &m_heights[((height - 1) * pitch) + z];

		// Bottom-to-top.
		for (int x = height - 2; x >= 0; --x) {
			pPixel = &m_heights[(x * pitch) + z];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}
	}
}

float HeightMap::heightAtPixel(int x, int z) const {
	return m_heights[z * m_width + x];
};

void HeightMap::normalAtPixel(int x, int z, Vector3f &n) const {
	// Returns the normal at the specified location on the height map.
	// The normal is calculated using the properties of the height map.
	// This approach is much quicker and more elegant than triangulating the
	// height map and averaging triangle surface normals.

	if (x > 0 && x < m_width - 1)
		n[0] = heightAtPixel(x - 1, z) - heightAtPixel(x + 1, z);
	else if (x > 0)
		n[0] = 2.0f * (heightAtPixel(x - 1, z) -heightAtPixel(x, z));
	else
		n[0] = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x + 1, z));

	if (z > 0 && z < m_height - 1)
		n[2] = heightAtPixel(x, z - 1) - heightAtPixel(x, z + 1);
	else if (z > 0)
		n[2] = 2.0f * (heightAtPixel(x, z - 1) - heightAtPixel(x, z));
	else
		n[2] = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x, z + 1));

	n[1] = 2.0f;
	n.normalize();
}

unsigned int HeightMap::getWidth() const {
	return m_width;
}

unsigned int HeightMap::getHeight() const {
	return m_height;
}

float HeightMap::getMinHeight() const {
	return m_minH;
}

float HeightMap::getMaxHeight() const {
	return m_maxH;
}

float HeightMap::getHeightScale() const {
	return m_heightScale;
}

const float* HeightMap::getHeights() const {
	return m_heights.data();
}

unsigned int HeightMap::heightIndexAt(int x, int z) const {
	// Given a 2D height map coordinate, this method returns the index
	// into the height map. This method wraps around for coordinates larger
	// than the height map size.
	return (((x + m_width) % m_width) + ((z + m_height) % m_height) * m_width);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Terrain::Terrain() {
	
}

Terrain::~Terrain() {
	m_positions.clear();
	m_positions.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
}

bool Terrain::init(const char* filename) {
	bool result;

	m_heightMap.loadFromRAW(filename, TERRAIN_SIZE, TERRAIN_SIZE);

	m_gridSpacing = static_cast<float>(TERRAIN_SIZE) / static_cast<float>(m_heightMap.getWidth());
	m_width = TERRAIN_SIZE;
	m_height = TERRAIN_SIZE;

	create(m_heightMap);

	return true;
}

const HeightMap& Terrain::getHeightMap() const {
	return m_heightMap;
}

const Vector3f& Terrain::getMin() const {
	return m_min;
}
const Vector3f& Terrain::getMax() const {
	return m_max;
}

void Terrain::create(const HeightMap& heightMap) {
	
	Vector3f normal;
	for (int z = 0; z < heightMap.getHeight(); z++) {
		for (int x = 0; x < heightMap.getWidth(); x++) {

			m_positions.push_back(Vector3f(static_cast<float>(x * m_gridSpacing), m_heightMap.heightAtPixel(x, z), static_cast<float>(z * m_gridSpacing)));
			m_texels.push_back(Vector2f(static_cast<float>(x) / static_cast<float>(heightMap.getWidth()) * (float)TEXTURE_REPEAT, static_cast<float>(z) / static_cast<float>(heightMap.getHeight()) * (float)TEXTURE_REPEAT));
			
			heightMap.normalAtPixel(x, z, normal);
			m_normals.push_back(normal);
		}
	}

	m_min = Vector3f(0.0f, m_heightMap.getMinHeight(), 0.0f);
	m_max = Vector3f(static_cast<float>(heightMap.getWidth()), m_heightMap.getMaxHeight(), static_cast<float>(heightMap.getHeight()));

	generateIndices();
	createBuffer();
}

void Terrain::generateIndices() {
	int resolutionZ = m_height;
	int resolutionX = m_width;

	for (int z = 0; z < resolutionZ - 1; z++) {
		for (int x = 0; x < resolutionX - 1; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      3		2 -* 3
			m_indexBuffer.push_back((z + 1) * (resolutionX)+(x + 1));
			m_indexBuffer.push_back(z * (resolutionX)+(x + 1));
			m_indexBuffer.push_back(z * (resolutionX)+x);


			m_indexBuffer.push_back((z + 1) * (resolutionX)+x);
			m_indexBuffer.push_back((z + 1) * (resolutionX)+(x + 1));
			m_indexBuffer.push_back(z * (resolutionX)+x);
		}
	}
}

void Terrain::generateIndicesTS() {
	int resolutionZ = m_height;
	int resolutionX = m_width;

	for (int z = 0; z < resolutionZ - 1; ++z) {

		if (z % 2 == 0) {

			for (int x = resolutionX - 1; x >= 0; --x) {
				m_indexBuffer.push_back(x + (z + 1) * resolutionX);
				m_indexBuffer.push_back(x + z * resolutionX);

			}

			// Add degenerate triangles to stitch strips together.
			m_indexBuffer.push_back((z + 1) * resolutionX);

		}else {
			for (int x = 0; x < resolutionX; ++x) {
				m_indexBuffer.push_back(x + (z + 1) * resolutionX);
				m_indexBuffer.push_back(x + z * (resolutionX));
			}

			// Add degenerate triangles to stitch strips together.
			m_indexBuffer.push_back((resolutionX - 1) + (z + 1) * resolutionX);
		}
	}
}

void Terrain::createBuffer() {
	m_drawCount = m_indexBuffer.size();


	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glGenBuffers(!m_positions.empty() + !m_texels.empty() + !m_normals.empty(), m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (!m_texels.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (!m_normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, !m_texels.empty() ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	glBindVertexArray(0);

	//m_positions.clear();
	//m_positions.shrink_to_fit();
	//m_indexBuffer.clear();
	//m_indexBuffer.shrink_to_fit();
	//m_texels.clear();
	//m_texels.shrink_to_fit();
	//m_normals.clear();
	//m_normals.shrink_to_fit();
}

float Terrain::heightAt(float x, float z) const {
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact height of the height map at that (x, z)
	// position using bilinear interpolation.
	x /= m_gridSpacing;
	z /= m_gridSpacing;

	long ix = Utils::floatToLong(x);
	long iz = Utils::floatToLong(z);

	float percentX = x - static_cast<float>(ix);
	float percentZ = z - static_cast<float>(iz);

	float topLeft = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix, iz)];
	float topRight = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix + 1, iz)];
	float bottomLeft = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix, iz + 1)];
	float bottomRight = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix + 1, iz + 1)];
	
	return Math::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
}

void Terrain::normalAt(float x, float z, Vector3f &n) const {
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact normal of the height map at that (x, z) position
	// using bilinear interpolation.
	x /= m_gridSpacing;
	z /= m_gridSpacing;

	long ix = Utils::floatToLong(x);
	long iz = Utils::floatToLong(z);

	float percentX = x - static_cast<float>(ix);
	float percentZ = z - static_cast<float>(iz);

	Vector3f topLeft;
	Vector3f topRight;
	Vector3f bottomLeft;
	Vector3f bottomRight;
	Vector3f normal;

	m_heightMap.normalAtPixel(ix, iz, topLeft);
	m_heightMap.normalAtPixel(ix + 1, iz, topRight);
	m_heightMap.normalAtPixel(ix, iz + 1, bottomLeft);
	m_heightMap.normalAtPixel(ix + 1, iz + 1, bottomRight);

	n = Math::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
	n.normalize();
}

std::vector<Vector3f>& Terrain::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& Terrain::getIndexBuffer() {
	return m_indexBuffer;
}

unsigned int Terrain::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void Terrain::drawRaw() const {
	
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glDrawElements(GL_TRIANGLE_STRIP, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Terrain::bindVAO() const {
	glBindVertexArray(m_vao);
}

void Terrain::unbindVAO() const {
	glBindVertexArray(0);
}
