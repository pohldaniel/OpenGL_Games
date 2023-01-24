#include "Water.h"
#include <iostream>
#include "Terrain.h"
#include "Application.h"

Water::Water(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_waterShader = Globals::shaderManager.getAssetPointer("water");
	m_textures["dudv"] = &Globals::textureManager.get("water_dudv");
	m_textures["normal"] = &Globals::textureManager.get("water_normal");

	m_reflectionBuffer.create(Application::Width, Application::Height);
	m_reflectionBuffer.attachTexture(AttachmentTex::RGBA);
	m_reflectionBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);

	m_refractionBuffer.create(Application::Width, Application::Height);
	m_refractionBuffer.attachTexture(AttachmentTex::RGBA);
	m_refractionBuffer.attachTexture(AttachmentTex::DEPTH24);
}

Water::~Water() {

}

void Water::create(unsigned int resolution, unsigned int width, float waterLevel) {
	m_resolution = resolution;
	m_width = width;
	m_gridSpacing = static_cast<float>(m_width) / static_cast<float>(m_resolution);
	m_waterLevel = waterLevel;

	generateVertices(m_vertexBuffer);
	generateIndices(m_indexBuffer);
	createBuffer();
}


void Water::generateIndices(std::vector<unsigned int>& indexBuffer) {
	

	for (unsigned int z = 0; z < m_resolution; z++) {
		for (unsigned int x = 0; x < m_resolution; x++) {

			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      3		2 -* 3
			m_indexBuffer.push_back(z * (m_resolution + 1) + x);
			m_indexBuffer.push_back((z + 1) * (m_resolution + 1) + (x + 1));
			m_indexBuffer.push_back(z * (m_resolution + 1) + (x + 1));

			m_indexBuffer.push_back(z * (m_resolution + 1) + x);
			m_indexBuffer.push_back((z + 1) * (m_resolution + 1) + x);
			m_indexBuffer.push_back((z + 1) * (m_resolution + 1) + (x + 1));
		}
	}
}

void Water::generateVertices(std::vector<float>& vertexBuffer) {
	
	for (unsigned int z = 0; z <= m_resolution; ++z) {
		for (unsigned int x = 0; x <= m_resolution; ++x) {
			vertexBuffer.push_back(static_cast<float>(x) * m_gridSpacing); vertexBuffer.push_back(m_waterLevel); vertexBuffer.push_back(static_cast<float>(z) * m_gridSpacing);
			vertexBuffer.push_back(static_cast<float>(x) / static_cast<float>(m_resolution)); vertexBuffer.push_back(static_cast<float>(z) / static_cast<float>(m_resolution));
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
		}
	}
}

void Water::createBuffer() {

	m_totalIndices = m_resolution * m_resolution * 6;
	m_totalVertices = (m_resolution + 1) * (m_resolution + 1);

	short stride = 8;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, stride * sizeof(float) * m_totalVertices, &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	//normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(5 * sizeof(float)));

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexBuffer.size(), &m_indexBuffer[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}


void Water::setGridPosition(int x, int z){
	m_model.translate(static_cast<float>(x * m_width), 0.0f, static_cast<float>(z * m_width));

	Matrix4f model;
	model.translate(static_cast<float>(x * m_width), 0.0f, static_cast<float>(z * m_width));
	Matrix4f::Transpose(model);
	modelMTX.push_back(model);
}

void Water::render(const Camera& camera, const unsigned int &reflectionTexture, const unsigned int &refractionTexture, const unsigned int &refractionDepthTexture) {
	
	
	move += WAVE_SPEED * m_dt;
	if (move > 1.0f) move = 0.0f;
	
	glUseProgram(m_waterShader->m_program);
	m_waterShader->loadMatrix("u_projection", Globals::projection);
	m_waterShader->loadMatrix("u_view", camera.getViewMatrix());
	m_waterShader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
	m_waterShader->loadVector("cameraPos", camera.getPosition());

	m_waterShader->loadVector("lightPosition", Vector3f(9000.0f, 9000.0f, 9000.0f));
	m_waterShader->loadVector("lightColor", Vector3f(1.0f, 1.0f, 1.0f));

	m_waterShader->loadFloat("move", move);
	
	m_waterShader->loadInt("reflection", 0);
	m_waterShader->loadInt("refraction", 1);
	m_waterShader->loadInt("dudvMap", 2);
	m_waterShader->loadInt("normalMap", 3);
	m_waterShader->loadInt("depthMap", 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);

	m_textures["dudv"]->bind(2);
	m_textures["normal"]->bind(3);
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_totalIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Texture::Unbind();

	glUseProgram(0);
}

void Water::bindReflectionBuffer() {
	m_reflectionBuffer.bind();
}

void Water::bindRefractionBuffer() {
	m_refractionBuffer.bind();
}

Framebuffer Water::getReflectionBuffer() {
	return m_reflectionBuffer;
}

Framebuffer Water::getRefractionBuffer() {
	return m_refractionBuffer;
}

float Water::getWaterLevel() {
	return m_waterLevel;
}