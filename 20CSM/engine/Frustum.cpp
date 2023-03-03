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

	float near = perspective[3][2] / (perspective[2][2] - 1);	
	float heightNear = (2.0f / perspective[1][1] ) * near;
	float widthNear = (heightNear *  perspective[1][1]) / perspective[0][0];

	float far = perspective[3][2] / (perspective[2][2] + 1) * scale;
	float heightFar = (2.0f / perspective[1][1]) * far;
	float widthFar = (heightFar  * perspective[1][1]) / perspective[0][0];
	

	Vector3f nearBottomLeft = Vector3f(-0.5f * widthNear, -0.5f * heightNear, near);
	Vector3f nearTopRight = Vector3f(0.5f * widthNear, 0.5f * heightNear, near);
	Vector3f farBottomLeft = Vector3f(-0.5f * widthFar, -0.5f * heightFar, far);
	Vector3f farTopRight = Vector3f(0.5f * widthFar, 0.5f * heightFar, far);
	m_center = Vector3f(0.0f, 0.0f, -(far + near) * 0.5f);
	

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

void Frustum::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Frustum::draw(const Camera& camera, const Vector3f& position, const Vector3f& scale) {
	/*Vector3f direction = camera.getViewDirection();
	Vector3f xaxis = Vector3f::Cross(direction, Vector3f(0.0f, 1.0f, 0.0f));
	Vector3f::Normalize(xaxis);

	Vector3f yaxis = Vector3f::Cross(xaxis, direction);
	Vector3f::Normalize(yaxis);
	Matrix4f lightView;
	lightView[0][0] = xaxis[0];
	lightView[1][0] = yaxis[0];
	lightView[2][0] = -direction[0];
	lightView[3][0] = position[0];

	lightView[0][1] = xaxis[1];
	lightView[1][1] = yaxis[1];
	lightView[2][1] = -direction[1];
	lightView[3][1] = position[1];

	lightView[0][2] = xaxis[2];
	lightView[1][2] = yaxis[2];
	lightView[2][2] = -direction[2];
	lightView[3][2] = position[2];

	lightView[0][3] = 0.0f;
	lightView[1][3] = 0.0f;
	lightView[2][3] = 0.0f;
	lightView[3][3] = 1.0f;

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() *  Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f, position) * Matrix4f::Scale(scale[0], scale[1], scale[2], position) *  lightView);
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	drawRaw();

	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);*/

	Vector3f direction = camera.getViewDirection();
	Vector3f xaxis = Vector3f::Cross(direction, Vector3f(0.0f, 1.0f, 0.0f));
	Vector3f::Normalize(xaxis);

	Vector3f yaxis = Vector3f::Cross(xaxis, direction);
	Vector3f::Normalize(yaxis);
	Matrix4f lightView;
	lightView[0][0] = xaxis[0];
	lightView[1][0] = yaxis[0];
	lightView[2][0] = direction[0];
	lightView[3][0] = position[0];

	lightView[0][1] = xaxis[1];
	lightView[1][1] = yaxis[1];
	lightView[2][1] = direction[1];
	lightView[3][1] = position[1];

	lightView[0][2] = xaxis[2];
	lightView[1][2] = yaxis[2];
	lightView[2][2] = direction[2];
	lightView[3][2] = position[2];

	lightView[0][3] = 0.0f;
	lightView[1][3] = 0.0f;
	lightView[2][3] = 0.0f;
	lightView[3][3] = 1.0f;

	
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(s_shaderFrustum->m_program);
	s_shaderFrustum->loadMatrix("u_transform", camera.getPerspectiveMatrix() * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f, position)  * Matrix4f::Scale(scale[0], scale[1], scale[2], position) * lightView * Matrix4f::SIGN);
	s_shaderFrustum->loadVector("u_color", Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	drawRaw();

	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}