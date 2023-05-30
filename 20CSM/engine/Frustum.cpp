#include "Frustum.h"

std::unique_ptr<Shader> Frustum::s_shaderFrustum = nullptr;

Frustum::Frustum(const Matrix4f& perspective, float scale) {
	createBuffer(perspective, scale);
	if (!s_shaderFrustum) {
		s_shaderFrustum = std::unique_ptr<Shader>(new Shader(FRUSTUM_VERTEX, FRUSTUM_FRGAMENT, false));
	}
}

Frustum::~Frustum() {

}

void Frustum::createBuffer(const Matrix4f& perspective, float scale) {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	float near = perspective[3][2] / (perspective[2][2] - 1.0f);
	float heightNear = (2.0f / perspective[1][1] ) * near;
	float widthNear = (heightNear *  perspective[1][1]) / perspective[0][0];

	float far = perspective[3][2] / (perspective[2][2] + 1.0f) * scale;
	float heightFar = (2.0f / perspective[1][1]) * far;
	float widthFar = (heightFar  * perspective[1][1]) / perspective[0][0];
	
	Vector3f nearBottomLeft = Vector3f(-0.5f * widthNear, -0.5f * heightNear, near);
	Vector3f nearBottomRight = Vector3f(0.5f * widthNear, -0.5f * heightNear, near);
	Vector3f nearTopRight = Vector3f(0.5f * widthNear, 0.5f * heightNear, near);
	Vector3f farBottomLeft = Vector3f(-0.5f * widthFar, -0.5f * heightFar, far);
	Vector3f farBottomRight = Vector3f(0.5f * widthFar, -0.5f * heightFar, far);
	Vector3f farTopRight = Vector3f(0.5f * widthFar, 0.5f * heightFar, far);
	m_center = Vector3f(0.0f, 0.0f, (far + near) * 0.5f);
	
	std::vector<float> vertex;
	Vector3f pos = nearBottomLeft;
	Vector3f size = nearTopRight - nearBottomLeft;

	vertex.push_back(pos[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2] + size[2]);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2] + size[2]);

	pos = farBottomLeft;
	size = farTopRight - farBottomLeft;

	vertex.push_back(pos[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + size[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2]);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + size[1]); vertex.push_back(pos[2]);


	/*static const GLushort indices[] = {
		0, 2, 3,
		0, 1, 2,

		4, 6, 7,
		4, 5, 6,

		// right
		1, 5, 6,
		6, 2, 1,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};*/

	static const GLushort indices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		1, 2, 6, 5,
		0, 3, 7, 4,
		0, 1, 5, 4,
		2, 3, 7, 6
	};

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();

}

void Frustum::updatePlane(const Camera& camera, const Matrix4f& perspective, const Matrix4f& model){
	Matrix4f mvp = perspective * camera.getViewMatrix() * model;
	//Near
	m_planes[0].normal[0] = mvp[0][3] + mvp[0][2];
	m_planes[0].normal[1] = mvp[1][3] + mvp[1][2];
	m_planes[0].normal[2] = mvp[2][3] + mvp[2][2];
	m_planes[0].d		  = mvp[3][3] + mvp[3][2];
	m_planes[0].normalize();

	//Far
	m_planes[1].normal[0] = mvp[0][3] - mvp[0][2];
	m_planes[1].normal[1] = mvp[1][3] - mvp[1][2];
	m_planes[1].normal[2] = mvp[2][3] - mvp[2][2];
	m_planes[1].d		  = mvp[3][3] - mvp[3][2];
	m_planes[1].normalize();

	//Left
	m_planes[2].normal[0] = mvp[0][3] + mvp[0][0];
	m_planes[2].normal[1] = mvp[1][3] + mvp[1][0];
	m_planes[2].normal[2] = mvp[2][3] + mvp[2][0];
	m_planes[2].d		  = mvp[3][3] + mvp[3][0];
	m_planes[2].normalize();

	//Right
	m_planes[3].normal[0] = mvp[0][3] - mvp[0][0];
	m_planes[3].normal[1] = mvp[1][3] - mvp[1][0];
	m_planes[3].normal[2] = mvp[2][3] - mvp[2][0];
	m_planes[3].d		  = mvp[3][3] - mvp[3][0];
	m_planes[3].normalize();

	//Bottom
	m_planes[4].normal[0] = mvp[0][3] + mvp[0][1];
	m_planes[4].normal[1] = mvp[1][3] + mvp[1][1];
	m_planes[4].normal[2] = mvp[2][3] + mvp[2][1];
	m_planes[4].d		  = mvp[3][3] + mvp[3][1];
	m_planes[4].normalize();

	//Top
	m_planes[5].normal[0] = mvp[0][3] - mvp[0][1];
	m_planes[5].normal[1] = mvp[1][3] - mvp[1][1];
	m_planes[5].normal[2] = mvp[2][3] - mvp[2][1];
	m_planes[5].d		  = mvp[3][3] - mvp[3][1];
	m_planes[5].normalize();

	if (m_debug) {
		float near = perspective[3][2] / (perspective[2][2] - 1);
		float heightNear = (2.0f / perspective[1][1]) * near;
		float widthNear = (heightNear *  perspective[1][1]) / perspective[0][0];

		float far = perspective[3][2] / (perspective[2][2] + 1);
		float heightFar = (2.0f / perspective[1][1]) * far;
		float widthFar = (heightFar  * perspective[1][1]) / perspective[0][0];
		const Vector3f& pos = camera.getPosition();
		const Vector3f& up = camera.getCamY();
		const Vector3f& right = camera.getCamX();
		const Vector3f& viewDirection = camera.getViewDirection();

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

		std::vector<float> vertex;

		vertex.push_back(nearBottomLeft[0]); vertex.push_back(nearBottomLeft[1]); vertex.push_back(nearBottomLeft[2]);
		vertex.push_back(nearBottomRight[0]); vertex.push_back(nearBottomRight[1]); vertex.push_back(nearBottomRight[2]);
		vertex.push_back(nearTopRight[0]); vertex.push_back(nearTopRight[1]); vertex.push_back(nearTopRight[2]);
		vertex.push_back(nearTopLeft[0]); vertex.push_back(nearTopLeft[1]); vertex.push_back(nearTopLeft[2]);

		vertex.push_back(farBottomLeft[0]); vertex.push_back(farBottomLeft[1]); vertex.push_back(farBottomLeft[2]);
		vertex.push_back(farBottomRight[0]); vertex.push_back(farBottomRight[1]); vertex.push_back(farBottomRight[2]);
		vertex.push_back(farTopRight[0]); vertex.push_back(farTopRight[1]); vertex.push_back(farTopRight[2]);
		vertex.push_back(farTopLeft[0]); vertex.push_back(farTopLeft[1]); vertex.push_back(farTopLeft[2]);

		if (!m_vaoFrustum) {
			const GLushort indicesFrustum[] = {
				0, 1, 2, 3,
				4, 5, 6, 7,
				1, 2, 6, 5,
				0, 3, 7, 4,
				0, 1, 5, 4,
				2, 3, 7, 6
			};

			unsigned int iboFrustum;
			glGenBuffers(1, &iboFrustum);
			glGenBuffers(1, &m_vboFrustum);

			glGenVertexArrays(1, &m_vaoFrustum);
			glBindVertexArray(m_vaoFrustum);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboFrustum);
			glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

			//Position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboFrustum);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFrustum), indicesFrustum, GL_STATIC_DRAW);

			glBindVertexArray(0);
			glDeleteBuffers(1, &iboFrustum);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vboFrustum);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(float), &vertex[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Frustum::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Frustum::draw(const Camera& camera, const Vector3f& position, const Vector3f& scale) {
	//Matrix4f lightView = Matrix4f::Rotate(camera.getViewDirection(), position) * Matrix4f::SIGN;

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f, position)  * Matrix4f::Scale(scale[0], scale[1], scale[2], position) * camera.getRotationMatrix(position) );
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	drawRaw();
	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

void Frustum::drawFrustm(const Camera& camera, const Vector3f& debugShift) {
	if (!m_debug) return;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_shaderFrustum->m_program);
	// better debugging
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * camera.getViewMatrix() * Matrix4f::Translate(debugShift));
	//s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * camera.getViewMatrix());
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 1.0f, 1.0f));

	glBindVertexArray(m_vaoFrustum);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

bool Frustum::intersectAABBFrustum(const Vector3f& position, const Vector3f& size) {
	Vector3f max = position + size;
	Vector3f min = position;
	
	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++) {
		int out = 0;
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(min[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(max[0], min[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(min[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(max[0], max[1], min[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(min[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(max[0], min[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(min[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		out += ((Vector4f::Dot(Vector4f(m_planes[i].normal, m_planes[i].d), Vector4f(max[0], max[1], max[2], 1.0f)) < 0.0) ? 1 : 0);
		if (out == 8) return false;
	}

	return true;
}

bool Frustum::IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Plane& plane) {
	Vector3f max = position + size;
	Vector3f min = position;
	Vector3f center = (max + min) * 0.5f;
	Vector3f extents = max - center;

	float r = extents[0] * abs(plane.normal[0]) + extents[1] * abs(plane.normal[1]) + extents[2] * abs(plane.normal[2]);
	float s = Vector3f::Dot(plane.normal, center) - plane.d;
	return abs(s) <= r;


}