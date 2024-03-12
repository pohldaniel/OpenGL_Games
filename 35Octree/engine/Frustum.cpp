#include "Frustum.h"

std::unique_ptr<Shader> Frustum::s_shaderFrustum = nullptr;

Frustum::Frustum() {
	
}

Frustum::~Frustum() {
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vboPos)
		glDeleteBuffers(1, &m_vboPos);

	if (m_vboCount)
		glDeleteBuffers(1, &m_vboCount);
}

void Frustum::init() {
	if (!s_shaderFrustum) {
		s_shaderFrustum = std::unique_ptr<Shader>(new Shader(FRUSTUM_VERTEX, FRUSTUM_FRGAMENT, false));
	}
}

bool Frustum::aabbVisible(const Vector3f* AABBVertices) {
	for (int i = 0; i < 6; i++) {
		if (Vector4f::Dot(m_planes[i], AABBVertices[m_origins[i]]) + m_planes[i][3] < 0) {
			return false;
		}
	}

	return true;
}

float Frustum::aabbDistance(const Vector3f* AABBVertices) {
	return Vector4f::Dot(m_planes[5], AABBVertices[m_origins[5]]);
}

void Frustum::updatePlane(const Matrix4f& perspective, const Matrix4f& view, const Matrix4f& model) {
	Matrix4f mvp = perspective * view * model;

	//Left
	m_planes[0][0] = mvp[0][3] + mvp[0][0];
	m_planes[0][1] = mvp[1][3] + mvp[1][0];
	m_planes[0][2] = mvp[2][3] + mvp[2][0];
	m_planes[0][3] = mvp[3][3] + mvp[3][0];
	//m_planes[0].normalize3();
	m_origins[0] = m_planes[0][2] < 0.0f ? (m_planes[0][1] < 0.0f ? (m_planes[0][0] < 0.0f ? 0 : 1) : (m_planes[0][0] < 0.0f ? 2 : 3)) : (m_planes[0][1] < 0.0f ? (m_planes[0][0] < 0.0f ? 4 : 5) : (m_planes[0][0] < 0.0f ? 6 : 7));

	//Right
	m_planes[1][0] = mvp[0][3] - mvp[0][0];
	m_planes[1][1] = mvp[1][3] - mvp[1][0];
	m_planes[1][2] = mvp[2][3] - mvp[2][0];
	m_planes[1][3] = mvp[3][3] - mvp[3][0];
	//m_planes[1].normalize3();
	m_origins[1] = m_planes[1][2] < 0.0f ? (m_planes[1][1] < 0.0f ? (m_planes[1][0] < 0.0f ? 0 : 1) : (m_planes[1][0] < 0.0f ? 2 : 3)) : (m_planes[1][1] < 0.0f ? (m_planes[1][0] < 0.0f ? 4 : 5) : (m_planes[1][0] < 0.0f ? 6 : 7));
	
	//Bottom
	m_planes[2][0] = mvp[0][3] + mvp[0][1];
	m_planes[2][1] = mvp[1][3] + mvp[1][1];
	m_planes[2][2] = mvp[2][3] + mvp[2][1];
	m_planes[2][3] = mvp[3][3] + mvp[3][1];
	//m_planes[2].normalize3();
	m_origins[2] = m_planes[2][2] < 0.0f ? (m_planes[2][1] < 0.0f ? (m_planes[2][0] < 0.0f ? 0 : 1) : (m_planes[2][0] < 0.0f ? 2 : 3)) : (m_planes[2][1] < 0.0f ? (m_planes[2][0] < 0.0f ? 4 : 5) : (m_planes[2][0] < 0.0f ? 6 : 7));

	//Top
	m_planes[3][0] = mvp[0][3] - mvp[0][1];
	m_planes[3][1] = mvp[1][3] - mvp[1][1];
	m_planes[3][2] = mvp[2][3] - mvp[2][1];
	m_planes[3][3] = mvp[3][3] - mvp[3][1];
	//m_planes[3].normalize3();
	m_origins[3] = m_planes[3][2] < 0.0f ? (m_planes[3][1] < 0.0f ? (m_planes[3][0] < 0.0f ? 0 : 1) : (m_planes[3][0] < 0.0f ? 2 : 3)) : (m_planes[3][1] < 0.0f ? (m_planes[3][0] < 0.0f ? 4 : 5) : (m_planes[3][0] < 0.0f ? 6 : 7));

	//Far
	m_planes[4][0] = mvp[0][3] - mvp[0][2];
	m_planes[4][1] = mvp[1][3] - mvp[1][2];
	m_planes[4][2] = mvp[2][3] - mvp[2][2];
	m_planes[4][3] = mvp[3][3] - mvp[3][2];
	//m_planes[4].normalize3();
	m_origins[4] = m_planes[4][2] < 0.0f ? (m_planes[4][1] < 0.0f ? (m_planes[4][0] < 0.0f ? 0 : 1) : (m_planes[4][0] < 0.0f ? 2 : 3)) : (m_planes[4][1] < 0.0f ? (m_planes[4][0] < 0.0f ? 4 : 5) : (m_planes[4][0] < 0.0f ? 6 : 7));

	//Near
	m_planes[5][0] = mvp[0][3] + mvp[0][2];
	m_planes[5][1] = mvp[1][3] + mvp[1][2];
	m_planes[5][2] = mvp[2][3] + mvp[2][2];
	m_planes[5][3] = mvp[3][3] + mvp[3][2];
	//m_planes[5].normalize3();
	m_origins[5] = m_planes[5][2] < 0.0f ? (m_planes[5][1] < 0.0f ? (m_planes[5][0] < 0.0f ? 0 : 1) : (m_planes[5][0] < 0.0f ? 2 : 3)) : (m_planes[5][1] < 0.0f ? (m_planes[5][0] < 0.0f ? 4 : 5) : (m_planes[5][0] < 0.0f ? 6 : 7));	
}

void Frustum::updateVbo(const Matrix4f& perspective, const Matrix4f& view) {
	if (m_debug) {
		float near = perspective[3][2] / (perspective[2][2] - 1);
		float heightNear = (2.0f / perspective[1][1]) * near;
		float widthNear = (heightNear *  perspective[1][1]) / perspective[0][0];

		float far = perspective[3][2] / (perspective[2][2] + 1);
		float heightFar = (2.0f / perspective[1][1]) * far;
		float widthFar = (heightFar  * perspective[1][1]) / perspective[0][0];

		const Vector3f& right = Vector3f(view[0][0], view[1][0], view[2][0]);
		const Vector3f& up = Vector3f(view[0][1], view[1][1], view[2][1]);
		const Vector3f& viewDirection = Vector3f(-view[0][2], -view[1][2], -view[2][2]);
		const Vector3f& pos = Vector3f(-(view[3][0] * view[0][0] + view[3][1] * view[0][1] + view[3][2] * view[0][2]),
			-(view[3][0] * view[1][0] + view[3][1] * view[1][1] + view[3][2] * view[1][2]),
			-(view[3][0] * view[2][0] + view[3][1] * view[2][1] + view[3][2] * view[2][2]));

		//worldSpace
		Vector3f centerNear = pos + viewDirection * near;
		Vector3f centerFar = pos + viewDirection * far;

		Vector3f nearBottomLeft = centerNear - up * (heightNear * 0.5f) - right * (widthNear * 0.5f);
		Vector3f nearTopLeft = centerNear + up * (heightNear * 0.5f) - right * (widthNear * 0.5f);
		Vector3f nearTopRight = centerNear + up * (heightNear * 0.5f) + right * (widthNear * 0.5f);
		Vector3f nearBottomRight = centerNear - up * (heightNear * 0.5f) + right * (widthNear * 0.5f);

		Vector3f farBottomLeft = centerFar - up * (heightFar * 0.5f) - right * (widthFar * 0.5f);
		Vector3f farTopLeft = centerFar + up * (heightFar * 0.5f) - right * (widthFar * 0.5f);
		Vector3f farTopRight = centerFar + up * (heightFar * 0.5f) + right * (widthFar * 0.5f);
		Vector3f farBottomRight = centerFar - up * (heightFar * 0.5f) + right * (widthFar * 0.5f);

		std::vector<Vector3f> vertex;

		vertex.push_back(farBottomLeft);
		vertex.push_back(farBottomRight);
		vertex.push_back(farTopRight);
		vertex.push_back(farTopLeft);

		vertex.push_back(nearBottomLeft);
		vertex.push_back(nearBottomRight);
		vertex.push_back(nearTopRight);
		vertex.push_back(nearTopLeft);

		if (!m_vao) {
			const unsigned short indicesFrustum[] = {
				4, 5, 6, 7,
				0, 1, 2, 3,
				1, 2, 6, 5,
				0, 3, 7, 4,
				0, 1, 5, 4,
				2, 3, 7, 6
			};

			const unsigned int vertexCount[] = {
				0, 1, 2, 3,
				4, 5, 6, 7
			};

			unsigned int iboFrustum;
			glGenBuffers(1, &iboFrustum);
			glGenBuffers(1, &m_vboPos);
			glGenBuffers(1, &m_vboCount);

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			//Position
			glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vector3f), nullptr, GL_DYNAMIC_DRAW);


			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			//Count
			glBindBuffer(GL_ARRAY_BUFFER, m_vboCount);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(unsigned int), vertexCount, GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboFrustum);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFrustum), indicesFrustum, GL_STATIC_DRAW);
			
			glBindVertexArray(0);
			glDeleteBuffers(1, &iboFrustum);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(Vector3f), &vertex[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Frustum::drawFrustm(const Matrix4f& projection, const Matrix4f& view, float distance) {
	if (!(m_debug && m_vao)) return;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", projection * view * Matrix4f::Translate(0.0f, 0.0f, distance));
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 1.0f, 1.0f));

	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

bool Frustum::intersectAABBFrustum(const Vector3f& position, const Vector3f& size) {
	Vector3f max = position + size;
	Vector3f min = position;

	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++) {
		int out = 0;
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(min[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(m_planes[i], Vector4f(max[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		if (out == 8) return false;
	}

	return true;
}

bool Frustum::IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Vector4f& plane) {

	Vector3f extents = size * 0.5f;
	Vector3f center = position + extents;

	float r = extents[0] * abs(plane[0]) + extents[1] * abs(plane[1]) + extents[2] * abs(plane[2]);
	float s = Vector4f::Dot(plane, center) - plane[3];
	return abs(s) <= r;
}

bool& Frustum::getDebug() {
	return m_debug;
}