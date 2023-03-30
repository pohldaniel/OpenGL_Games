#include "MeshSphere.h"

MeshSphere::MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {
	m_radius = radius;
	m_position = position;
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

	m_numBuffers = 1 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_model = Matrix4f::IDENTITY;

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

	buildMesh();
}

MeshSphere::MeshSphere(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) : MeshSphere(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, generateTexels, generateNormals, generateTangents, generateNormalDerivatives) { }

MeshSphere::MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture) {

	m_radius = radius;
	m_position = position;
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

	m_numBuffers = 2 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_model = Matrix4f::IDENTITY;

	m_texture = std::make_shared<Texture>(texture);
	m_shader = std::make_shared<Shader>("shader/texture.vert", "shader/texture.frag");

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

	buildMesh();
}

MeshSphere::MeshSphere(const Vector3f &position, float radius, const std::string &texture) : MeshSphere(position, radius, true, true, false, false, texture) {}

MeshSphere::MeshSphere(float radius, const std::string &texture) : MeshSphere(Vector3f(0.0f, 0.0f, 0.0f), radius, true, true, false, false, texture) { }

MeshSphere::~MeshSphere() {}

void MeshSphere::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshSphere::buildMesh() {

	if (m_isInitialized) return;

	std::vector<Vector3f> tangents;
	std::vector<Vector3f> bitangents;
	std::vector<Vector3f> normalsDu;
	std::vector<Vector3f> normalsDv;

	float uAngleStep = (2.0f * PI) / float(m_uResolution);
	float vAngleStep = PI / float(m_vResolution);
	float vSegmentAngle, uSegmentAngle;

	for (int i = 0; i <= m_vResolution; ++i) {
		vSegmentAngle = PI * 0.5f - i * vAngleStep;
		
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);
													
		for (int j = 0; j <= m_uResolution; ++j) {
			uSegmentAngle = j * uAngleStep;
			
			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			float x = cosVSegment * cosUSegment;
			float z = cosVSegment * sinUSegment;
			m_positions.push_back(Vector3f(m_radius * x, m_radius * sinVSegment, m_radius * z) + m_position);

			if (m_generateTexels)
				m_texels.push_back(Vector2f(1.0f - (float)j / m_uResolution, 1.0f - (float)i / m_vResolution));

			if (m_generateNormals)
				m_normals.push_back(Vector3f(x, sinVSegment, z));

			if (m_generateTangents) {
				tangents.push_back(Vector3f(-sinUSegment * sinVSegment, 0.0, cosUSegment* sinVSegment).normalize());
				bitangents.push_back(Vector3f(cosVSegment*cosUSegment, -sinVSegment, cosVSegment*sinUSegment).normalize());
			}

			if (m_generateNormalDerivatives) {
				float n1u = -sinVSegment * sinUSegment;
				float n3u = sinVSegment * cosUSegment;

				Vector3f dndu = Vector3f(n1u, 0.0, n3u).normalize();
				normalsDu.push_back(dndu);

				float n1v = cosVSegment  * cosUSegment;
				float n2v = -sinVSegment;
				float n3v = cosVSegment  * sinUSegment;

				Vector3f dndv = Vector3f(n1v, n2v, n3v).normalize();
				normalsDv.push_back(dndv);
			}
		}
	}

	unsigned int k1, k2;
	for (int i = 0; i < m_vResolution; ++i){
		k1 = i * (m_uResolution + 1);
		k2 = k1 + m_uResolution + 1;

		for (int j = 0; j < m_uResolution; ++j, ++k1, ++k2) {

			if (i != 0) {
				m_indexBuffer.push_back(k1); m_indexBuffer.push_back(k2); m_indexBuffer.push_back(k1 + 1);
			}

			if (i != (m_vResolution - 1)) {
				m_indexBuffer.push_back(k1 + 1); m_indexBuffer.push_back(k2); m_indexBuffer.push_back(k2 + 1);
			}	
		}
	}

	m_drawCount = m_indexBuffer.size();

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

	/*m_positions.clear();
	m_positions.shrink_to_fit();*/
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	tangents.clear();
	tangents.shrink_to_fit();
	bitangents.clear();
	bitangents.shrink_to_fit();
	normalsDu.clear();
	normalsDu.shrink_to_fit();
	normalsDv.clear();
	normalsDv.shrink_to_fit();
	/*m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();*/

	m_isInitialized = true;
}

void MeshSphere::draw(const Camera camera) {
	glUseProgram(m_shader->m_program);

	m_texture->bind(0);
	m_shader->loadMatrix("u_modelView", m_model * camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void MeshSphere::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}