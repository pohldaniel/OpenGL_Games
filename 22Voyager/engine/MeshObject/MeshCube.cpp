#include "MeshCube.h"

MeshCube::MeshCube(int uResolution, int vResolution) : MeshCube(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), true, true, false, false, uResolution, vResolution) { }

MeshCube::MeshCube(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) : MeshCube(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), generateTexels, generateNormals, generateTangents, generateNormalDerivatives, uResolution, vResolution) { }

MeshCube::MeshCube(const Vector3f &position, const Vector3f& size, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) {

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

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents + 2 * generateNormalDerivatives;

	m_center = m_position + m_size * 0.5f;

	buildMesh4Q();
}

MeshCube::~MeshCube() {}

void MeshCube::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

const Vector3f& MeshCube::getPosition() const {
	return m_position;
}

const Vector3f& MeshCube::getSize() const {
	return m_size;
}

const Vector3f& MeshCube::getCenter() const {
	return m_center;
}

void MeshCube::buildMesh() {

	float vStep = (1.0f / m_vResolution) * m_size[1];
	float uStep = (1.0f / m_uResolution) * m_size[0];

	//front
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j < m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = m_size[2];

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;
				Vector2f textureCoordinate = Vector2f(u, 1 - v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[2];

	//right
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = m_uResolution; j > 0; j--) {

			// Calculate vertex position on the surface of a quad
			float x = m_size[0];
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;				
				Vector2f textureCoordinate = Vector2f(u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//back
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = m_uResolution; j > 0; j--) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				m_texels.push_back(textureCoordinate);
				m_hasTexels = true;
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[2];

	//left
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j < m_uResolution; j++) {
			// Calculate vertex position on the surface of a quad
			float x = 0.0f;
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}
	
	vStep = (1.0f / m_vResolution) * m_size[2];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//bottom
	for (unsigned int i = 0; i <= m_vResolution ; i++) {
		for (unsigned int j = 0; j <= m_uResolution ; j++) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u,v);			
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[2];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//top
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = m_size[1];
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	int face = 0;
	for (unsigned int v = 0; v < m_vResolution; v++) {

		for (unsigned int u = 0; u < 4 * m_uResolution * (m_vResolution + 1); u++) {

			//Connect the penultimate vertex to the first
			if (u == ((m_uResolution - 1) + face * (m_uResolution * (1 + m_vResolution)))) {
				face++;

				m_indexBuffer.push_back(u + (1 + v) * m_uResolution);
				m_indexBuffer.push_back(u + v * (m_uResolution));
				m_indexBuffer.push_back((m_uResolution * (v + 1 + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));

				m_indexBuffer.push_back(u + v * (m_uResolution));
				m_indexBuffer.push_back((m_uResolution * (v + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));
				m_indexBuffer.push_back((m_uResolution * (v + 1 + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));

			}
			else if (v == 0 && u < m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) <= u && u < 2 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) * 2 <= u && u < 3 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) * 3 <= u && u < 4 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);
			}
		}
		face = 0;
	}

	for (unsigned short face = 0; face < 2; face++) {

		int k = (4 + face) * m_uResolution * (m_vResolution + 1) + face * (m_uResolution + 1);
		for (int i = k; i < k + m_vResolution; i++) {
			for (int j = k; j < k + m_uResolution; j++) {
				if (face % 2 == 0) {
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);

					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j + 1);
				}
				else {
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);

					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
				}
			}
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

void MeshCube::buildMesh4Q() {

	float vStep = (1.0f / m_vResolution) * m_size[1];
	float uStep = (1.0f / m_uResolution) * m_size[0];

	//front
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = m_size[2];

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[2];

	//right
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = m_size[0];
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 -u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[2];

	//left
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = 0.0f;
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[1];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//back
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				m_texels.push_back(textureCoordinate);
				m_hasTexels = true;
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[2];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//bottom
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_size[2];
	uStep = (1.0f / m_uResolution) * m_size[0];

	//top
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = m_size[1];
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateNormals) {
				m_normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, 1.0f - v);
				m_texels.push_back(textureCoordinate);
			}
		}
	}

	//calculate the indices
	for (short face = 0; face < 6; face++) {
		int k = (m_vResolution + 1) * (m_uResolution + 1) * face;
		for (int v = 0; v < m_vResolution; v++) {
			for (int u = 0; u < m_uResolution; u++) {

				if (face % 2 == 0) {
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));

					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u));
				}else {
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));

					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));				
				}
			}
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

int MeshCube::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshCube::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}