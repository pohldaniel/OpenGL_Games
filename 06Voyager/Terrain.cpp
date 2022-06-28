#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "engine/Extension.h"
#include "terrain.h"

//-----------------------------------------------------------------------------
// HeightMap.
//-----------------------------------------------------------------------------

HeightMap::HeightMap() : m_size(0), m_gridSpacing(0), m_heightScale(1.0f){
}

HeightMap::~HeightMap(){
	destroy();
}

bool HeightMap::create(int size, int gridSpacing, float scale){
	m_heightScale = scale;
	m_size = size;
	m_gridSpacing = gridSpacing;

	try
	{
		m_heights.resize(m_size * m_size);
	}
	catch (const std::bad_alloc &)
	{
		return false;
	}

	memset(&m_heights[0], 0, m_heights.size());
	return true;
}

void HeightMap::destroy(){
	m_heightScale = 1.0f;
	m_size = 0;
	m_gridSpacing = 0;
	m_heights.clear();
}

void HeightMap::generateDiamondSquareFractal(float roughness){
	// Generates a fractal height field using the diamond-square (midpoint
	// displacement) algorithm. Note that only square height fields work with
	// this algorithm.
	//
	// Based on article and associated code:
	// "Fractal Terrain Generation - Midpoint Displacement" by Jason Shankel
	// (Game Programming Gems I, pp.503-507).

	srand(static_cast<unsigned int>(time(0)));

	std::fill(m_heights.begin(), m_heights.end(), 0.0f);

	int p1, p2, p3, p4, mid;
	float dH = m_size * 0.5f;
	float dHFactor = powf(2.0f, -roughness);
	float minH = 0.0f, maxH = 0.0f;

	for (int w = m_size; w > 0; dH *= dHFactor, w /= 2)
	{
		// Diamond Step.
		for (int z = 0; z < m_size; z += w)
		{
			for (int x = 0; x < m_size; x += w)
			{
				p1 = heightIndexAt(x, z);
				p2 = heightIndexAt(x + w, z);
				p3 = heightIndexAt(x + w, z + w);
				p4 = heightIndexAt(x, z + w);
				mid = heightIndexAt(x + w / 2, z + w / 2);

				m_heights[mid] = HeightMap::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;
				
				minH = std::min(minH, m_heights[mid]);
				maxH = std::max(maxH, m_heights[mid]);
			}
		}

		// Square step.
		for (int z = 0; z < m_size; z += w)
		{
			for (int x = 0; x < m_size; x += w)
			{
				p1 = heightIndexAt(x, z);
				p2 = heightIndexAt(x + w, z);
				p3 = heightIndexAt(x + w / 2, z - w / 2);
				p4 = heightIndexAt(x + w / 2, z + w / 2);
				mid = heightIndexAt(x + w / 2, z);

				m_heights[mid] = HeightMap::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;

				minH = std::min(minH, m_heights[mid]);
				maxH = std::max(maxH, m_heights[mid]);

				p1 = heightIndexAt(x, z);
				p2 = heightIndexAt(x, z + w);
				p3 = heightIndexAt(x + w / 2, z + w / 2);
				p3 = heightIndexAt(x - w / 2, z + w / 2);
				mid = heightIndexAt(x, z + w / 2);

				m_heights[mid] = HeightMap::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;

				minH = std::min(minH, m_heights[mid]);
				maxH = std::max(maxH, m_heights[mid]);
			}
		}
	}

	smooth();

	// Normalize height field so altitudes fall into range [0,255].
	for (int i = 0; i < m_size * m_size; ++i)
		m_heights[i] = 255.0f * (m_heights[i] - minH) / (maxH - minH);
}

float HeightMap::heightAt(float x, float z) const{
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact height of the height map at that (x, z)
	// position using bilinear interpolation.

	x /= static_cast<float>(m_gridSpacing);
	z /= static_cast<float>(m_gridSpacing);

	assert(x >= 0.0f && x < float(m_size));
	assert(z >= 0.0f && z < float(m_size));

	long ix = HeightMap::floatToLong(x);
	long iz = HeightMap::floatToLong(z);
	float topLeft = m_heights[heightIndexAt(ix, iz)] * m_heightScale;
	float topRight = m_heights[heightIndexAt(ix + 1, iz)] * m_heightScale;
	float bottomLeft = m_heights[heightIndexAt(ix, iz + 1)] * m_heightScale;
	float bottomRight = m_heights[heightIndexAt(ix + 1, iz + 1)] * m_heightScale;
	float percentX = x - static_cast<float>(ix);
	float percentZ = z - static_cast<float>(iz);

	return HeightMap::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
}

void HeightMap::normalAt(float x, float z, Vector3f &n) const{
	// Given a (x, z) position on the rendered height map this method
	// calculates the exact normal of the height map at that (x, z) position
	// using bilinear interpolation.

	x /= static_cast<float>(m_gridSpacing);
	z /= static_cast<float>(m_gridSpacing);

	assert(x >= 0.0f && x < float(m_size));
	assert(z >= 0.0f && z < float(m_size));

	long ix = HeightMap::floatToLong(x);
	long iz = HeightMap::floatToLong(z);

	float percentX = x - static_cast<float>(ix);
	float percentZ = z - static_cast<float>(iz);

	Vector3f topLeft;
	Vector3f topRight;
	Vector3f bottomLeft;
	Vector3f bottomRight;
	Vector3f normal;

	normalAtPixel(ix, iz, topLeft);
	normalAtPixel(ix + 1, iz, topRight);
	normalAtPixel(ix, iz + 1, bottomLeft);
	normalAtPixel(ix + 1, iz + 1, bottomRight);

	n = HeightMap::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
	n.normalize();
}

void HeightMap::normalAtPixel(int x, int z, Vector3f &n) const{
	// Returns the normal at the specified location on the height map.
	// The normal is calculated using the properties of the height map.
	// This approach is much quicker and more elegant than triangulating the
	// height map and averaging triangle surface normals.

	if (x > 0 && x < m_size - 1)
		n[0] = heightAtPixel(x - 1, z) - heightAtPixel(x + 1, z);
	else if (x > 0)
		n[0] = 2.0f * (heightAtPixel(x - 1, z) - heightAtPixel(x, z));
	else
		n[0] = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x + 1, z));

	if (z > 0 && z < m_size - 1)
		n[2] = heightAtPixel(x, z - 1) - heightAtPixel(x, z + 1);
	else if (z > 0)
		n[2] = 2.0f * (heightAtPixel(x, z - 1) - heightAtPixel(x, z));
	else
		n[2] = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x, z + 1));

	n[2] = 2.0f * m_gridSpacing;
	n.normalize();
}

void HeightMap::blur(float amount){
	// Applies a simple FIR (Finite Impulse Response) filter across the height
	// map to blur it. 'amount' is in range [0,1]. 0 is no blurring, and 1 is
	// very strong blurring.

	float *pPixel = 0;
	float *pPrevPixel = 0;
	int width = m_size;
	int height = m_size;
	int pitch = m_size;

	// Blur horizontally. Both left-to-right, and right-to-left.
	for (int i = 0; i < height; ++i)
	{
		pPrevPixel = &m_heights[i * pitch];

		// Left-to-right.
		for (int j = 1; j < width; ++j)
		{
			pPixel = &m_heights[(i * pitch) + j];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}

		pPrevPixel = &m_heights[(i * pitch) + (width - 1)];

		// Right-to-left.
		for (int j = width - 2; j >= 0; --j)
		{
			pPixel = &m_heights[(i * pitch) + j];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}
	}

	// Blur vertically. Both top-to-bottom, and bottom-to-top.
	for (int i = 0; i < width; ++i)
	{
		pPrevPixel = &m_heights[i];

		// Top-to-bottom.
		for (int j = 1; j < height; ++j)
		{
			pPixel = &m_heights[(j * pitch) + i];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}

		pPrevPixel = &m_heights[((height - 1) * pitch) + i];

		// Bottom-to-top.
		for (int j = height - 2; j >= 0; --j)
		{
			pPixel = &m_heights[(j * pitch) + i];
			*pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
			pPrevPixel = pPixel;
		}
	}
}

unsigned int HeightMap::heightIndexAt(int x, int z) const
{
	// Given a 2D height map coordinate, this method returns the index
	// into the height map. This method wraps around for coordinates larger
	// than the height map size.
	return (((x + m_size) % m_size) + ((z + m_size) % m_size) * m_size);
}

void HeightMap::smooth()
{
	// Applies a box filter to the height map to smooth it out.

	std::vector<float> source(m_heights);
	float value = 0.0f;
	float cellAverage = 0.0f;
	int i = 0;
	int bounds = m_size * m_size;

	for (int y = 0; y < m_size; ++y)
	{
		for (int x = 0; x < m_size; ++x)
		{
			value = 0.0f;
			cellAverage = 0.0f;

			i = (y - 1) * m_size + (x - 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (y - 1) * m_size + x;
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (y - 1) * m_size + (x + 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = y * m_size + (x - 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = y * m_size + x;
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = y * m_size + (x + 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (y + 1) * m_size + (x - 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (y + 1) * m_size + x;
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			i = (y + 1) * m_size + (x + 1);
			if (i >= 0 && i < bounds)
			{
				value += source[i];
				cellAverage += 1.0f;
			}

			m_heights[y * m_size + x] = value / cellAverage;
		}
	}
}

//-----------------------------------------------------------------------------
// Terrain.
//-----------------------------------------------------------------------------
const float     HEIGHTMAP_TILING_FACTOR = 12.0f;
Terrain::Terrain(){
	

	m_terrainShader = Globals::shaderManager.getAssetPointer("terrain");
	m_textures["dirt"] = &Globals::textureManager.get("dirt");
	m_textures["grass"] = &Globals::textureManager.get("grass");
	m_textures["rock"] = &Globals::textureManager.get("rock");
	m_textures["snow"] = &Globals::textureManager.get("snow");
	m_textures["null"] = &Globals::textureManager.get("null");

	m_textures["dirt"]->setRepeat();
	m_textures["grass"]->setRepeat();
	m_textures["rock"]->setRepeat();
	m_textures["snow"]->setRepeat();
	m_textures["null"]->setRepeat();

	const float HEIGHTMAP_SCALE = 2.0f;
	m_regions[0].min = 0.0f;
	m_regions[0].max = 50.0f * HEIGHTMAP_SCALE;

	m_regions[1].min = 51.0f * HEIGHTMAP_SCALE;
	m_regions[1].max = 101.0f * HEIGHTMAP_SCALE;

	m_regions[2].min = 102.0f * HEIGHTMAP_SCALE;
	m_regions[2].max = 203.0f * HEIGHTMAP_SCALE;

	m_regions[3].min = 204.0f * HEIGHTMAP_SCALE;
	m_regions[3].max = 255.0f * HEIGHTMAP_SCALE;
	
	m_totalVertices = 0;
	m_totalIndices = 0;
	m_disableColorMaps = false;
}

Terrain::~Terrain(){
	destroy();
}

void Terrain::createProcedural(int size, int gridSpacing, float scale, float roughness){
	m_heightMap.create(size, gridSpacing, scale);	
	terrainCreateProcedural(size, gridSpacing, scale);
	generateUsingDiamondSquareFractal(roughness);
}

void Terrain::create(int size, int gridSpacing, float scale, float roughness) {
	m_heightMap.create(size, gridSpacing, scale);
	m_heightMap.generateDiamondSquareFractal(roughness);
	terrainCreate(size, gridSpacing, scale);
}

void Terrain::destroy(){
	m_heightMap.destroy();
	terrainDestroy();
}

void Terrain::draw(const Camera& camera){
	terrainDraw(camera);
}

bool Terrain::generateUsingDiamondSquareFractal(float roughness){
	m_heightMap.generateDiamondSquareFractal(roughness);
	return generateVertices();
}

bool Terrain::terrainCreate(int size, int gridSpacing, float scale){

	generateVertices(m_vertexBuffer);

	if(m_mode == Terrain::Mode::TRIANGLE_STRIP)
		generateIndicesTS(m_indexBuffer);
	else
		generateIndices(m_indexBuffer);

	m_totalVertices = size * size;
	m_totalIndices = m_mode == Terrain::Mode::TRIANGLE_STRIP ? (size - 1) * (size * 2 + 1) : (size - 1) * (size - 1) * 6;

	short stride = 8;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices, &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexBuffer.size(), &m_indexBuffer[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	return true;
}

bool Terrain::terrainCreateProcedural(int size, int gridSpacing, float scale) {

	if (m_mode == Terrain::Mode::TRIANGLE_STRIP)
		generateIndicesTS(m_indexBuffer);
	else
		generateIndices(m_indexBuffer);

	m_totalVertices = size * size;
	m_totalIndices = m_mode == Terrain::Mode::TRIANGLE_STRIP ? (size - 1) * (size * 2 + 1)  : (size - 1) * (size - 1) * 6;

	//m_vertexBuffer.reserve(m_totalVertices * sizeof(Vertex));

	short stride = 8;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices, NULL, GL_DYNAMIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexBuffer.size(), &m_indexBuffer[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	return true;
}

void Terrain::terrainDestroy(){
	
}

void Terrain::terrainDraw(const Camera& camera){
	
	Vector4f lightDir = Vector4f(0.0f, 1.0f, 0.0f, 0.0f);

	glUseProgram(m_terrainShader->m_program);
	m_terrainShader->loadMatrix("u_transform", camera.getViewMatrix() * Globals::projection);
	m_terrainShader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

	m_terrainShader->loadFloat("tilingFactor", HEIGHTMAP_TILING_FACTOR);
	m_terrainShader->loadFloat("region1.max", m_regions[0].max);
	m_terrainShader->loadFloat("region1.min", m_regions[0].min);
	m_terrainShader->loadFloat("region2.max", m_regions[1].max);
	m_terrainShader->loadFloat("region2.min", m_regions[1].min);
	m_terrainShader->loadFloat("region3.max", m_regions[2].max);
	m_terrainShader->loadFloat("region3.min", m_regions[2].min);
	m_terrainShader->loadFloat("region4.max", m_regions[3].max);
	m_terrainShader->loadFloat("region4.min", m_regions[3].min);

	m_terrainShader->loadVector("lightDir", lightDir);

	m_terrainShader->loadInt("region1ColorMap", 0);
	m_terrainShader->loadInt("region2ColorMap", 1);
	m_terrainShader->loadInt("region3ColorMap", 2);
	m_terrainShader->loadInt("region4ColorMap", 3);

	if (m_disableColorMaps) {
		m_textures["null"]->bind(0);
		m_textures["null"]->bind(1);
		m_textures["null"]->bind(2);
		m_textures["null"]->bind(3);

	}else {
		m_textures["dirt"]->bind(0);
		m_textures["grass"]->bind(1);
		m_textures["rock"]->bind(2);
		m_textures["snow"]->bind(3);
	}
	
	glBindVertexArray(m_vao);
	glDrawElements(m_mode == Terrain::Mode::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, m_totalIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glUseProgram(0);
}

void Terrain::generateIndices(std::vector<unsigned int>& indexBuffer) {
	int size = m_heightMap.getSize();

	for (int z = 0; z < size - 1; z++) {
		for (int x = 0; x < size - 1; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			m_indexBuffer.push_back(z * size + x);
			m_indexBuffer.push_back((z + 1) * size + (x + 1));
			m_indexBuffer.push_back(z * size + (x + 1));

			m_indexBuffer.push_back(z * size + x);
			m_indexBuffer.push_back((z + 1) * size + x);
			m_indexBuffer.push_back((z + 1) * size + (x + 1));
		}
	}
}

void Terrain::generateIndicesTS(std::vector<unsigned int>& indexBuffer) {
	int size = m_heightMap.getSize();

	for (int z = 0; z < size - 1; ++z) {
		if (z % 2 == 0) {

			for (int x = 0; x < size; ++x) {
				m_indexBuffer.push_back(x + z * size);
				m_indexBuffer.push_back(x + (z + 1) * size);
			}

			// Add degenerate triangles to stitch strips together.
			m_indexBuffer.push_back((size - 1) + (z + 1) * size);
		}else {
			for (int x = size - 1; x >= 0; --x) {
				m_indexBuffer.push_back(x + z * size);
				m_indexBuffer.push_back(x + (z + 1) * size);
			}

			// Add degenerate triangles to stitch strips together.
			m_indexBuffer.push_back((z + 1) * size);
		}
	}
}

bool Terrain::generateIndices(){

	void *pBuffer = 0;
	int size = m_heightMap.getSize();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	if (!(pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY))){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return false;
	}

	unsigned int *pIndex = static_cast<unsigned int *>(pBuffer);

	for (int z = 0; z < size - 1; z++) {
		for (int x = 0; x < size - 1; x++) {
		
			*pIndex++ = z * size + x;
			*pIndex++ = (z + 1) * size + (x + 1);
			*pIndex++ = z * size + (x + 1);

			*pIndex++ = z * size + x;
			*pIndex++ = (z + 1) * size + x;
			*pIndex++ = (z + 1) * size + (x + 1);
		}
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return true;
}

bool Terrain::generateIndicesTS() {
	void *pBuffer = 0;
	int size = m_heightMap.getSize();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	if (!(pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY))) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return false;
	}

	unsigned int *pIndex = static_cast<unsigned int *>(pBuffer);

	for (int z = 0; z < size - 1; ++z){
		if (z % 2 == 0){

			for (int x = 0; x < size; ++x){
				*pIndex++ = x + z * size;
				*pIndex++ = x + (z + 1) * size;
			}

			// Add degenerate triangles to stitch strips together.
			*pIndex++ = (size - 1) + (z + 1) * size;
		}else{
			for (int x = size - 1; x >= 0; --x){
				*pIndex++ = x + z * size;
				*pIndex++ = x + (z + 1) * size;
			}

			// Add degenerate triangles to stitch strips together.
			*pIndex++ = (z + 1) * size;
		}
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return true;
}

void Terrain::generateVertices(std::vector<float>& vertexBuffer) {
	Vertex *pVertices = 0;
	Vertex *pVertex = 0;
	int currVertex = 0;
	int size = m_heightMap.getSize();
	int gridSpacing = m_heightMap.getGridSpacing();
	float heightScale = m_heightMap.getHeightScale();
	Vector3f normal;

	for (int z = 0; z < size; ++z){
		for (int x = 0; x < size; ++x){
			//std::cout << m_heightMap.heightAtPixel(x, z) * heightScale << std::endl;
			m_heightMap.normalAtPixel(x, z, normal);
			vertexBuffer.push_back(static_cast<float>(x * gridSpacing)); vertexBuffer.push_back(m_heightMap.heightAtPixel(x, z) * heightScale); vertexBuffer.push_back(static_cast<float>(z * gridSpacing));
			vertexBuffer.push_back(normal[0]); vertexBuffer.push_back(normal[1]); vertexBuffer.push_back(normal[2]);
			vertexBuffer.push_back(static_cast<float>(x) / static_cast<float>(size)); vertexBuffer.push_back(static_cast<float>(z) / static_cast<float>(size));
		}
	}

}

bool Terrain::generateVertices(){

	Vertex *pVertices = 0;
	Vertex *pVertex = 0;
	int currVertex = 0;
	int size = m_heightMap.getSize();
	int gridSpacing = m_heightMap.getGridSpacing();
	float heightScale = m_heightMap.getHeightScale();
	Vector3f normal;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	if (!pVertices){
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return false;
	}

	for (int z = 0; z < size; ++z){
		for (int x = 0; x < size; ++x){

			currVertex = z * size + x;
			pVertex = &pVertices[currVertex];

			pVertex->x = static_cast<float>(x * gridSpacing);
			pVertex->y = m_heightMap.heightAtPixel(x, z) * heightScale;
			pVertex->z = static_cast<float>(z * gridSpacing);

			m_heightMap.normalAtPixel(x, z, normal);
			pVertex->nx = normal[0];
			pVertex->ny = normal[1];
			pVertex->nz = normal[2];

			pVertex->s = static_cast<float>(x) / static_cast<float>(size);
			pVertex->t = static_cast<float>(z) / static_cast<float>(size);
		}
	}

	
	//std::memcpy(&m_vertexBuffer[0], pVertices, m_totalVertices * sizeof(Vertex));
	
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}