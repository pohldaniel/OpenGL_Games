#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <soil2/SOIL2.h>

#include "Terrain.h"
#include "Constants.h"

Terrain::Terrain() :
	m_fTerrainHeight(70.0f), 
	m_cellSpacing(3.0f),
	m_terrainLength(256), 
	m_terrainWidth(256),
	m_terrainXPos(0.0f),
	m_terrainZPos(0.0f) {

	m_model.translate(m_terrainXPos, 0.0f, m_terrainZPos);
}

Terrain::~Terrain(){

}

void Terrain::init() {

	m_textures.resize(6);

	m_textures[0].loadFromFile("res/Textures/soil01.jpg", false);
	m_textures[0].setFilter(GL_LINEAR);
	m_textures[0].setWrapMode(GL_REPEAT);

	m_textures[1].loadFromFile("res/Textures/soil02.jpg", false);
	m_textures[1].setFilter(GL_LINEAR);
	m_textures[1].setWrapMode(GL_REPEAT);

	m_textures[2].loadFromFile("res/Textures/soil03.jpg", false);
	m_textures[2].setFilter(GL_LINEAR);
	m_textures[2].setWrapMode(GL_REPEAT);

	m_textures[3].loadFromFile("res/Textures/soil04.jpg", false);
	m_textures[3].setFilter(GL_LINEAR);
	m_textures[3].setWrapMode(GL_REPEAT);

	m_textures[4].loadFromFile("res/Textures/blendMap.png", false);
	m_textures[4].setFilter(GL_LINEAR);
	m_textures[4].setWrapMode(GL_REPEAT);

	m_textures[5].loadFromFile("res/Textures/soil03_NormalMap.jpg", false);
	m_textures[5].setFilter(GL_LINEAR);
	m_textures[5].setWrapMode(GL_REPEAT);
}

void Terrain::loadHeightmapImage(const char* fileName){

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName, &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	if (imageData == nullptr) {
		std::cerr << "ERROR: Unable to load heightmap.\n";
		return;
	}

	std::vector<float> tmp;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width * numCompontents; j = j + numCompontents) {
			tmp.push_back((float)imageData[i *(width * numCompontents) + j] / 255.0f);
		}

		m_vHeights.push_back(tmp);
		tmp.clear();
	}

	SOIL_free_image_data(imageData);

	float loadingCount = 0.0f;
	int iTotalLoadingCycles = height * width;

	std::vector<Vector3f> vertices;
	std::vector<Vector2f> textures;
	std::vector<Vector3f> normals;

	for (unsigned int i = 0; i < m_vHeights.size(); ++i) {

		for (unsigned int j = 0; j < m_vHeights[0].size(); ++j) {

			if (m_vHeights[i][j] <= 0.0) {
				continue;
			}

			vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight, j * m_cellSpacing));
			textures.push_back(Vector2f(i * 1.0f / m_vHeights.size(), j * 1.0f / m_vHeights[0].size()));
			normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
		}
	}

	for (unsigned int i = 0; i < m_vHeights.size() - 1; ++i) {

		for (unsigned int j = 0; j < m_vHeights.size() - 1; ++j){

			++loadingCount;
			m_indices.push_back((i * m_vHeights[0].size()) + j);
			m_indices.push_back((i * m_vHeights[0].size()) + j + 1);
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j);

			m_indices.push_back((i * m_vHeights[0].size()) + j + 1);			
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j + 1);
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j);
		}

		std::cout << "Loading Terrain: " << loadingCount << " / " << iTotalLoadingCycles << " \r";
	}

	std::cout << "Terrain loading operation completed \n";

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_VBO[TEXTURE_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[TEXTURE_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(Vector2f), &textures[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_VBO[NORMAL_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_VBO[ELEMENT_BUFFER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[ELEMENT_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::cout << "Terrain loaded successfully!\n";
}

void Terrain::createTerrainWithPerlinNoise() {

	// Set the seed of the noise so that it is different every time
	noise.SetSeed(static_cast<unsigned int>(time(0)));
	double frequency = 4.2;
	int octaves = 5;

	const double fx = 256.0 / frequency;
	const double fy = 256.0 / frequency;

	std::vector<float> tmp;
	for (unsigned int y = 0; y < 256; ++y) {

		for (unsigned int x = 0; x < 256; ++x) {

			const _RGB color(noise.OctaveNoise(x / fx, y / fy, octaves));
			tmp.push_back((float)color.r);
		}

		m_vHeights.push_back(tmp);
		tmp.clear();
	}

	std::vector<Vector3f> vertices;
	std::vector<Vector2f> textures;
	std::vector<Vector3f> normals;
	std::vector<Vector3f> tangents;

	// Calculate vertices
	int terrainHeightOffsetFront = 50;
	int terrainHeightOffsetBack = 0;
	int terrainHeightOffsetLeftSide = 50;
	int terrainHeightOffsetRightSide = 0;

	for (unsigned int i = 0; i < m_vHeights.size(); ++i) {
		terrainHeightOffsetFront = 50;
		terrainHeightOffsetBack = 0;
		terrainHeightOffsetLeftSide -= 4;

		if (i > 245)
			terrainHeightOffsetRightSide += 4;

		for (unsigned int j = 0; j < m_vHeights[0].size(); ++j) {

			if (i < 12) {
				vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight + terrainHeightOffsetLeftSide, j * m_cellSpacing));

			}else if (i > 245) {
				vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight + terrainHeightOffsetRightSide, j * m_cellSpacing));

			}else if (j < 12) {
				vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight + terrainHeightOffsetFront, j * m_cellSpacing));
				terrainHeightOffsetFront -= 4;

			}else if (j > 245) {
				terrainHeightOffsetBack += 4;

				if (terrainHeightOffsetBack > 70)
					terrainHeightOffsetBack = 70;

				vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight + terrainHeightOffsetBack, j * m_cellSpacing));

			}else{
				terrainHeightOffsetFront = 50;
				vertices.push_back(Vector3f(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight, j * m_cellSpacing));
			}

			//Vertices.push_back(glm::vec3(i * m_cellSpacing, m_vHeights[i][j] * m_fTerrainHeight, j * m_cellSpacing));
			textures.push_back(Vector2f(i * 1.0f / m_vHeights.size(), j * 1.0f / m_vHeights[0].size()));
			normals.push_back(calculateNormal(i, j));
		}
	}

	// Calculate indices
	for (unsigned int i = 0; i < m_vHeights.size() - 1; ++i) {

		for (unsigned int j = 0; j < m_vHeights.size() - 1; ++j){

			m_indices.push_back((i * m_vHeights[0].size()) + j);
			m_indices.push_back((i * m_vHeights[0].size()) + j + 1);
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j);

			m_indices.push_back((i * m_vHeights[0].size()) + j + 1);		
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j + 1);
			m_indices.push_back(((i + 1) * m_vHeights[0].size()) + j);
		}
	}

	// Calculate tangents
	for (unsigned int i = 0; i < m_vHeights.size(); ++i) {

		for (unsigned int j = 0; j < m_vHeights[0].size(); ++j) {
			int vertexIndex = j + i * 256;
			Vector3f v1 = vertices[vertexIndex];

			if (j < 255) {
				Vector3f v2 = vertices[vertexIndex + 1];
				Vector3f result = v1 - v2;
				result = Vector3f::Normalize(result);
				tangents.push_back(result);

			} else {
				Vector3f v2 = vertices[vertexIndex - 1];
				Vector3f result = v1 - v2;
				result = Vector3f::Normalize(result);
				tangents.push_back(result);
			}
		}
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(TOTAL_BUFFERS, m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[TEXTURE_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(Vector2f), &textures[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[TANGENT_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(Vector3f), &tangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_VBO[ELEMENT_BUFFER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[ELEMENT_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Vector3f Terrain::calculateNormal(unsigned int x, unsigned int z){

	if (x >= 0 && x < m_terrainWidth - 1 && z >= 0 && z < m_terrainLength - 1){

		float heightL = getHeightOfTerrain((float)x - 1, (float)z);
		float heightR = getHeightOfTerrain((float)x + 1, (float)z);
		float heightD = getHeightOfTerrain((float)x, (float)z - 1);
		float heightU = getHeightOfTerrain((float)x, (float)z + 1);

		Vector3f normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = Vector3f::Normalize(normal);
		return normal;
	}

	return Vector3f(0.0f, 0.0f, 0.0f);
}

float Terrain::getHeightOfTerrain(float x, float z) const{

	float result = 0.0f;

	float terrainX = x - m_terrainXPos;
	float terrainZ = z - m_terrainZPos;

	// calculate length of grid square
	float gridSquareLength = m_terrainLength * m_cellSpacing / ((float)m_terrainWidth - 1);

	// Check which grid square the player is in
	int gridX = (int)std::floor(terrainX / gridSquareLength);
	int gridZ = (int)std::floor(terrainZ / gridSquareLength);

	// Check if position is on the terrain
	if (gridX >= m_terrainWidth - 1 || gridZ >= m_terrainLength - 1 || gridX < 0 || gridZ < 0){
		return 0.0f;
	}

	// Find out where the player's position is on the terrain
	float xCoord = std::fmod(terrainX, gridSquareLength) / gridSquareLength;
	float zCoord = std::fmod(terrainZ, gridSquareLength) / gridSquareLength;

	// Top triangle of the quad, else the bottom triangle of the quad
	if (xCoord <= (1 - zCoord)){
		result = barryCentric(Vector3f(0, m_vHeights[gridX][gridZ] * m_fTerrainHeight, 0),
			Vector3f(1, m_vHeights[gridX + 1][gridZ] * m_fTerrainHeight, 0),
			Vector3f(0, m_vHeights[gridX][gridZ + 1] * m_fTerrainHeight, 1),
			Vector2f(xCoord, zCoord));

	}else {
		result = barryCentric(Vector3f(1, m_vHeights[gridX + 1][gridZ] * m_fTerrainHeight, 0),
			Vector3f(1, m_vHeights[gridX + 1][gridZ + 1] * m_fTerrainHeight, 1),
			Vector3f(0, m_vHeights[gridX][gridZ + 1] * m_fTerrainHeight, 1),
			Vector2f(xCoord, zCoord));
	}

	return result;
}

// -------------------
// Author: Rony Hanna
// Description: Helper function used to find the height of a triangle the player is currently on
// -------------------
float Terrain::barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const{
	float det = (p2[2] - p3[2]) * (p1[0] - p3[0]) + (p3[0] - p2[0]) * (p1[2] - p3[2]);
	float l1 = ((p2[2] - p3[2]) * (pos[0] - p3[0]) + (p3[0] - p2[0]) * (pos[1] - p3[2])) / det;
	float l2 = ((p3[2] - p1[2]) * (pos[0] - p3[0]) + (p1[0] - p3[0]) * (pos[1] - p3[2])) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1[1] + l2 * p2[1] + l3 * p3[1];
}

void Terrain::draw(const Camera& camera, DirectionalLight* directionLight, PointLight* pointLight, SpotLight* spotLight) {

	auto shader = Globals::shaderManager.getAssetPointer("terrain");

	shader->use();
	shader->loadInt("meshTexture", 0);
	shader->loadInt("rTexture", 1);
	shader->loadInt("gTexture", 2);
	shader->loadInt("bTexture", 3);
	shader->loadInt("blendMap", 4);
	shader->loadInt("grassNormalMap", 5);

	// Activate all the textures
	for (unsigned int i = 0; i < 6; ++i) {
		m_textures[i].bind(i);
	}

	shader->loadMatrix("model", m_model);
	shader->loadMatrix("view", camera.getViewMatrix());
	shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	shader->loadVector("viewPos", camera.getPosition());

	shader->loadVector("dirLight.direction", directionLight->getDirection());
	shader->loadVector("dirLight.ambient", directionLight->getAmbient());
	shader->loadVector("dirLight.diffuse", directionLight->getDiffuse());
	shader->loadVector("dirLight.specular", directionLight->getSpecular());
	shader->loadVector("dirLight.lightColour", directionLight->getColour());

	shader->loadVector("pointLight.position", pointLight->getPosition());
	shader->loadVector("pointLight.ambient", pointLight->getAmbient());
	shader->loadVector("pointLight.diffuse", pointLight->getDiffuse());
	shader->loadVector("pointLight.specular", pointLight->getSpecular());
	shader->loadVector("pointLight.lightColour", pointLight->getColour());
	shader->loadFloat("pointLight.constant", pointLight->getConstant());
	shader->loadFloat("pointLight.linear", pointLight->getLinear());
	shader->loadFloat("pointLight.quadratic", pointLight->getQuadratic());

	shader->loadVector("spotlight.position", spotLight->getPosition());
	shader->loadVector("spotlight.direction", spotLight->getDirection());
	shader->loadVector("spotlight.diffuse", spotLight->getDiffuse());
	shader->loadVector("spotlight.specular", spotLight->getSpecular());
	shader->loadFloat("spotlight.constant", spotLight->getConstant());
	shader->loadFloat("spotlight.linear", spotLight->getLinear());
	shader->loadFloat("spotlight.quadratic", spotLight->getQuadratic());
	shader->loadFloat("spotlight.cutOff", cosf(spotLight->getCutOff() * PI_ON_180));
	shader->loadFloat("spotlight.outerCutOff", cosf(spotLight->getOuterCutOff()* PI_ON_180));

	// Fog effect
	if (m_fog)
		shader->loadBool("fogActive", true);
	else
		shader->loadBool("fogActive", false);

	// Draw the terrain
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[ELEMENT_BUFFER]);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader->unuse();
}