#include "CubeBuffer.h"

CubeBuffer::CubeBuffer(unsigned int internalFormat, int size) {
	m_internalFormat = internalFormat;
	m_minFilter = GL_NEAREST;
	m_magFilter = GL_NEAREST;
	m_mode = GL_CLAMP_TO_EDGE;
	m_size = size;

	Texture::CreateTextureCube(m_texture, size, internalFormat, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture::CreateTextureCube(m_depthTexture, size, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE);
	
	m_fbo.create(size, size);
	
	createBuffer();

	m_targetVectors[0] = Vector3f(1.0f, 0.0f, 0.0f);
	m_targetVectors[1] = Vector3f(-1.0f, 0.0f, 0.0f);
	m_targetVectors[2] = Vector3f(0.0f, 1.0f, 0.0f);
	m_targetVectors[3] = Vector3f(0.0f, -1.0f, 0.0f);
	m_targetVectors[4] = Vector3f(0.0f, 0.0f, 1.0f);
	m_targetVectors[5] = Vector3f(0.0f, 0.0f, -1.0f);

	m_upVectors[0] = Vector3f(0.0f, -1.0f, 0.0f);
	m_upVectors[1] = Vector3f(0.0f, -1.0f, 0.0f);
	m_upVectors[2] = Vector3f(0.0f, 0.0f, 1.0f);
	m_upVectors[3] = Vector3f(0.0f, 0.0f, -1.0f);
	m_upVectors[4] = Vector3f(0.0f, -1.0f, 0.0f);
	m_upVectors[5] = Vector3f(0.0f, -1.0f, 0.0f);

	m_persMatrix.perspective(90.0f, 1.0f, m_nearPlane, m_farPlane);
	m_invPersMatrix.invPerspective(90.0f, 1.0f, m_nearPlane, m_farPlane);
	m_position.set(0.0f, 0.0f, 0.0f);
	m_viewMatrices.resize(6);
}

CubeBuffer::~CubeBuffer() {
	glDeleteTextures(1, &m_texture);

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void CubeBuffer::draw() {

	if (m_draw) {
		m_draw();
	}else {

		m_fbo.bind();
		m_shader->use();
		glBindVertexArray(m_vao);

		for (GLuint face = 0; face < 6; ++face) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_texture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_viewMatrix.lookAt(m_position, m_position + m_targetVectors[face], m_upVectors[face]);
			m_viewDirection = m_targetVectors[face];
			m_innerDraw();
		}
		glBindVertexArray(0);
		m_shader->unuse();
		m_fbo.unbind();
	}
}

void CubeBuffer::attachTexture(unsigned int face) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_texture, 0);
}

void CubeBuffer::attachLayerd() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo.getFramebuffer());
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glDrawBuffer(GL_FRONT);
	glReadBuffer(GL_FRONT);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeBuffer::bindVao() {
	glBindVertexArray(m_vao);
}

void CubeBuffer::unbindVao() {
	glBindVertexArray(0);
}

void CubeBuffer::updateAllViewMatrices() {
	m_viewMatrices[0].lookAt(m_position, m_position + m_targetVectors[0], m_upVectors[0]);
	m_viewMatrices[1].lookAt(m_position, m_position + m_targetVectors[1], m_upVectors[1]);
	m_viewMatrices[2].lookAt(m_position, m_position + m_targetVectors[2], m_upVectors[2]);
	m_viewMatrices[3].lookAt(m_position, m_position + m_targetVectors[3], m_upVectors[3]);
	m_viewMatrices[4].lookAt(m_position, m_position + m_targetVectors[4], m_upVectors[4]);
	m_viewMatrices[5].lookAt(m_position, m_position + m_targetVectors[5], m_upVectors[5]);
}

void CubeBuffer::updateViewMatrix(unsigned int face) {
	m_viewMatrix.lookAt(m_position, m_position + m_targetVectors[face], m_upVectors[face]);
	m_invViewMatrix.invLookAt(m_position, m_position + m_targetVectors[face],m_upVectors[face]);
	m_viewDirection = m_targetVectors[face];
}

void CubeBuffer::setFiltering(unsigned int minFilter) {
	m_minFilter = minFilter;
	m_magFilter = minFilter == 9985 || minFilter == 9987 ? GL_LINEAR : minFilter == 9984 || minFilter == 9986 ? GL_NEAREST : minFilter;
	Texture::SetFilter(m_texture, m_minFilter, m_magFilter, GL_TEXTURE_CUBE_MAP);
	Texture::SetFilter(m_depthTexture, m_minFilter, m_magFilter, GL_TEXTURE_CUBE_MAP);
}

void CubeBuffer::setWrapMode(unsigned int mode) {
	m_mode = mode;
	Texture::SetWrapMode(m_texture, mode, GL_TEXTURE_CUBE_MAP);
	Texture::SetWrapMode(m_depthTexture, mode, GL_TEXTURE_CUBE_MAP);
}

void CubeBuffer::resize(int size) {
	m_size = size;
	m_fbo.resize(m_size, m_size);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, m_size, m_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeBuffer::setShader(Shader* shader) {
	m_shader = shader;
}

unsigned int& CubeBuffer::getTexture() {
	return m_texture;
}

int CubeBuffer::getSize() {
	return m_size;
}

void CubeBuffer::createBuffer() {

	std::vector<float> vertex;

	vertex.push_back(-1.0f); vertex.push_back(-1.0f); vertex.push_back(1.0f);
	vertex.push_back( 1.0f); vertex.push_back(-1.0f); vertex.push_back(1.0f);
	vertex.push_back( 1.0f); vertex.push_back( 1.0f); vertex.push_back(1.0f);
	vertex.push_back(-1.0f); vertex.push_back( 1.0f); vertex.push_back(1.0f);

	vertex.push_back(-1.0f); vertex.push_back(-1.0f); vertex.push_back(-1.0f);
	vertex.push_back( 1.0f); vertex.push_back(-1.0f); vertex.push_back(-1.0f);
	vertex.push_back( 1.0f); vertex.push_back( 1.0f); vertex.push_back(-1.0f);
	vertex.push_back(-1.0f); vertex.push_back( 1.0f); vertex.push_back(-1.0f);

	unsigned short indices[] = {
		// positive X
		1, 5, 6,
		6, 2, 1,
		// negative X
		4, 0, 3,
		3, 7, 4,
		// positive Y
		3, 2, 6,
		6, 7, 3,
		// negative Y
		4, 5, 1,
		1, 0, 4,
		// positive Z
		0, 1, 2,
		2, 3, 0,
		// negative Z
		7, 6, 5,
		5, 4, 7
	};

	if (m_vbo) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else {

		short stride = 3;

		unsigned int ibo;
		glGenBuffers(1, &ibo);

		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glDeleteBuffers(1, &ibo);
	}
	vertex.clear();
	vertex.shrink_to_fit();
}

void CubeBuffer::setInnerDrawFunction(std::function<void()> fun) {
	m_innerDraw = fun;
}

void CubeBuffer::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

Framebuffer& CubeBuffer::getFramebuffer() {
	return m_fbo;
}

Shader* CubeBuffer::getShader() {
	return m_shader;
}

unsigned int& CubeBuffer::getVao() {
	return m_vao;
}

void CubeBuffer::drawRaw() {
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

const Matrix4f& CubeBuffer::getPerspectiveMatrix() {
	return m_persMatrix;
}

const Matrix4f& CubeBuffer::getViewMatrix() {
	return m_viewMatrix;
}


const Vector3f& CubeBuffer::getViewDirection() {
	return m_viewDirection;
}

const Matrix4f& CubeBuffer::getInvViewMatrix() {
	return m_invViewMatrix;
}
const Matrix4f& CubeBuffer::getInvPerspectiveMatrix() {
	return m_invPersMatrix;
}

const Vector3f& CubeBuffer::getPosition() {
	return m_position;
}

void CubeBuffer::unbind() {
	m_fbo.unbind();
}

const std::vector<Matrix4f>& CubeBuffer::getViewMatrices() {
	return m_viewMatrices;
}

void CubeBuffer::setPosition(const Vector3f& position) {
	m_position = position;
}