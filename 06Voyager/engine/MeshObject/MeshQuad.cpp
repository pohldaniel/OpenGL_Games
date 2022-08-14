#include "MeshQuad.h"

MeshQuad::MeshQuad(float width, float height, float groundLevel, bool generateTexels, bool generateNormals) {

	m_numBuffers = 4;

	m_width = width;
	m_height = height;
	m_groundLevel = groundLevel;
	m_generateTexels = generateTexels;
	m_generateNormals = generateNormals;

	m_hasTexels = false;

	m_uResolution = 49;
	m_vResolution = 49;

	m_transform = Transform();

	_modelMatrix = Matrix4f::IDENTITY;

	m_offset = Vector3f(m_width * 0.5, m_height * 0.5, 0.0f);
}

MeshQuad::MeshQuad(float width, float height, float groundLevel) : MeshQuad(width, height, groundLevel, true, true) {

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

void MeshQuad::buildMesh(){
	float vStep = (1.0f / m_vResolution) * m_height;
	float uStep = (1.0f /m_uResolution) * m_width;

	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {
			
			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_offset[0];
			float z = i * vStep - m_offset[1];
			float y = m_groundLevel;

			Vector3f position = Vector3f(x, y, z);
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

				Vector2f textureCoordinate = Vector2f(u, v);
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

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(m_numBuffers, m_vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);
 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	/*m_positions.clear();
	m_positions.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();*/

	m_isInitialized = true;
}

void MeshQuad::draw(const Camera camera) {

	glUseProgram(m_shader->m_program);
	
	m_texture->bind(0);
	m_shader->loadMatrix("u_projection", Globals::projection, false);
	m_shader->loadMatrix("u_view", camera.getViewMatrix(), false);
	m_shader->loadMatrix("u_model", _modelMatrix);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	Texture::Unbind();

	glUseProgram(0);

}

int MeshQuad::getNumberOfTriangles() {
	return m_numberOfTriangle;
}

void MeshQuad::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void MeshQuad::translate(float dx, float dy, float dz) {
	//m_modelMatrix.translate(dx, dy, dz);
	_modelMatrix.translate(dx, dy, dz);
}

void MeshQuad::scale(float a, float b, float c) {
	//m_modelMatrix.scale(a, b, c);
	_modelMatrix.scale(a, b, c);
}

void MeshQuad::setTransformation(const Matrix4f& model){
	_modelMatrix = model;
}