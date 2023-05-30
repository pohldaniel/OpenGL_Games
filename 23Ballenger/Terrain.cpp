#include <iostream>

#include "Terrain.h"

HeightMap::HeightMap() : m_width(0), m_height(0), m_heightScale(256.0f / 64.0f) {

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
	float minH = 0.0f, maxH = 0.0f;
	for (int z = 0; z < m_height; z++) {
		for (int x = 0; x < m_width; x++) {
			index = z * m_width + x;
			m_heights[index] = data[index];

			minH = std::min(minH, m_heights[index]);
			maxH = std::max(maxH, m_heights[index]);
		}
	}

	free(data);

	//smooth();
	normalizeHeightMap(m_heightScale, minH);
}

void HeightMap::normalizeHeightMap(const float scaleFactor, const float minH) {
	for (int i = 0; i < m_width * m_height; ++i) {
		m_heights[i] = (m_heights[i] - minH) / scaleFactor;
	}	
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
	m_data = 0;
	m_vertices = 0;
}


Terrain::Terrain(const Terrain& other) {
}

Terrain::~Terrain() {
}


bool Terrain::Initialize(const char* filename) {
	bool result;

	m_heightMap.loadFromRAW(filename, TERRAIN_SIZE, TERRAIN_SIZE);

	m_gridSpacing = static_cast<float>(TERRAIN_SIZE) / static_cast<float>(m_heightMap.getWidth());
	m_width = TERRAIN_SIZE;
	m_height = TERRAIN_SIZE;

	create(m_heightMap);

	InitializeBuffers();

	return true;
}


void Terrain::Shutdown() {
	// Release the vertex array.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}

void Terrain::create(const HeightMap& heightMap) {
	
	int index;
	Vector3f normal;
	m_data =  new HeightMapType[heightMap.getWidth() * heightMap.getHeight()];
	
	// Read the image data into the height map.
	for (int z = 0; z < heightMap.getHeight(); z++) {
		for (int x = 0; x < heightMap.getWidth(); x++) {

			index = (heightMap.getWidth() * z) + x;
			m_data[index].x = static_cast<float>(x * m_gridSpacing);
			m_data[index].y = m_heightMap.heightAtPixel(x, z);
			m_data[index].z = static_cast<float>(z * m_gridSpacing);

			heightMap.normalAtPixel(x, z, normal);
			m_data[index].nx = normal[0];
			m_data[index].ny = normal[1];
			m_data[index].nz = normal[2];

			m_data[index].tu = static_cast<float>(x) / static_cast<float>(heightMap.getWidth()) * (float)TEXTURE_REPEAT;
			m_data[index].tv = static_cast<float>(z) / static_cast<float>(heightMap.getHeight()) * (float)TEXTURE_REPEAT;
		}
	}

	
}

void Terrain::ShutdownHeightMap() {
	if (m_data) {
		delete[] m_data;
		m_data = 0;
	}

	return;
}

bool Terrain::InitializeBuffers() {
	int index, i, j, index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_width - 1) * (m_height - 1) * 6;

	// Create the vertex array.
	m_vertices = new Vertex[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for (j = 0; j<(m_height - 1); j++) {
		for (i = 0; i<(m_width - 1); i++) {
			index1 = (m_height * j) + i;          // Bottom left.
			index2 = (m_height * j) + (i + 1);      // Bottom right.
			index3 = (m_height * (j + 1)) + i;      // Upper left.
			index4 = (m_height * (j + 1)) + (i + 1);  // Upper right.

															 // Upper left.
			tv = m_data[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_data[index3].x, m_data[index3].y, m_data[index3].z);
			m_vertices[index].texture = Vector2f(m_data[index3].tu, tv);
			m_vertices[index].normal = Vector3f(m_data[index3].nx, m_data[index3].ny, m_data[index3].nz);
			index++;

			// Upper right.
			tu = m_data[index4].tu;
			tv = m_data[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_data[index4].x, m_data[index4].y, m_data[index4].z);
			m_vertices[index].texture = Vector2f(tu, tv);
			m_vertices[index].normal = Vector3f(m_data[index4].nx, m_data[index4].ny, m_data[index4].nz);
			index++;

			// Bottom left.
			m_vertices[index].position = Vector3f(m_data[index1].x, m_data[index1].y, m_data[index1].z);
			m_vertices[index].texture = Vector2f(m_data[index1].tu, m_data[index1].tv);
			m_vertices[index].normal = Vector3f(m_data[index1].nx, m_data[index1].ny, m_data[index1].nz);
			index++;

			// Bottom left.
			m_vertices[index].position = Vector3f(m_data[index1].x, m_data[index1].y, m_data[index1].z);
			m_vertices[index].texture = Vector2f(m_data[index1].tu, m_data[index1].tv);
			m_vertices[index].normal = Vector3f(m_data[index1].nx, m_data[index1].ny, m_data[index1].nz);
			index++;

			// Upper right.
			tu = m_data[index4].tu;
			tv = m_data[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_data[index4].x, m_data[index4].y, m_data[index4].z);
			m_vertices[index].texture = Vector2f(tu, tv);
			m_vertices[index].normal = Vector3f(m_data[index4].nx, m_data[index4].ny, m_data[index4].nz);
			index++;

			// Bottom right.
			tu = m_data[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			m_vertices[index].position = Vector3f(m_data[index2].x, m_data[index2].y, m_data[index2].z);
			m_vertices[index].texture = Vector2f(tu, m_data[index2].tv);
			m_vertices[index].normal = Vector3f(m_data[index2].nx, m_data[index2].ny, m_data[index2].nz);
			index++;
		}
	}

	return true;
}


void Terrain::ShutdownBuffers() {
	// Release the vertex array.
	if (m_vertices) {
		delete[] m_vertices;
		m_vertices = 0;
	}

	return;
}

int Terrain::GetVertexCount() {
	return m_vertexCount;
}


void Terrain::CopyVertexArray(void* vertexList) {
	memcpy(vertexList, m_vertices, sizeof(Vertex) * m_vertexCount);
	return;
}

float Terrain::heightAt(float x, float z) const {
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact height of the height map at that (x, z)
	// position using bilinear interpolation.
	x /= m_gridSpacing;
	z /= m_gridSpacing;

	long ix = Terrain::floatToLong(x);
	long iz = Terrain::floatToLong(z);

	float percentX = x - static_cast<float>(ix);
	float percentZ = z - static_cast<float>(iz);

	float topLeft = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix, iz)];
	float topRight = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix + 1, iz)];
	float bottomLeft = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix, iz + 1)];
	float bottomRight = m_heightMap.getHeights()[m_heightMap.heightIndexAt(ix + 1, iz + 1)];
	
	return Terrain::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
}

void Terrain::normalAt(float x, float z, Vector3f &n) const {
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact normal of the height map at that (x, z) position
	// using bilinear interpolation.
	x /= m_gridSpacing;
	z /= m_gridSpacing;

	long ix = Terrain::floatToLong(x);
	long iz = Terrain::floatToLong(z);

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

	n = Terrain::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
	n.normalize();
}

