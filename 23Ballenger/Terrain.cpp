#include <iostream>

#include "Terrain.h"


Terrain::Terrain() {
	m_heightMap = 0;
	m_vertices = 0;
}


Terrain::Terrain(const Terrain& other) {
}

Terrain::~Terrain() {
}


bool Terrain::Initialize(const char* filename) {
	bool result;

	// Load in the height map for the terrain.
	m_heightMap = loadFromRAW(filename, TERRAIN_SIZE, TERRAIN_SIZE);
	if (!result) {
		return false;
	}

	m_terrainWidth = TERRAIN_SIZE;
	m_terrainHeight = TERRAIN_SIZE;

	float MAX_HEIGHT = 64.0f;
	float SCALE_FACTOR = 256.0f / MAX_HEIGHT;
	// Normalize the height of the height map.
	normalizeHeightMap(SCALE_FACTOR);

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the texture.
	//result = LoadTexture(device, textureFilename);
	if (!result) {
		return false;
	}

	// Initialize the vertex array that will hold the geometry for the terrain.
	result = InitializeBuffers();
	if (!result) {
		return false;
	}

	return true;
}


void Terrain::Shutdown() {
	// Release the vertex array.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}

Terrain::HeightMapType* Terrain::loadFromRAW(const char* filename, unsigned int width, unsigned int height) {
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	FILE *pFile = fopen(filename, "rb");
	fread(data, width * height, 1, pFile);
	fclose(pFile);


	HeightMapType* array = new HeightMapType[width * height];

	int index;

	// Read the image data into the height map.
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {

			index = (height * j) + i;

			float scaledHeight = data[index];

			array[index].x = (float)i;
			array[index].y = scaledHeight;
			array[index].z = (float)j;
		}
	}
	free(data);

	return array;
}


void Terrain::normalizeHeightMap(const float scaleFactor) {
	int i, j;


	for (j = 0; j<m_terrainHeight; j++) {
		for (i = 0; i<m_terrainWidth; i++) {
			m_heightMap[(m_terrainHeight * j) + i].y /= scaleFactor;
		}
	}

	return;
}


bool Terrain::CalculateNormals() {
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals) {
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(m_terrainHeight - 1); j++) {
		for (i = 0; i<(m_terrainWidth - 1); i++) {
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}


void Terrain::ShutdownHeightMap() {
	if (m_heightMap) {
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void Terrain::CalculateTextureCoordinates() {
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j<m_terrainHeight; j++) {
		for (i = 0; i<m_terrainWidth; i++) {
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount){
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool Terrain::InitializeBuffers() {
	int index, i, j, index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Create the vertex array.
	m_vertices = new Vertex[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for (j = 0; j<(m_terrainHeight - 1); j++) {
		for (i = 0; i<(m_terrainWidth - 1); i++) {
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

															 // Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_vertices[index].texture = Vector2f(m_heightMap[index3].tu, tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = Vector2f(tu, tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			index++;

			// Bottom left.
			m_vertices[index].position = Vector3f(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = Vector2f(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			index++;

			// Bottom left.
			m_vertices[index].position = Vector3f(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = Vector2f(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = Vector3f(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = Vector2f(tu, tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			m_vertices[index].position = Vector3f(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_vertices[index].texture = Vector2f(tu, m_heightMap[index2].tv);
			m_vertices[index].normal = Vector3f(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
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