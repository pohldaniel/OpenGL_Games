#include "Skybox.h"

SkyBox::SkyBox(const float& dt, const float& fdt, const float scale, const Vector3f& position) : m_dt(dt), m_fdt(fdt){
	m_scale = scale;
	m_position = position;
	m_skyboxShader = Globals::shaderManager.getAssetPointer("skybox");

	m_cubemapDay = &Globals::cubemapManager.get("day");
	m_cubemapNight = &Globals::cubemapManager.get("night");


	createBuffer();
}

SkyBox::~SkyBox() {

}

void SkyBox::toggleDayNight() {
	
	if (!m_transitionEnd) {
		m_fadeOut = m_fadeIn;
		m_fadeIn = !m_fadeIn;
	}else {
		m_fadeIn = m_fadeOut;
		m_fadeOut = !m_fadeOut;
	}
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

void SkyBox::update() {
	m_rotation = m_rotation >= 360.0f ? 0 : m_rotation + m_rotationSpeed * m_dt;
	m_model.invRotate(Vector3f(0.0f, 1.0f, 0.0f), m_rotation);

	if (m_fadeIn) {	
		m_blend = m_blend <= 1.0f ? m_blend + m_transitionSpeed * m_dt : 1.0f;
		m_fadeIn = m_blend <= 1.0f;
		m_transitionEnd = !m_fadeIn;
	}

	if (m_fadeOut) {
		m_blend = m_blend >= 0.0f ? m_blend - m_transitionSpeed * m_dt : 0.0f;
		m_fadeOut = m_blend >= 0.0f;
		m_transitionEnd = m_fadeOut;
	}	
}

void SkyBox::render(const Camera& camera) {
	Matrix4f view = camera.getViewMatrix();
	view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
	
	glDepthFunc(GL_LEQUAL);
	glUseProgram(m_skyboxShader->m_program);
	m_skyboxShader->loadMatrix("u_projection", Globals::projection);
	m_skyboxShader->loadMatrix("u_view", view);
	m_skyboxShader->loadMatrix("u_model", m_model);
	m_skyboxShader->loadFloat("u_blendFactor", m_blend);
	m_skyboxShader->loadInt("u_day", 0);
	m_skyboxShader->loadInt("u_nigth", 1);
	m_cubemapDay->bind(0);
	m_cubemapNight->bind(1);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Cubemap::Unbind();

	glUseProgram(0);
	glDepthFunc(GL_LESS);
}