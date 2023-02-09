#include "MeshQuad.h"

MeshQuad::MeshQuad(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) : MeshQuad(Vector3f(-1.0f, 0.0f, -1.0f), Vector2f(2.0f, 2.0f), generateTexels, generateNormals, generateTangents, generateNormalDerivatives) {

}

MeshQuad::MeshQuad(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {

	m_position = position;
	m_size = size;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;

	m_hasTexels = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasNormalDerivatives = false;

	m_isInitialized = false;

	m_uResolution = 49;
	m_vResolution = 49;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents + 2 * generateNormalDerivatives;

	m_transform = Transform();
	m_model = Matrix4f::IDENTITY;

	m_center = m_position + Vector3f(m_size[0], 0.0f, m_size[1]) * 0.5f;
	buildMesh();
}

MeshQuad::MeshQuad(const Vector3f &position, const Vector2f& size) : MeshQuad(position, size, true, true, false, false) {

}

MeshQuad::~MeshQuad() {}

void MeshQuad::setShader(Shader* shader) {
	m_shader = std::make_shared<Shader>(shader);
}

void MeshQuad::setTexture(Texture* texture) {
	m_texture.reset(texture);
}

void MeshQuad::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

int MeshQuad::getNumberOfTriangles() {
	return m_numberOfTriangle;
}

void MeshQuad::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void MeshQuad::translate(float dx, float dy, float dz) {
	m_model.translate(dx, dy, dz);
}

void MeshQuad::scale(float a, float b, float c) {
	m_model.scale(a, b, c);
}

void MeshQuad::setTransformation(const Matrix4f& model) {
	m_model = model;
}

const Vector3f& MeshQuad::getPosition() const {
	return m_position;
}

const Vector2f& MeshQuad::getSize() const {
	return m_size;
}

const Vector3f& MeshQuad::getCenter() const {
	return m_center;
}

void MeshQuad::buildMesh(){
	float vStep = (1.0f / m_vResolution) * m_size[1];
	float uStep = (1.0f /m_uResolution) * m_size[0];

	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {
			
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float z = i * vStep;
			float y = 0.0f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	if (m_generateTexels) {

		for (unsigned int i = 0; i <= m_vResolution; i++) {
			for (int j = 0; j <= m_uResolution; j++) {

				// Calculate texels on the surface of a quad
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, 1.0f - v);
				m_texels.push_back(textureCoordinate);
			}
		}
		m_hasTexels = true;
	}

	//calculate the indices
	for (int z = 0; z < m_vResolution ; z++) {
		for (int x = 0; x < m_uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			m_indexBuffer.push_back(z * (m_uResolution + 1) + x);
			m_indexBuffer.push_back((z + 1) * (m_uResolution + 1) + x + 1);			
			m_indexBuffer.push_back(z * (m_uResolution + 1) + x + 1);

			m_indexBuffer.push_back(z * (m_uResolution + 1) + x);
			m_indexBuffer.push_back((z + 1) * (m_uResolution + 1) + x);			
			m_indexBuffer.push_back((z + 1) * (m_uResolution + 1) + x + 1);
		}
	}

	m_drawCount = m_indexBuffer.size();
	m_numberOfTriangle = m_drawCount / 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(m_numBuffers, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (m_generateTexels) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (m_generateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_generateTexels ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	//m_positions.clear();
	//m_positions.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	//m_indexBuffer.clear();
	//m_indexBuffer.shrink_to_fit();

	m_isInitialized = true;
}

void MeshQuad::draw(const Camera camera) {

	glUseProgram(m_shader->m_program);
	
	m_texture->bind(0);
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix());
	m_shader->loadMatrix("u_view", camera.getViewMatrix());
	m_shader->loadMatrix("u_model", m_model);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	Texture::Unbind();

	glUseProgram(0);

}

void MeshQuad::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

