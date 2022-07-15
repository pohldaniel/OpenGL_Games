#include "Skybox.h"

SkyBox::SkyBox(const float scale, const Vector3f& position) {
	m_scale = scale;
	m_position = position;
	m_skyboxShader = Globals::shaderManager.getAssetPointer("skybox");

	m_cubemap = &Globals::cubemapManager.get("day");

	createBuffer();
}

SkyBox::~SkyBox() {

}

void SkyBox::toggleDayNight() {
	m_day = !m_day;
	m_cubemap = m_day ? &Globals::cubemapManager.get("day") : &Globals::cubemapManager.get("night");
}

void SkyBox::createBuffer() {

	m_vertices.push_back(-m_scale); m_vertices.push_back(-m_scale); m_vertices.push_back(m_scale);
	m_vertices.push_back(m_scale); m_vertices.push_back(-m_scale); m_vertices.push_back(m_scale);
	m_vertices.push_back(m_scale); m_vertices.push_back(m_scale); m_vertices.push_back(m_scale);
	m_vertices.push_back(-m_scale); m_vertices.push_back(m_scale); m_vertices.push_back(m_scale);

	m_vertices.push_back(-m_scale); m_vertices.push_back(-m_scale); m_vertices.push_back(-m_scale);
	m_vertices.push_back(m_scale); m_vertices.push_back(-m_scale); m_vertices.push_back(-m_scale);
	m_vertices.push_back(m_scale); m_vertices.push_back(m_scale); m_vertices.push_back(-m_scale);
	m_vertices.push_back(-m_scale); m_vertices.push_back(m_scale); m_vertices.push_back(-m_scale);

	m_model.translate(m_position[0], m_position[1] , m_position[2]);

	unsigned int indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	
	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	m_vertices.clear();
	m_vertices.shrink_to_fit();
}

void SkyBox::render(const Camera& camera) {
	Matrix4f view = camera.getViewMatrix();
	view[0][3] = 0.0f; view[1][3] = 0.0f; view[2][3] = 0.0f;
	
	glDepthFunc(GL_LEQUAL);
	glUseProgram(m_skyboxShader->m_program);
	m_skyboxShader->loadMatrix("u_transform", view * Globals::projection );
	m_cubemap->bind(0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Cubemap::Unbind();

	glUseProgram(0);
	glDepthFunc(GL_LESS);
}